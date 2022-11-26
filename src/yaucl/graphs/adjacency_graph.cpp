/*
 * adjacency_graph.cpp
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

#include "yaucl/graphs/adjacency_graph.h"

adjacency_graph::adjacency_graph() : V_size(0), E_size(0) {}

size_t adjacency_graph::add_node() {
    nodes.emplace_back();
    return V_size++;
}

size_t adjacency_graph::add_edge(size_t src, size_t dst) {
    edge_ids.emplace_back(src, dst);
    ingoing_edges[dst].emplace_back(E_size);
    return nodes.at(src).emplace_back(E_size++);
}

std::pair<size_t, size_t> adjacency_graph::add_undirected_edge(size_t src, size_t dst) {
    return {add_edge(src,dst), add_edge(dst, src)};
}

const std::pair<size_t, size_t> &adjacency_graph::edge_from_id(size_t edge_id) const {
    return edge_ids.at(edge_id);
}

const std::vector<size_t> &adjacency_graph::getOutgoingEdgesId(size_t node_id) const {
    return nodes.at(node_id);
}

const std::vector<size_t> &adjacency_graph::getIngoingEdgesId(size_t node_id) const {
    return ingoing_edges.at(node_id);
}

void adjacency_graph::DFSUtil(size_t src, std::unordered_set<size_t> &visited) {
    visited.insert(src);

    for (size_t edge_id : nodes.at(src)) {
        size_t dst = edge_ids.at(edge_id).second;
        if (!visited.contains(dst))
            DFSUtil(dst, visited);
    }
}

void adjacency_graph::dot(std::ostream &os) {
    os << "digraph finite_state_machine {\n"
          "    rankdir=LR;\n"
          "    size=\"8,5\"\n";
    for (int node_id = 0, N = nodes.size(); node_id<N; node_id++) {
        std::string shape = "circle";
        os << "node [shape = circle, label=\"" << std::to_string(node_id) << "\", fontsize=10] q" << node_id << ";\n";
    }
    os << "\n\n";
    for (int node_id = 0, N = nodes.size(); node_id<N; node_id++) {
        std::vector<size_t> outgoing = nodes.at(node_id);
        for (const size_t edge_id : outgoing) {
            size_t dst = edge_ids.at(edge_id).second;
            os << "q" << node_id << " -> q" << dst << ";\n";
        }
    }
    os << "}";
}

void
adjacency_graph::printAllPathsUtil(size_t u, size_t d, std::unordered_set<size_t> &visited, std::vector<ssize_t> &path,
                                   size_t path_index, std::unordered_set<size_t> &visited_src_dst, std::unordered_set<size_t> &global) {
    // Mark the current node and store it in path[]
    global.insert(u);
    visited.insert(u);
    path[path_index] = u;
    path_index++;

    // If current vertex is same as destination, then print
    // current path[]
    if (u == d) {
        for (int i = 0; i < path_index; i++)
            visited_src_dst.insert(path[i]);
    } else {
        for (size_t edge_id : nodes.at(u)) {
            size_t dst = edge_ids.at(edge_id).second;
            if ((!visited.contains(dst)) && (!global.contains(dst)))
                printAllPathsUtil(dst, d, visited, path, path_index, visited_src_dst, global);
        }
    }

    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited.erase(u);
}
