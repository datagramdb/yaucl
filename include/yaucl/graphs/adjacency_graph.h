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

enum graph_cases {
    ADJACENCY_GRAPH_CASE,
    WEIGHTED_LABELLED_GRAPH_CASE
};

struct adjacency_graph {
    static std::vector<size_t> empty_vector;
    enum graph_cases casusu;
    size_t V_size, E_size;
    std::vector<std::vector<size_t>> nodes;
    std::vector<std::pair<size_t, size_t>> edge_ids;
    std::unordered_map<size_t, std::vector<size_t>> ingoing_edges;
    std::unordered_set<size_t> removed_nodes;

    adjacency_graph(graph_cases casusu = ADJACENCY_GRAPH_CASE) : casusu{casusu}, V_size(0), E_size(0) {};
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

    inline std::vector<size_t> topological_sort2(ssize_t start_from) {
        roaring::Roaring64Map visited;
        std::vector<size_t> Stack;
        if (start_from > 0) {
            topologicalSortUtil(start_from, visited, Stack);
        }
        for (size_t i = 0; i < V_size; i++)
            if (!visited.contains(i))
                topologicalSortUtil(i, visited, Stack);
        std::vector<size_t> time(V_size, -1);
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
        return time;
    }

    inline std::unordered_map<size_t, std::vector<size_t>> topological_sort(ssize_t start_from) {
        std::unordered_map<size_t, std::vector<size_t>> time_to_node;
        auto result = topological_sort2(start_from);
        for (size_t ref_first = 0, N = result.size(); ref_first<N; ref_first++) {
            time_to_node[result.at(ref_first)].emplace_back(ref_first);
        }
        return time_to_node;
    }

    void dot(std::ostream& os);

    bool operator==(const adjacency_graph &rhs) const;
    bool operator!=(const adjacency_graph &rhs) const;

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
private:
    void printAllPathsUtil(size_t u, size_t d, std::unordered_set<size_t>& visited, std::vector<ssize_t>& path, size_t path_index, std::unordered_set<size_t>& visited_src_dst, std::unordered_set<size_t>& global);
    const static std::vector<size_t> emptyVector;
};

void adjacency_graph_inv_DFSUtil(size_t src, const adjacency_graph& ag, roaring::Roaring64Map &visited) ;
void adjacency_graph_DFSUtil(size_t src, const adjacency_graph& ag, roaring::Roaring64Map &visited) ;
void adjacency_graph_DFSUtil_with_edge_prop(size_t src,
                                            const adjacency_graph& ag,
                                            roaring::Roaring64Map &visited,
                                            const std::function<bool(size_t)>& edgeProp = [](size_t){return true;});



size_t add_node(adjacency_graph* graph, const std::string& label = "");
size_t add_edge(adjacency_graph* graph, size_t src, size_t dst, double cost = 1.0);

/**
 *
 * @param graph
 * @param edge_id           Edge to be changed
 * @param new_dst           New destination for the edge
 * @param cost_to_update    New cost for the edge
 * @return
 */
bool update_edge_target(adjacency_graph *graph, size_t edge_id, size_t new_dst, double* cost_to_update);
bool update_edge_source(adjacency_graph *graph, size_t edge_id, size_t new_src, double* cost_to_update);
bool remove_edge(adjacency_graph *graph, size_t edge_id);
std::pair<size_t, size_t> add_undirected_edge(adjacency_graph* graph, size_t src, size_t dst, double cost = 1.0);
const std::pair<size_t, size_t>& edge_from_id(const adjacency_graph* graph, size_t edge_id);
const std::vector<size_t> &getOutgoingEdgesId(adjacency_graph* graph, size_t node_id);
std::unordered_map<size_t, std::vector<size_t>>::const_iterator hasIngoingEdges(const adjacency_graph* graph, size_t node_id);
const std::vector<size_t> &getIngoingEdgesId(const adjacency_graph* graph, size_t node_id);
void DFSUtil(const adjacency_graph* graph, size_t src, std::unordered_set<size_t> &visited);
void printAllPathsUtil(const adjacency_graph* graph, size_t u, size_t d, std::unordered_set<size_t> &visited, std::vector<ssize_t> &path,
                       size_t path_index, std::unordered_set<size_t> &visited_src_dst, std::unordered_set<size_t> &global);


struct weigthed_labelled_automata : public adjacency_graph {
    std::vector<double> edge_weight;
    std::vector<std::string> node_label;
    std::unordered_map<size_t, std::string> inv_label_conversion;          // Maps a node-id to an associated label
    std::unordered_map<std::string, std::vector<size_t>> label_conversion; // Maps a label to the nodes associated to it
    std::unordered_map<std::string, size_t> final_labels;                  // Maps a label to an unique-id associated to it
    size_t count_labels = 0;
    double minimum_edge_weight;

    weigthed_labelled_automata();
    weigthed_labelled_automata(const weigthed_labelled_automata&) = default;
    weigthed_labelled_automata(weigthed_labelled_automata&&) = default;
    weigthed_labelled_automata& operator=(const weigthed_labelled_automata&) = default;
    weigthed_labelled_automata& operator=(weigthed_labelled_automata&&) = default;
};

double edge_cost(const adjacency_graph* graph, size_t edge_id);
const std::string& node_label(const adjacency_graph* graph, size_t node_id);
void dot(adjacency_graph* graph, std::ostream &os);
void from_string(weigthed_labelled_automata& graph, const std::vector<std::string>& trace);
bool remove_node(adjacency_graph* graph, size_t node_id);
void edge_compacting(weigthed_labelled_automata& graph);
void edge_compacting(weigthed_labelled_automata &graph, size_t i);
void edge_normalize(weigthed_labelled_automata& graph);

#endif //CLASSIFIERS_ADJACENCY_GRAPH_H
