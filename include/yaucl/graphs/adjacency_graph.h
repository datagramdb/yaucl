/*
 * adjacency_graph.h
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
// Created by giacomo on 28/02/21.
//

#ifndef CLASSIFIERS_ADJACENCY_GRAPH_H
#define CLASSIFIERS_ADJACENCY_GRAPH_H

#include <functional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <roaring64map.hh>
#include <yaucl/numeric/ssize_t.h>
#include <stack>
#include "yaucl/structures/stack_container.h"


struct adjacency_graph {
    size_t V_size, E_size;
    std::vector<std::vector<size_t>> nodes;
    std::vector<std::pair<size_t, size_t>> edge_ids;
    std::unordered_map<size_t, std::vector<size_t>> ingoing_edges;

    adjacency_graph();
    virtual  ~adjacency_graph() {
        nodes.clear();
        edge_ids.clear();
        ingoing_edges.clear();
    }
    adjacency_graph(const adjacency_graph&) = default;
    adjacency_graph(adjacency_graph&&) = default;
    adjacency_graph& operator=(const adjacency_graph&) = default;
    adjacency_graph& operator=(adjacency_graph&&) = default;

    /**
     * Adds a node to the ids
     * @return  The id of the new node
     */
    size_t add_node();
    size_t add_edge(size_t src, size_t dst);
    std::pair<size_t, size_t> add_undirected_edge(size_t src, size_t dst);
    const std::pair<size_t, size_t>& edge_from_id(size_t edge_id) const;
    const std::vector<size_t>& getOutgoingEdgesId(size_t node_id) const;
    const std::vector<size_t>& getIngoingEdgesId(size_t node_id) const;
    void DFSUtil(size_t src, std::unordered_set<size_t>& visited);
    void clear() {
        nodes.clear();
        edge_ids.clear();
        ingoing_edges.clear();
        V_size = E_size = 0;
    }

    std::unordered_map<size_t, std::vector<size_t>> topological_sort(ssize_t start_from) {
        roaring::Roaring64Map visited;
        std::vector<size_t> Stack;
        if (start_from > 0) {
            topologicalSortUtil(start_from, visited, Stack);
        }
        for (size_t i = 0; i < V_size; i++)
            if (!visited.contains(i))
                topologicalSortUtil(i, visited, Stack);
        std::unordered_map<size_t, size_t> time;
        std::unordered_map<size_t, std::vector<size_t>> time_to_node;
        bool firstVisit = true;
        std::reverse(Stack.begin(), Stack.end());
        for (size_t v : Stack) {
            if (firstVisit) {
                time[v] = 0;
                firstVisit = false;
            } else {
                time[v] = 0;
                auto it = ingoing_edges.find(v);
                if (it != ingoing_edges.end()) {
                    for (size_t edgeId : it->second)
                        time[v] = std::max(time[edge_ids.at(edgeId).first], time[v]);
                    time[v]++;
                }
            }
        }
        for (const auto& ref : time)
            time_to_node[ref.second].emplace_back(ref.first);
        return time_to_node;
    }
    void dot(std::ostream& os);

    bool operator==(const adjacency_graph &rhs) const;
    bool operator!=(const adjacency_graph &rhs) const;

private:
    void topologicalSortUtil(size_t v, roaring::Roaring64Map& visited,
                        std::vector<size_t>& Stack)
    {
        // Mark the current node as visited.
        visited.add(v);

        // Recur for all the vertices adjacent to this vertex
        for (size_t edge_id: nodes.at(v)) {
            size_t i = edge_ids.at(edge_id).second;
            if (!visited.contains(i))
                topologicalSortUtil(i, visited, Stack);
        }

        // Push current vertex to stack which stores result
        Stack.emplace_back(v);
    }
    void printAllPathsUtil(size_t u, size_t d, std::unordered_set<size_t>& visited, std::vector<ssize_t>& path, size_t path_index, std::unordered_set<size_t>& visited_src_dst, std::unordered_set<size_t>& global);
    const static std::vector<size_t> emptyVector;
};


void adjacency_graph_inv_DFSUtil(size_t src, const adjacency_graph& ag, roaring::Roaring64Map &visited) ;

void adjacency_graph_DFSUtil(size_t src, const adjacency_graph& ag, roaring::Roaring64Map &visited) ;

void adjacency_graph_DFSUtil_with_edge_prop(size_t src,
                                            const adjacency_graph& ag,
                                            roaring::Roaring64Map &visited,
                                            const std::function<bool(size_t)>& edgeProp = [](size_t){return true;});
#endif //CLASSIFIERS_ADJACENCY_GRAPH_H
