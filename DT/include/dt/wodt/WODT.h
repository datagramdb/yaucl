//
// Created by giacomo on 02/10/24.
// This is a C++ translation from this Python implementation: https://github.com/TorshaMajumder/Ensembles_of_Oblique_Decision_Trees
// (https://github.com/TorshaMajumder/Ensembles_of_Oblique_Decision_Trees/blob/master/Decision_trees/WODT.py)
// This provides the implementation for this paper: https://ojs.aaai.org/index.php/AAAI/article/view/4505
//

#ifndef DT_WODT_H
#define DT_WODT_H

#include "lbfgsb.h"

static inline double sigmoid(double z) {
    if (z<-500) z=-500;
    return 1.0/(std::exp(-z)+1.0);
}

class WODT : public optimize::Function
{
private:
    static constexpr optimize::Scalar b = 100;
    const optimize::Matrix& subFeatures_X;
    const optimize::Vector& Y;
    int maxClass;
    static constexpr double epsilonepsilon = std::numeric_limits<double>::epsilon();
    static Eigen::IOFormat HeavyFmt;

public:
    WODT(const optimize::Matrix& subFeatures_X,
         const optimize::Vector& Y,
         int maxClass) : subFeatures_X{subFeatures_X}, Y{Y}, maxClass{maxClass} {}


    optimize::Scalar computeValue(const optimize::Vector& a) override;
    optimize::Vector computeGradient(const optimize::Vector& a) override;
};


#endif //DT_WODT_H
