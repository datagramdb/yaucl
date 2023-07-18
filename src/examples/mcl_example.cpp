#include <yaucl/learning/MCL.h>

int main(void) {
    Eigen::SparseMatrix<double> m(4,4);
    m.coeffRef(0,0) = 0.5;
    m.coeffRef(0,1) = 0.5;
    m.coeffRef(1,0) = 1;
    m.coeffRef(2,3) = 0.75;
    m.coeffRef(2,2) = 0.25;
    m.coeffRef(3,2) = 0.75;
    m.coeffRef(3,1) = 0.25;
    m = m + Eigen::MatrixXd::Identity(m.rows(),m.cols());
    MCL_NORMALISED(m); // (A)
    double consideration_threshold = 0.000001;

    normalizations type = SYM_NORMALIZED_LAPLACIAN;
    switch (type) {
        case MCL_NORMALISATION:  // (A)
            break;

        case SIMPLE_LAPLACIAN:
            m = LAPLACIAN_DIAG(m) - m;
            break;

        case SYM_NORMALIZED_LAPLACIAN: {
            auto d = LAPLACIAN_DIAG(m);
            auto sqrt = d.cwisePow(0.5);
            m = Eigen::MatrixXd::Identity(m.rows(),m.cols()) -sqrt * m * sqrt ;
        }
            break;

        case RANDOM_WALK_NORMALIZED: {
            m =Eigen::MatrixXd::Identity(m.rows(),m.cols())-LAPLACIAN_DIAG(m).cwisePow(-1)*m;
        }
    }

    MCL(m, 2, 100);
    std::vector<std::vector<size_t>> clusters;
    std::vector<size_t> current;
    for (size_t i = 0; i<m.rows(); i++) {
        current.clear();
        Eigen::SparseVector<double>  row = m.row(i);
        for (Eigen::SparseVector<double>::InnerIterator i_(row); i_; ++i_){
            if (i_.value() > consideration_threshold)
                current.emplace_back(i_.index() );
        }
        if (!current.empty())
            clusters.emplace_back(current);
    }

    return 0;
}