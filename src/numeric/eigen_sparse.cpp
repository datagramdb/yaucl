//
// Created by giacomo on 05/07/21.
//

#include <yaucl/numeric/eigen_sparse.h>

yaucl::numeric::svector yaucl::numeric::fromStdVector(const std::vector<double> &marking) {
    svector result{static_cast<Eigen::Index>(marking.size())};
    for (size_t i = 0, N = marking.size(); i<N; i++)
        if (marking[i] != 0)
            result.coeffRef(i) = marking[i];
    return result;
}

std::vector<double> yaucl::numeric::fromSVector(const yaucl::numeric::svector &vec) {
    std::vector<double> mark(vec.size());
    for (svector::InnerIterator it1(vec); it1; ++it1) {
        mark[it1.index()] = it1.value();
    }
    return mark;
}

