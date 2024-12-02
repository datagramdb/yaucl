//
// Created by giacomo on 02/10/24.
//

#include "dt/wodt/WODT.h"

Eigen::IOFormat WODT::HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

optimize::Scalar WODT::computeValue(const optimize::Vector &a) {
    Eigen::VectorXd paras = a.tail(a.size() - 1);
//    double threshold = a(0);
//        VectorXd p = sigmoid((subFeatures_X * paras) - threshold);
    optimize::Vector p = ((subFeatures_X * paras).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
    auto w_R = p;
    auto w_L = Eigen::VectorXd::Ones(w_R.size()) - w_R;
    double w_R_sum = w_R.sum();
    double w_L_sum = w_L.sum();
//    std::cout << w_R.format(HeavyFmt) << std::endl;
//    std::cout << w_L.format(HeavyFmt) << std::endl;

    Eigen::VectorXd w_R_eachClass = Eigen::VectorXd::Zero(maxClass+1);
    Eigen::VectorXd w_L_eachClass = Eigen::VectorXd::Zero(maxClass+1);
    for (int k = 0; k <= maxClass; ++k) {
        w_R_eachClass(k) = (Y.array() == k).select(w_R, 0).sum();//(Y.array() == k).cast<double>().dot(w_R);
        w_L_eachClass(k) = (Y.array() == k).select(w_L, 0).sum(); //(Y.array() == k).cast<double>().dot(w_L);
    }

//    auto a1 = w_L_sum * std::log2(w_L_sum + epsilonepsilon);
//    auto a2 = w_R_sum * std::log2(w_R_sum + epsilonepsilon);
//    auto a3 = (w_R_eachClass.array() * (w_R_eachClass.array() + epsilonepsilon).log2()).sum();
//    auto a4 = (w_L_eachClass.array() * (w_L_eachClass.array() + epsilonepsilon).log2()).sum();
    double fun = w_L_sum * std::log2(w_L_sum + epsilonepsilon) +
                 w_R_sum * std::log2(w_R_sum + epsilonepsilon) -
                 (w_R_eachClass.array() * (w_R_eachClass.array() + epsilonepsilon).log2()).sum() -
                 (w_L_eachClass.array() * (w_L_eachClass.array() + epsilonepsilon).log2()).sum();

//    std::cout << w_L_sum << "--> " << fun << std::endl;
    return fun;
//        optimize::Vector paras = x.segment(1, x.size()-1);
//        auto threshold = x.coeffRef(0);
//        auto w_R = p;
//        auto w_L = 1 - w_R.array();
//        auto w_R_sum = w_R.sum();
//        auto w_L_sum = w_L.sum();
//        optimize::Scalar fun = 0;
//        fun += w_L_sum * std::log2(w_L_sum + epsilonepsilon);
//        fun += w_R_sum * std::log2(w_R_sum + epsilonepsilon);
//        for (int i = 0; i<=maxClass; i++) {
//            double w_R_eachClass_i = (Y.array() == i).select(w_R, 0).sum();
//            fun -= (w_R_eachClass_i * std::log2(w_R_eachClass_i + epsilonepsilon));
//            double w_L_eachClass_i = (Y.array() == i).select(w_L, 0).sum();
//            fun -= (w_L_eachClass_i * std::log2(w_L_eachClass_i + epsilonepsilon));
//        }
//        return fun;
}

optimize::Vector WODT::computeGradient(const optimize::Vector &a) {

    Eigen::VectorXd paras = a.tail(a.size() - 1);
    double threshold = a(0);
    auto Mx = subFeatures_X * paras;
    optimize::Vector p = ((Mx).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
//        VectorXd p = sigmoid((subFeatures_X * paras) - threshold);
    auto w_R = p;
    auto w_L = Eigen::VectorXd::Ones(w_R.size()) - w_R;

    Eigen::VectorXd w_R_eachClass = Eigen::VectorXd::Zero(maxClass+1);
    Eigen::VectorXd w_L_eachClass = Eigen::VectorXd::Zero(maxClass+1);
    for (int k = 0; k <= maxClass; ++k) {
        w_R_eachClass(k) = (Y.array() == k).select(w_R, 0).sum();//(Y.array() == k).cast<double>().dot(w_R);
        w_L_eachClass(k) = (Y.array() == k).select(w_L, 0).sum(); //(Y.array() == k).cast<double>().dot(w_L);
    }

    auto wrExp = w_R_eachClass(Y);
    auto wlExp = w_L_eachClass(Y);
    Eigen::VectorXd la = (wrExp.array() * w_R.sum() + epsilonepsilon).log2() -
                         (wlExp.array() * w_L.sum() + epsilonepsilon).log2();
    Eigen::VectorXd beta = la.array() * p.array() * (1 - p.array());
    Eigen::VectorXd jac = Eigen::VectorXd::Zero(a.size());
    jac(0) = -beta.sum();
    jac.tail(a.size() - 1) = subFeatures_X.transpose() * beta;
    return jac;
//
//        for (optimize::Index i = 0; i < x.size() - 1; ++i) {
//            if (i == 0) {
//                g(i) = 4*b*(x(i)*x(i)*x(i) - x(i)*x(i+1)) + 2*x(0) - 2;
//            }
//            if (i > 0 && i < x.size() - 1) {
//                g(i) = 4*b*(x(i)*x(i)*x(i) - x(i)*x(i+1)) + 2*b*(x(i) - x(i-1)*x(i-1)) + 2*x(i) - 2;
//            }
//            if (i+1 == x.size() - 1) {
//                g(i+1) = 2*b*(x(i+1) - x(i)*x(i));
//            }
//        }
//        return g;
}