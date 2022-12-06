/*
 * minimize_tree.h
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

#ifndef CUCCIOLO_MINIMIZE_TREE_H
#define CUCCIOLO_MINIMIZE_TREE_H

#include <yaucl/structures/query_interval_set/algorithms/find_interval.h>
#include <yaucl/functional/removeIndicesFromVector.h>
#include <yaucl/functional/assert.h>
#include <stack>

/**
 * This function minimizes the interval tree by finding the elements that are not pointed by any query interval and
 * by pushing the leaves towards the parent's parent if the parent is not pointed by any other query interval.
 *
 * After the minimization, all the nodes are pointed, and the non-pointed nodes are pushed towards a specific vector,
 * in which we will have all the elements that are not directly required by a query
 *
 * @tparam T
 * @param node
 */
template<typename T, typename PrevNext> void minimize_tree(const PrevNext& indexer, struct node_recur<T>& node) {
    std::stack<std::pair<node_recur<T>*, size_t>> dfs_tree_visit;
    dfs_tree_visit.emplace(&node, 0);

    size_t references = 0; // Counting the frequency of accessing the interval from a query interval
    while (!dfs_tree_visit.empty()) {
        std::pair<node_recur<T>*, size_t> ptr = dfs_tree_visit.top();
        ///std::cout << " * " << *ptr.first << "@child = " << ptr.second << std::endl;
        if (ptr.first->children.empty())
            DEBUG_ASSERT(false); // this should never happen, as the algorithm directly visit the leaves from the parent (*)
        else {
            size_t currentChild = ptr.second;
            size_t N = ptr.first->children.size();

            // Incrementing the occurrence only when I start the iteration, that is when the number of children is zero
            if (!ptr.second) references += ptr.first->isPointed;

            // Scanning the children that are leaves (= empty children) (*)
            while ((currentChild < N) && (ptr.first->children.at(currentChild).children.empty())) {

                if (!ptr.first->children.at(currentChild).isPointed) {
                    // merging at the node level all the elements not pointed by another vector
                    if (ptr.first->children.at(currentChild).onlyForPointed.empty()) {
                        ptr.first->onlyForPointed.emplace_back(ptr.first->children.at(currentChild));
                    } else {
                        for (const auto& ref : ptr.first->children.at(currentChild).onlyForPointed) {
                            ptr.first->onlyForPointed.emplace_back(ref);
                        }
                    }
                    ptr.first->toErase.emplace_back(currentChild);
                } else {
                    // If this node is pointed by another query, increment the references with the one from the parent
                    ptr.first->children.at(currentChild).isPointed += references;
                }

                currentChild++; // Moving towards the next child
            }

            // At this point, either I reach a child that is not a leaf, or I finished to scan the children
            if (currentChild < ptr.first->children.size()) {
                // If the current element is a non-leaf node, perform the DFS visit of the tree
                // Recursively scanning the non-leaf child (2), preserving the current iterator step for the next iteration (1) and (3)
                dfs_tree_visit.pop();                                                //1) pop the current iteration step
                dfs_tree_visit.emplace(ptr.first, currentChild + 1);              //2) pushing the next iteration step
                dfs_tree_visit.emplace(&ptr.first->children.at(currentChild), 0); //3) pushing the next child step
            } else {
                // Otherwise, if I finished to iterate over the children,
                //  a) remove the elements that were transfered to onlyForPointed
                if (!ptr.first->toErase.empty()) {
                    // Removing all the indices that we need to remove
                    removeIndicesFromVector(ptr.first->children, ptr.first->toErase.begin(), ptr.first->toErase.end());
                    // Remove the indices to remove
                    ptr.first->toErase.clear();
                }

                // b) If the current element is not pointed, then I can potentially transfer all the contained unpointed nodes up
                size_t NL = ptr.first->children.size();
                bool allPointed = false;
                if ((dfs_tree_visit.size() > 1) && (!ptr.first->isPointed) && (NL>0)) {
                    allPointed = true;
                    for (size_t i = 0; i<NL; i++) {
                        if (!ptr.first->children.at(i).isPointed) {
                            allPointed = false;
                            break;
                        }
                    }
                }

                // If the current element has elements that are not pointed or if I am the last element,
                // I can merge together the intervals that are not disjoint. This will ease the search/scan
                // by reducing the data available
                if (!allPointed) {
                    auto& result = ptr.first->children;
                    std::vector<std::vector<std::pair<T, T>>> mergeSet;
                    mergeSet.emplace_back();
                    mergeSet[0].emplace_back(result.at(0).min, result.at(0).max);

                    // Finding the number of disjoint macro-intervals
                    size_t mergeSetIndex = 0;
                    // Counting how many of such intervals are singletons
                    size_t countSingletonsPerMergeSets = 0;
                    for (size_t i = 1, N = result.size(); i<N; i++) {
                        T maxPrev = result.at(i-1).max;
                        T nextOf = indexer.getNext(maxPrev);
                        T minOfNext = result.at(i).min;
                        if (nextOf == minOfNext) {
                            mergeSet[mergeSetIndex].emplace_back(minOfNext, result.at(i).max);
                        } else {
                            countSingletonsPerMergeSets += (mergeSet.at(mergeSetIndex).size() == 1 ? 1 : 0);
                            mergeSet.emplace_back();
                            mergeSetIndex++;
                            mergeSet[mergeSetIndex].emplace_back(minOfNext, result.at(i).max);
                        }
                    }
                    countSingletonsPerMergeSets += (mergeSet.at(mergeSetIndex).size() == 1 ? 1 : 0);

                    // If there is more than just one disjoint interval, then it means that I can merge more disjoint elements
                    // Also, skip the processing step if all the intervals are disjoint.
                    if ((mergeSetIndex > 0) && (countSingletonsPerMergeSets != (mergeSetIndex+1))) {
                        std::vector<node_recur<T>> neueResult;
                        size_t prev = 0;
                        for (const std::vector<std::pair<T, T>>& x : mergeSet) {
                            size_t N = x.size();
                            if (N > 1) {
                                // If the current disjoint element contains more than one element, then merge all these
                                // intervals into one
                                node_recur<T>& ref = neueResult.emplace_back(x.begin()->first, x.rbegin()->second);
                                for (size_t i = 0; i<N; i++) {
                                    ref.children.emplace_back(result.at(prev+i));
                                }
                            } else {
                                // Otherwise, copy the singleton back.
                                neueResult.emplace_back(result.at(prev));
                            }
                            prev += N;
                        }
                        result = neueResult;
                    }
                }

                dfs_tree_visit.pop();
                references -= ptr.first->isPointed;
                if (allPointed) {
                    // If I can transfer some nodes, do it!
                    std::pair<node_recur<T>*, size_t> top = dfs_tree_visit.top();
                    dfs_tree_visit.pop();
                    DEBUG_ASSERT(&top.first->children.at(top.second-1) == ptr.first);
                    size_t copySize = ptr.first->children.size();

                    if ((top.first != &node) || (top.first->isPointed)) {
                        for (const auto& x : ptr.first->onlyForPointed)
                            top.first->onlyForPointed.emplace_back(x);
                    }
                    if (copySize > 0) {
                        std::vector<node_recur<T>> tmp;
                        for (const auto& x : ptr.first->children)
                            tmp.emplace_back(x);
                        top.first->children.insert(top.first->children.begin()+top.second, tmp.begin(), tmp.end());
                    }
                    top.first->children.erase(top.first->children.begin() + (top.second-1));
                    dfs_tree_visit.emplace(top.first, top.second-1+copySize);
                }

            }
        }
    }

    // After refactoring the tree, picking up the best minimum and maximum value.
    // This provides a further minimization, so that the intervals that are not required
    // are completely removed.
    if (!node.children.empty()) {
        if (node.children.begin()->min > node.min) {
            node.min = node.children.begin()->min;
        }
        if (node.children.rbegin()->max < node.max) {
            node.max = node.children.rbegin()->max;
        }
    }

}

#endif //CUCCIOLO_MINIMIZE_TREE_H
