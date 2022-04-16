/*
 * interval_comparator.h
 * This file is part of bpm21
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 03/01/21.
//

#ifndef CUCCIOLO_INTERVAL_COMPARATOR_H
#define CUCCIOLO_INTERVAL_COMPARATOR_H

#include <utility>

/**
 * This struct orders the query intervals from the smaller to the larger, and orders the intervals with the same length
 * as usual
 * @tparam T
 * @tparam PrevNext
 */
template<typename T, typename PrevNext> struct interval_comparator {
    PrevNext indexer;
    bool operator()(const std::pair<T,T>& lhs, const std::pair<T,T>& rhs) const {
        auto leftDistance = indexer.distance(lhs.second, lhs.first);
        auto rightDistance = indexer.distance(rhs.second, rhs.first);
        return (leftDistance < rightDistance) ||
               ((leftDistance == rightDistance) && ((lhs.first < rhs.first) || ((lhs.first == rhs.first) && (lhs.second < rhs.second))));
    }
};


template<typename T, typename PrevNext> struct interval_comparator_inverse {
    PrevNext indexer;
    bool operator()(
            const std::pair<T, T>& lhs, const std::pair<T,T>& rhs) const {
        auto leftDistance = indexer.distance(lhs.second, lhs.first);
        auto rightDistance = indexer.distance(rhs.second, rhs.first);
        return (leftDistance > rightDistance) ||
               ((leftDistance == rightDistance) && ((lhs.first > rhs.first) || ((lhs.first == rhs.first) && (lhs.second > rhs.second))));
    }
};

#endif //CUCCIOLO_INTERVAL_COMPARATOR_H
