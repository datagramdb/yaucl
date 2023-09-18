/*
 * ForTheWin.h
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

#ifndef DISTANCE_FORTHEWIN_H
#define DISTANCE_FORTHEWIN_H

#include <utility>
#include <vector>



struct ForTheWin {
    enum gain_measures {
        Entropy,
        Gini,
        CART
    };
    std::vector<std::pair<double,double>> v;
    std::vector<double> n;
    double total, nPos, nNeg;

    ForTheWin(int max_classes) : v(max_classes+1, std::pair<double,double>(0.0,0.0)), n(max_classes+1, 0.0), total{0.0} {}
    ForTheWin(const ForTheWin&) = default;
    ForTheWin(ForTheWin&& ) = default;
    ForTheWin& operator=(const ForTheWin&) = default;
    ForTheWin& operator=(ForTheWin&& ) = default;

    void setP(int clazz, double pos, double neg);
    double countClass(int clazz);

    size_t nClasses() const;
    double getClassPrecision(int clazz) const;
    double getP(int clazz, bool posNegOtherwise) const;
    double getGain(gain_measures type) const;
    void goodBad(double pos, double neg);

    void normalizeCountClass();
};


#endif //DISTANCE_FORTHEWIN_H
