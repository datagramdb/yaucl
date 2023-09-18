#include <yaucl/learning/MCL.h>

int main(void) {
    yaucl::learning::MCL::SparseMatrix<double> m=yaucl::learning::MCL::SparseMatrix<double>(4,4);
    m.coeffRef(0,0) = 0.5;
    m.coeffRef(0,1) = 0.5;
    m.coeffRef(1,0) = 1;
    m.coeffRef(2,3) = 0.75;
    m.coeffRef(2,2) = 0.25;
    m.coeffRef(3,2) = 0.75;
    m.coeffRef(3,1) = 0.25;
    yaucl::learning::MCL::normalizations type = yaucl::learning::MCL::SYM_NORMALIZED_LAPLACIAN;

    auto result = perform_MCL_clustering(m, type, 2, 100, false);

    return 0;
}