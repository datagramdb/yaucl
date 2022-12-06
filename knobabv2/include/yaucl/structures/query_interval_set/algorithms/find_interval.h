/*
 * find_interval.h
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

#ifndef CUCCIOLO_FIND_INTERVALS_H
#define CUCCIOLO_FIND_INTERVALS_H

#include <yaucl/structures/query_interval_set/algorithms/insert_interval.h>
#include <yaucl/hashing/pair_hash.h>
#include <unordered_set>

template <typename T, typename PrevNext>
std::vector<std::pair<T,T>> find_interval(const PrevNext& indexer, struct node_recur<T>& element, T left, T right) {
    std::vector<std::pair<T,T>> result;
    if (right < left) return result;
    std::queue<forInsertion<T>> q;
    q.emplace(left, right, &element);

    while (!q.empty()) {
        forInsertion<T> stackElement = q.front();
        node_recur<T>* it = stackElement.nodeStack;
        T& currentLeft = stackElement.left;
        T& currentRight = stackElement.right;

        if ((currentLeft < it->min)|| (it->max < currentRight)) return result;

        // Scanning the sorted intervals (assert)
        size_t N = it->children.size();
        size_t M = it->onlyForPointed.size();

        if (M != 0) {
            for (size_t i = 0; i<M; i++) {
                auto ref = it->onlyForPointed.at(i);
                if ((ref.min >= currentLeft) && (ref.min <= currentRight) &&
                    (ref.max <= currentRight) && (ref.max >= currentLeft)) {
                    result.emplace_back(ref.min, ref.max);
                }
            }
        }

        node_recur<T>* list_ptr = (N == 0 ? nullptr : it->children.data());

        if (it->children.empty()) {
            if ((it->min == currentLeft) && (it->max == currentRight)) {
                result.emplace_back(currentLeft, currentRight);
            }
        } else {
            for (size_t i = 0; i<N; i++) {
                if ((list_ptr[i].min == currentLeft) && (list_ptr[i].max == currentRight)) {
                    std::unordered_set<std::pair<T,T>> S;
                    list_ptr[i].collect_intervals2(S, indexer); // TODO: collect_intervals
                    result.insert(result.end(), S.begin(), S.end());
                } else if (list_ptr[i].max < currentLeft) {
                    continue;
                } else if (list_ptr[i].min <= currentLeft) {
                    if (list_ptr[i].max > currentRight) {
                        q.emplace(currentLeft, currentRight, &list_ptr[i]);
                    } else {
                        q.emplace(currentLeft, list_ptr[i].max, &list_ptr[i]);
                        currentLeft = indexer.getNext(list_ptr[i].max);
                        if (currentLeft > currentRight)
                            std::swap(currentRight, currentLeft);
                    }
                } else if ((list_ptr[i].min >= currentLeft) && (list_ptr[i].min <= currentRight) &&
                           (list_ptr[i].max <= currentRight) && (list_ptr[i].max >= currentLeft)) {
                    q.emplace(list_ptr[i].min, list_ptr[i].max, &list_ptr[i]);
                } else
                    break;
            }

        }


        q.pop();
    }


    return result;
}


#endif //CUCCIOLO_FIND_INTERVALS_H
