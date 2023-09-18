//
// Created by giacomo on 18/07/23.
//

#ifndef KNOBAB_SERVER_MCL_H
#define KNOBAB_SERVER_MCL_H

#include <iostream>
#include <vector>

#include <Eigen/Sparse>
#include <Eigen/Dense>

namespace yaucl::learning::MCL {
    template<typename T> using SparseMatrix = Eigen::SparseMatrix<T, Eigen::RowMajor>;

    template<typename T>T VECT(SparseMatrix<T>& left,
                               SparseMatrix<T>& right,
                               T init,
                               T curr,
                               const std::function<T(T,T)>& fun_cmp,
                               const std::function<T(T,T)>& combine){
        for (size_t k = 0, n = std::max(left.outerSize(), right.outerSize()); k<n; k++) {
            typename SparseMatrix<T>::InnerIterator itL(left, k);
            typename SparseMatrix<T>::InnerIterator itR(right, k);
            while (itL) {
                if (!itR)
                    // Finished range 2, include the rest of range 1:
                {
                    while (itL) {
                        init = combine(init, fun_cmp(itL.value(),0)); ++itL;
                    }
                    break;
                }

                if (itR.row() == itL.row()) {
                    init = combine(init, fun_cmp(itL.value(),itR.value()));
                    ++itR;
                    ++itL;
                } else if (itL.row() < itR.row())
                {
                    init = combine(init, fun_cmp(itL.value(),0)); ++itL;
                } else {
                    init = combine(init, fun_cmp(0,itR.value())); ++itR;
                }
            }
            while (itR) {
                init = combine(init, fun_cmp(0,itR.value())); ++itR;
            }
        }
        return init;
    }

    static inline void INFLATE(SparseMatrix<double>& m, double pow){
        m = m.cwisePow(pow);
    }

    static inline std::vector<double> DIMSUMS(SparseMatrix<double>& m, bool cols = true){
        std::vector<double> colSum(cols ? m.cols() : m.rows(), 0.0);
        for (int k = 0; k < m.outerSize(); ++k){
            for (SparseMatrix<double>::InnerIterator it(m, k); it; ++it){
                colSum[cols ? it.col() : it.row()] += it.value();
            }
        }
        return colSum;
    }

    static inline void NORMALISED(SparseMatrix<double>& m){
        std::vector<double> colSum = DIMSUMS(m, true);
        for (int k = 0; k < m.outerSize(); ++k){
            for (SparseMatrix<double>::InnerIterator it(m, k); it; ++it){
                it.valueRef() = it.value() / colSum.at(it.col());
            }
        }
    }

    static inline void MCL(SparseMatrix<double>& m, double inflate, size_t iterate) {
        for (size_t i = 0; i<iterate; i++) {
            auto oldM = m;
            NORMALISED(m);
            m = m * m;
            INFLATE(m, inflate);
            NORMALISED(m);

            // More efficient than computing (oldM - m).cwiseAbs().cwiseTypedLessOrEqual(std::numeric_limits<double>::epsilon()).sum()!
            double result = VECT<double>(oldM, m, 0.0, 0.0, [](double l, double r) {
                return std::abs(l-r) <= std::numeric_limits<double>::epsilon() ? 1.0 : 0.0;
            }, [](double l, double r) {
                return l+r;
            });

            if (result ==((double) m.cols() * m.rows())) {
                return;
            }
        }
    }

    static inline SparseMatrix<double> LAPLACIAN_DIAG(SparseMatrix<double>& m) {
        auto t = DIMSUMS(m, false);
        typedef Eigen::Triplet<double> T;
        std::vector<Eigen::Triplet<double>> tripletList;
        tripletList.reserve(t.size());
        for(size_t i = 0; i<t.size(); i++) {
            tripletList.emplace_back(i,i,t.at(i));
        }
        Eigen::SparseMatrix<double> diag(t.size(), t.size());
        diag.setFromTriplets(tripletList.begin(), tripletList.end());
        return diag;
    }

    enum normalizations {
        MCL_NORMALISATION,
        SIMPLE_LAPLACIAN,
        SYM_NORMALIZED_LAPLACIAN,
        RANDOM_WALK_NORMALIZED
    };

    static inline
    std::vector<std::vector<size_t>> perform_MCL_clustering(SparseMatrix<double>& m,
                                                            normalizations type,
                                                            double inflate,
                                                            size_t iterate,
                                                            bool remove_singletons = true,
                                                            double consideration_threshold = 0.000001) {

        SparseMatrix<double> I(m.rows(), m.cols());
        I.setIdentity();
        m = m+I;
        NORMALISED(m); // (A)
        switch (type) {
            case MCL_NORMALISATION:  // (A)
                break;

            case SIMPLE_LAPLACIAN:
                m = LAPLACIAN_DIAG(m) - m;
                break;

            case SYM_NORMALIZED_LAPLACIAN: {
                auto d = LAPLACIAN_DIAG(m);
                auto sqrt = d.cwisePow(0.5);
                m = I-sqrt * m * sqrt ;
            } break;

            case RANDOM_WALK_NORMALIZED: {
                m = LAPLACIAN_DIAG(m).cwisePow(-1)*m;
                m = I-m;
            }
        }

        MCL(m, inflate, iterate);
        std::vector<std::vector<size_t>> clusters;
        std::vector<size_t> current;
        for (size_t i = 0; i<m.rows(); i++) {
            current.clear();
            Eigen::SparseVector<double>  row = m.row(i);
            for (Eigen::SparseVector<double>::InnerIterator i_(row); i_; ++i_){
                if (i_.value() > consideration_threshold)
                    current.emplace_back(i_.index() );
            }
            if ((!current.empty()) && (!remove_singletons || (current.size() >1)))
                clusters.emplace_back(current);
        }
        return clusters;
    }
}

#endif //KNOBAB_SERVER_MCL_H
