/*
 * DecisionTree.cpp
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

#include <yaucl/learning/DecisionTree.h>

std::vector<int> getOnLocations(int a) {
    std::vector<int> result;
    int place = 0;
    while (a != 0) {
        if (a & 1) {
            result.push_back(place);
        }
        ++place;
        a >>= 1;
    }
    return result;
}
