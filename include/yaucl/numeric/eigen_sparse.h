//
// Created by giacomo on 15/01/21.
//

#ifndef INCONSISTENCY_DETECTOR_EIGEN_SPARSE_H
#define INCONSISTENCY_DETECTOR_EIGEN_SPARSE_H

#include <Eigen/SparseCore>
#include <vector>

namespace yaucl {
    namespace numeric {
        using svector = Eigen::SparseVector<double> ;
        svector fromStdVector(const std::vector<double>& marking);
        std::vector<double> fromSVector(const svector& vec);
    }
}

#endif //INCONSISTENCY_DETECTOR_EIGEN_SPARSE_H
