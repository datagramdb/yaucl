/*
 * ForTheWin.cpp
 * This file is part of yaucl-learning
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl-learning is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl-learning is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl-learning. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 03/11/22.
//

#include <cmath>
#include <yaucl/learning/dt/ForTheWin.h>
#include <algorithm>

//void ForTheWin::setP(int clazz, double pos, double neg) {
//    v[clazz].first = pos/nPos;
//    v[clazz].second = neg/nNeg;
//}

//double ForTheWin::getP(int clazz, bool posNegOtherwise) const {
//    return posNegOtherwise ? v.at(clazz).first : v.at(clazz).second;
//}


static inline double entropy(const std::vector<double>& Dp) {
    double sum = 0.0;
    for (const double x : Dp) sum += (x * std::log2(x+1));
    return -sum;
}

static inline double gini(const std::vector<double>& Dp) {
    double sum = 0.0;
    for (const double x : Dp) sum += (x * x);
    return 1.0-sum;
}

static inline double entropy(const std::vector<std::pair<double,double>>& Dp, bool isfirst) {
    double sum = 0.0;
    for (const auto x : Dp) {
        double item = isfirst ? x.first : x.second;
        sum += (item * std::log2(item+1));
    }
    return -sum;
}

static inline double gini(const std::vector<std::pair<double,double>>& Dp, bool isfirst) {
    double sum = 0.0;
    for (const auto x : Dp) {
        double item = isfirst ? x.first : x.second;
        sum += (item * item);
    }
    return 1.0-sum;
}

static inline double cartSum(const std::vector<std::pair<double,double>>& Dp) {
    double sum = 0.0;
    for (const auto x : Dp) {
        sum += std::abs(x.first-x.second);
    }
    return sum;
}

static inline double entropy(double nPos,
                             double nNeg,
                             const std::vector<std::pair<double,double>>& Dp,
                             const std::vector<double>& D) {
    double nTot = nPos+nNeg;
    // H(Dx,Dy) = nY/n H(Dy)+ nX/n H(Dx)
    double hDxDy = (nPos/nTot) * entropy(Dp, true) + (nNeg/nTot)* entropy(Dp, false);
    // H(D) - H(Dx,Dy)
    return entropy(D) - hDxDy;
}

static inline double gini(double nPos,
                          double nNeg,
                          const std::vector<std::pair<double,double>>& Dp,
                          const std::vector<double>& D) {
    double nTot = nPos+nNeg;
    // H(Dx,Dy) = nY/n H(Dy)+ nX/n H(Dx)
    double hDxDy = (nPos/nTot) * gini(Dp, true) + (nNeg/nTot)* gini(Dp, false);
    // H(D) - H(Dx,Dy)
    return gini(D) - hDxDy;
}

static inline double cart(double nPos,
                          double nNeg,
                          const std::vector<std::pair<double,double>>& Dp) {
    double nTot = nPos+nNeg;
    // H(Dx,Dy) = nY/n H(Dy)+ nX/n H(Dx)
    return 2.0*(nPos/nTot)*(nNeg/nTot)*cartSum(Dp);
}

double ForTheWin::getGain(ForTheWin::gain_measures type) const {
    switch (type) {
        case Entropy:
            return entropy(nPos, nNeg, this->v, this->n);

        case Gini:
            return gini(nPos, nNeg, this->v, this->n);

        case CART:
            return cart(nPos, nNeg, this->v);
    }
}

//double ForTheWin::countClass(int clazz) {
//    total+=1.0;
//    return (n[clazz]+=1.0);
//}

//double ForTheWin::getClassPrecision(int clazz) const { return n.at(clazz); }

//size_t ForTheWin::nClasses() const { return v.size(); }

//void ForTheWin::goodBad(double d, double d1) {
//    nPos = d; nNeg = d1;
//}

//void ForTheWin::normalizeCountClass() {
//    std::transform(n.begin(), n.end(), n.begin(), [this](double x ){
//        return x/total;
//    });
//}
