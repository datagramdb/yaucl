/*
 * insert_interval.h
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

#ifndef CUCCIOLO_INSERT_INTERVAL_H
#define CUCCIOLO_INSERT_INTERVAL_H

#include <yaucl/structures/query_interval_set/structures/forInsertion.h>
#include <cassert>
#include <vector>
#include <queue>

template <typename T, typename PrevNext>
std::vector<std::pair<T,T>> insert_interval(const PrevNext& indexer, struct node_recur<T>& element, T left, T right) {
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

        if (it->children.empty()) {// If there is no other interval, use the thrichotomy
            //std::cout << stackElement << ": thrichotomy" << std::endl;
            T nextToLeft = indexer.getPrev(currentLeft);
            if ((it->min < nextToLeft))
                it->children.emplace_back(it->min, nextToLeft);
            // This is the actual interval to which I am interested on!
            it->children.emplace_back(currentLeft, currentRight);
            result.emplace_back(currentLeft, currentRight);
            size_t offset = it->children.size()-1;
            ///std::cout << " ~~ for current (1): " << it->children[offset] << std::endl;
            T nextToRight = indexer.getNext(currentRight);
            if (it->max > nextToRight)
                it->children.emplace_back(nextToRight, it->max);
            // Returning the interesting interval
            it->children[offset].isPointed++;
        } else {
            // Scanning the sorted intervals (assert)
            size_t N = it->children.size();
            node_recur<T>* list_ptr = (N == 0 ? nullptr : it->children.data());

            //bool isCurrentUpdated = true;

            for (size_t i = 0; i<N; i++) {
                //if (isCurrentUpdated) {
                //std::cout << stackElement << ": scanningChildren @" << i << " up-to <" << N << std::endl;
                //isCurrentUpdated = false;
                //}
                if ((list_ptr[i].min == currentLeft) && (list_ptr[i].max == currentRight)) {
                    // If the current interval precisely describes the current interval, then return it!
                    list_ptr[i].isPointed++;
                    ///std::cout << " ~~ for current (2): " << list_ptr[i] << std::endl;
                    result.emplace_back(currentLeft, currentRight);
                } else if (list_ptr[i].max < currentLeft) {
                    // If the current interval does not contain the present interval, then skip it!
                    continue;
                } else if (list_ptr[i].min <= currentLeft) {
                    // If the current interval has a left intersection, then

                    if (list_ptr[i].max > currentRight) {   // 1) If the interval is completely contained, then ok
                        q.emplace(currentLeft, currentRight, &list_ptr[i]);
                    } else {
                        // 2) Otherwise, if there is a partial overlap, perform the current overlap, and then continue next!
                        q.emplace(currentLeft, list_ptr[i].max, &list_ptr[i]);
                        currentLeft = indexer.getNext(list_ptr[i].max);
                        if (currentLeft > currentRight)
                            std::swap(currentRight, currentLeft);
                        //isCurrentUpdated = true;
                    }
                } else {
                    break;//assert(false && "ERROR: unexpected case!");
                }
            }
        }

        q.pop();
    }


    return result;
}


#endif //CUCCIOLO_INSERT_INTERVAL_H
