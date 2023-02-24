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

#include <yaucl/graphs/FlexibleFA.h>
#include <algorithm>

/**
 * This class allows the computation of all the loops
 */
template <typename NodeLabel, typename EdgeLabel>
struct AllCycles {
    roaring::Roaring64Map visited;
    const struct FlexibleFA<NodeLabel, EdgeLabel>& graph;
    std::unordered_set<std::vector<size_t>> cycles;
    std::vector<std::vector<size_t>> cycles_of_vertices;
    std::vector<std::vector<size_t>> cycles_of_edges;

    AllCycles(struct FlexibleFA<NodeLabel, EdgeLabel> &graph) : graph(graph) {}
    AllCycles(const AllCycles&) = default;
    AllCycles& operator=(const AllCycles&) = default;

    /**
     * Similar to operator(), but, instead of returning the loops as node ids,
     * this returns edge ids defining the loop
     *
     * @return
     */
    void edge_loops() {
        visited.clear();
        cycles.clear();
        cycles_of_vertices.clear();
        cycles_of_edges.clear();
        std::vector<std::vector<size_t>> edges_toret;
        std::vector<size_t> arrayV, arrayE;
        arrayV.reserve(graph.vSize()+1);
        arrayE.reserve(graph.vSize());
        for (size_t i : graph.getNodeIds()) {
            if (!visited.contains(i)) {
                dfsLoopVisitWithEdges(i, -1, arrayV, arrayE);
            }
        }

        // Moving that into a list, so we can iterate over those and access by id
//        edges_toret = {cycles.begin(), cycles.end()};
//        std::sort(edges_toret.begin(), edges_toret.end(), [](auto l, auto r) {
//            return l.size() < r.size();
//        });
//        return std::pair<const std::vector<std::vector<size_t>>*,
//                const std::vector<std::vector<size_t>>*>{(const std::vector<std::vector<size_t>>*)&cycles_of_vertices, (const std::vector<std::vector<size_t>>*)&cycles_of_edges};
    }

    std::vector<std::vector<size_t>> operator()(bool doPrune=true) { // run
        visited.clear();
        cycles.clear();

        std::vector<std::vector<size_t>> toret;
        std::vector<size_t> array;

        // Getting all the distinct cycles
        array.reserve(graph.vSize()+1);
        for (size_t i : graph.getNodeIds()) {
            if (!visited.contains(i)) {
                dfsLoopVisit(i, array);
            }
        }

        // Moving that into a list, so we can iterate over those and access by id
        toret = {cycles.begin(), cycles.end()};
        std::sort(toret.begin(), toret.end(), [](auto l, auto r) {
            return l.size() < r.size();
        });
        if (!doPrune) return toret;


        // Removing all the redundant cycles
        for (size_t i = 0, n = cycles.size(); i<n; i++) {
            auto& curI = toret[i];
            std::sort(curI.begin(), curI.end());
            for (int j = 0; j<i; j++) {
                // already sorted per j<i!
                auto& curJ = toret.at(j);
                if (!cycles.contains(curJ)) continue;

                // Removing all the cycles that are contained in bigger cycles: we reduce the number of the loops to visit
                if (std::includes(curI.begin(), curI.end(), curJ.begin(), curJ.end())) {
                    cycles.erase(curJ);
                }
            }
        }

        return {cycles.begin(), cycles.end()};
    }

private:

    void dfsLoopVisit(size_t x, std::vector<size_t> &stack_array) {
        visited.add(x);
        stack_array.emplace_back(x);
        for (const std::pair<EdgeLabel, size_t>& edgeId : graph.outgoingEdges(x)) {
            if (visited.contains(edgeId.second)) {
                // Checking whether the current element has been visited in the current path
                auto it = std::find(stack_array.begin(), stack_array.end(),edgeId.second);
                if (it != stack_array.end()) {
                    // If it is present in the stack, then I found a loop
                    cycles.emplace(it, stack_array.end());
                }
                // Otherwise, block the recursive iteration
            } else {
                dfsLoopVisit(edgeId.second, stack_array);
            }
        }
        stack_array.pop_back();
    }

    void dfsLoopVisitWithEdges(size_t x,
                               ssize_t edge_id,
                               std::vector<size_t> &stack_vector_array,
                               std::vector<size_t> &stack_edge_array) {
        visited.add(x);
        stack_vector_array.emplace_back(x);
        if (edge_id != -1) stack_edge_array.emplace_back(edge_id);
        for (const size_t & out_edge_id : graph.g.getOutgoingEdgesId(x)) {
            auto& ref = graph.g.edge_from_id(out_edge_id).second;
            if ((!graph.removed_nodes.contains(ref)) && (!graph.removed_edges.contains(out_edge_id))) {
                if (visited.contains(ref)) {
                    // Checking whether the current element has been visited in the current path
                    auto beg = stack_vector_array.begin();
                    auto it = std::find(beg, stack_vector_array.end(), ref);
                    if (it != stack_vector_array.end()) {
                        // If it is present in the stack, then I found a loop
                        size_t dist = it - beg; // offset in the vertex set
                        DEBUG_ASSERT(dist>0);
                        // the one in the edges set is that, minus one
                        cycles_of_vertices.emplace_back(it, stack_vector_array.end());
                        auto& ite = cycles_of_edges.emplace_back(stack_edge_array.begin()+(dist), stack_edge_array.end());
                        ite.emplace_back(out_edge_id);
#ifdef DEBUG
                        std::vector<size_t> element;
                        for (size_t i = 0; i<ite.size(); i++) {
                            const auto& cp = graph.g.edge_ids.at(ite.at(i));
                            if (i == 0)
                                element.emplace_back(cp.first);
                            element.emplace_back(cp.second);
                        }
                        DEBUG_ASSERT(element.front() == element.back());
                        element.pop_back();
                        DEBUG_ASSERT(element == cycles_of_vertices.back());
#endif
                    }
                    // Otherwise, block the recursive iteration
                } else {
                    dfsLoopVisitWithEdges(ref, out_edge_id, stack_vector_array, stack_edge_array);
                }
            }
        }

        stack_vector_array.pop_back();
        stack_edge_array.pop_back();
    }

};






#endif //YAUCL_GRAPH_ALL_CYCLES_H
