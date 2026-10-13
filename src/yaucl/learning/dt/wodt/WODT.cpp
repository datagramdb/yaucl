//
// Created by giacomo on 02/10/24.
//

#include "yaucl/learning/dt/wodt/WODT.h"

//Eigen::IOFormat WODT::HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");



optimize::Scalar WODT::computeValue(const optimize::Vector &a) {
    auto paras = a.tail(a.size() - 1);
    optimize::Vector p = ((subFeatures_X * paras).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
    auto w_R = p;
    auto w_L = Eigen::VectorXf::Ones(w_R.size()) - w_R;
    double w_R_sum = w_R.sum();
    double w_L_sum = w_L.sum();

    Eigen::VectorXf w_R_eachClass = Eigen::VectorXf::Zero(maxClass+1);
    Eigen::VectorXf w_L_eachClass = Eigen::VectorXf::Zero(maxClass+1);
    for (int k = 0; k <= maxClass; ++k) {
        w_R_eachClass(k) = (Y.array() == k).select(w_R, 0.f).sum();
        w_L_eachClass(k) = (Y.array() == k).select(w_L, 0.f).sum();
    }

    double fun = w_L_sum * std::log2(w_L_sum + epsilonepsilon) +
                 w_R_sum * std::log2(w_R_sum + epsilonepsilon) -
                 (w_R_eachClass.array() * (w_R_eachClass.array() + epsilonepsilon).log2()).sum() -
                 (w_L_eachClass.array() * (w_L_eachClass.array() + epsilonepsilon).log2()).sum();

    return fun;
}

optimize::Vector WODT::computeGradient(const optimize::Vector &a) {

    Eigen::VectorXf paras = a.tail(a.size() - 1);
    double threshold = a(0);
    auto Mx = subFeatures_X * paras;
    optimize::Vector p = ((Mx).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
//        VectorXd p = sigmoid((subFeatures_X * paras) - threshold);
    auto w_R = p;
    auto w_L = Eigen::VectorXf::Ones(w_R.size()) - w_R;

    Eigen::VectorXf w_R_eachClass = Eigen::VectorXf::Zero(maxClass+1);
    Eigen::VectorXf w_L_eachClass = Eigen::VectorXf::Zero(maxClass+1);
    for (int k = 0; k <= maxClass; ++k) {
        w_R_eachClass(k) = (Y.array() == k).select(w_R, 0).sum();//(Y.array() == k).cast<double>().dot(w_R);
        w_L_eachClass(k) = (Y.array() == k).select(w_L, 0).sum(); //(Y.array() == k).cast<double>().dot(w_L);
    }

    auto wrExp = w_R_eachClass(Y);
    auto wlExp = w_L_eachClass(Y);
    Eigen::VectorXf la = (wrExp.array() * w_R.sum() + epsilonepsilon).log2() -
                         (wlExp.array() * w_L.sum() + epsilonepsilon).log2();
    Eigen::VectorXf beta = la.array() * p.array() * (1 - p.array());
    Eigen::VectorXf jac = Eigen::VectorXf::Zero(a.size());
    jac(0) = -beta.sum();
    jac.tail(a.size() - 1) = subFeatures_X.transpose() * beta;
    return jac;
}
