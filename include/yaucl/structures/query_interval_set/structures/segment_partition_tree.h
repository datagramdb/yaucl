/*
 * segment_partition_tree.h
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

#ifndef CUCCIOLO_SEGMENT_PARTITION_TREE_H
#define CUCCIOLO_SEGMENT_PARTITION_TREE_H

#include <vector>
#include <yaucl/structures/query_interval_set/structures/node_recur.h>
#include <yaucl/structures/query_interval_set/algorithms/minimize_tree.h>
#include <yaucl/structures/query_interval_set/operators/interval_comparator.h>

template <typename T, typename PrevNext> struct segment_partition_tree {
    PrevNext     indexer;
    struct node_recur<T> element;
    T min;
    T max;

    segment_partition_tree(T min, T max) : element(min, max), min{min}, max{max} {}
    segment_partition_tree() : element{} {}
    DEFAULT_COPY_ASSGN(segment_partition_tree)

    /*std::unordered_set<std::pair<T,T>> collect_intervals()  {
        std::unordered_set<std::pair<T,T>> S;
        for (size_t i = 0, N = element.children.size(); i<N; i++)
            element.children.at(i).collect_intervals(S);
        return S;
    }*/

    std::unordered_set<std::pair<T,T>> collect_intervals2()  {
        std::unordered_set<std::pair<T,T>> S;
        element.collect_intervals2(S, indexer);
        return S;
    }

    std::vector<std::pair<T, T>> findInterval(const T &left, const T &right) {
        return find_interval(indexer, element, left, right);
    }

#if 0
    std::vector<std::pair<T,T>> bulk_load;

    void load_phase_insert_data(const T& lowerBound, const T& upperBound) {
        bulk_load.emplace_back(lowerBound, upperBound);
    }

    void index_phase() {
        bulk_load.emplace_back(min, max);
        std::sort(bulk_load.begin(), bulk_load.end(), interval_comparator_inverse<std::string, StringPrevNext>());
        // Inserting one interval at a time
        bulk_load.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
        for (const auto& i : bulk_load) {
            /*std::cout << i << " = " <<*/ insert_interval(this->indexer, this->element, i.first, i.second) /*<< std::endl*/;
            ///std::cout << tree << std::endl;
            ///std::cout << std::endl;
        }
        //std::cout << *this << std::endl;
        minimize_tree<T, PrevNext>(indexer, element);
        /*for (const auto& i : bulk_insertion) {
            std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
        }*/
        //std::cout << *this << std::endl;
    }
#endif

    friend std::ostream &operator<<(std::ostream &os, const segment_partition_tree &tree) {
        os << " min: " << tree.element.min << " max: " << tree.element.max << std::endl;
        for (size_t i = 0, N = tree.element.children.size(); i<N; i++)
            tree.element.children.at(i).print(os, 2);
        return os;
    }
};

template <typename T>
struct spt_bulk_insertion {
    std::vector<std::pair<T, T>> bulk_insertion;

    template <typename Iterator> void insert(Iterator begin, Iterator end ) {
        bulk_insertion.insert(bulk_insertion.end(), begin, end);
    }

    template <typename NextType>
    segment_partition_tree<T, NextType> finalize(T min, T max) {
        segment_partition_tree<T, NextType> result{min, max};
        bulk_insertion.emplace_back(min, max);
        std::sort(bulk_insertion.begin(), bulk_insertion.end(), interval_comparator_inverse<T, NextType>());
        // Inserting one interval at a time

        bulk_insertion.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
        for (const auto& i : bulk_insertion) {
            /*std::cout << i << " = " <<*/ insert_interval(result.indexer, result.element, i.first, i.second) /*<< std::endl*/;
            ///std::cout << tree << std::endl;
            ///std::cout << std::endl;
        }
        //std::cout << *this << std::endl;
        minimize_tree<T, NextType>(result.indexer, result.element);
        /*for (const auto& i : bulk_insertion) {
            std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
        }*/
        //std::cout << *this << std::endl;
        return result;
    }
};

#endif //CUCCIOLO_SEGMENT_PARTITION_TREE_H
