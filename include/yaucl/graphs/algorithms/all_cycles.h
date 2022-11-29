//
// Created by giacomo on 29/11/22.
//

#ifndef YAUCL_GRAPH_ALL_CYCLES_H
#define YAUCL_GRAPH_ALL_CYCLES_H

#include <string>
#include "yaucl/hashing/vector_hash.h"
#include <vector>
#include <unordered_set>
#include "roaring64map.hh"

template <typename T, typename Iterator>
std::vector<std::vector<T>> remove_subpaths(Iterator begin, Iterator end) {
    std::vector<std::vector<T>> toret;
    std::unordered_set<std::vector<T>> intermediate;
    for (auto it = begin; it != end; it++) {
        intermediate.insert(*it);
    }
    std::sort(toret.begin(), toret.end(), [](auto x, auto y) {
        return x.size() < y.size();
    });
    roaring::Roaring64Map posToRemove;
    for (size_t i = 0, n = toret.size(); i<n; i++) {
        auto& curI = toret[i];
        std::sort(curI.begin(), curI.end());
        for (size_t j = 0; j<i; j++) {
            auto& curJ = toret[j];
            if (posToRemove.contains(j)) continue;
            // Removing all the cycles that are contained in bigger cycles: we reduce the number of the loops to visit
            if (std::includes(curI.begin(), curI.end(), curJ.begin(), curJ.end())) {
                posToRemove.add(j);
            }
        }
    }

    remove_index(toret, posToRemove);
    return toret;
}


template <typename T, typename Iterator>
std::vector<std::vector<T>> remove_suppaths(Iterator begin, Iterator end) {
    std::vector<std::vector<T>> toret;
    std::unordered_set<std::vector<T>> intermediate;
    for (auto it = begin; it != end; it++) {
        intermediate.insert(*it);
    }
    std::sort(toret.begin(), toret.end(), [](auto x, auto y) {
        return x.size() < y.size();
    });
    roaring::Roaring64Map posToRemove;
    for (size_t i = 0, n = toret.size(); i<n; i++) {
        auto& curI = toret[i];
        std::sort(curI.begin(), curI.end());
        for (size_t j = 0; j<i; j++) {
            auto& curJ = toret[j];
            // Removing all the cycles that are contained in bigger cycles: we reduce the number of the loops to visit
            if (std::includes(curI.begin(), curI.end(), curJ.begin(), curJ.end())) {
                posToRemove.add(i);
            }
        }
    }

    remove_index(toret, posToRemove);
    return toret;
}

#include <yaucl/graphs/adjacency_graph.h>
#include <algorithm>

/**
 * This class allows the computation of all the loops
 */
struct AllCycles {
    roaring::Roaring64Map visited;
    const struct adjacency_graph& graph;
    std::unordered_set<std::vector<size_t>> cycles;

    AllCycles(const adjacency_graph &graph);
    AllCycles(const AllCycles&) = default;
    AllCycles& operator=(const AllCycles&) = default;

    std::vector<std::vector<size_t>> operator()();

private:

    void dfsLoopVisit(size_t x,
                      size_t i,
                      std::vector<size_t>& stack_array);

};


#endif //YAUCL_GRAPH_ALL_CYCLES_H
