//
// Created by giacomo on 18/07/23.
//

#ifndef KNOBAB_SERVER_MCL_H
#define KNOBAB_SERVER_MCL_H

#include <iostream>
#include <vector>

#include <Eigen/Sparse>
#include <Eigen/Dense>

static inline void MCL_INFLATE(Eigen::SparseMatrix<double>& m, double pow){
    m = m.cwisePow(pow);
}

static inline std::vector<double> MCL_DIMSUMS(Eigen::SparseMatrix<double>& m, bool cols = true){
    std::vector<double> colSum(cols ? m.cols() : m.rows(), 0.0);
    for (int k = 0; k < m.outerSize(); ++k){
        for (Eigen::SparseMatrix<double>::InnerIterator it(m, k); it; ++it){
            colSum[cols ? it.col() : it.row()] += it.value();
        }
    }
    return colSum;
}

static inline void MCL_NORMALISED(Eigen::SparseMatrix<double>& m){
    std::vector<double> colSum = MCL_DIMSUMS(m, true);
    for (int k = 0; k < m.outerSize(); ++k){
        for (Eigen::SparseMatrix<double>::InnerIterator it(m, k); it; ++it){
            it.valueRef() = it.value() / colSum.at(it.col());
        }
    }
}

static inline void MCL(Eigen::SparseMatrix<double>& m, double inflate, size_t iterate) {
    for (size_t i = 0; i<iterate; i++) {
        auto oldM = m;
        MCL_NORMALISED(m);
        m = m * m;
        MCL_INFLATE(m, inflate);
        MCL_NORMALISED(m);
        if ((oldM - m).cwiseAbs().cwiseTypedLessOrEqual(std::numeric_limits<double>::epsilon()).sum() == m.cols() * m.rows()) {
            return;
        }
    }
}

static inline Eigen::SparseMatrix<double> LAPLACIAN_DIAG(Eigen::SparseMatrix<double>& m) {
    auto t = MCL_DIMSUMS(m, false);
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


#endif //KNOBAB_SERVER_MCL_H
