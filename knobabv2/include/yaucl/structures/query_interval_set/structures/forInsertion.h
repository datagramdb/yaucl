/*
 * forInsertion.h
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

#ifndef CUCCIOLO_FORINSERTION_H
#define CUCCIOLO_FORINSERTION_H

#include <yaucl/structures/query_interval_set/structures/segment_partition_tree.h>

template<typename T> struct forInsertion {
    T left, right;
    node_recur<T>* nodeStack;

    forInsertion() : nodeStack(nullptr) {}
    forInsertion(T left, T right, node_recur<T> *nodeStack) : left(left), right(right), nodeStack(nodeStack) {}
    forInsertion(const forInsertion& ) = default;
    forInsertion& operator=(const forInsertion& ) = default;

    friend std::ostream &operator<<(std::ostream &os, const forInsertion &insertion) {
        os << "->[" << insertion.left << "," << insertion.right << "] @? " << *insertion.nodeStack;
        return os;
    }
};

#endif //CUCCIOLO_FORINSERTION_H
