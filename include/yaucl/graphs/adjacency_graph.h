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

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <yaucl/numeric/ssize_t.h>

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

    void dot(std::ostream& os);

private:
    void printAllPathsUtil(size_t u, size_t d, std::unordered_set<size_t>& visited, std::vector<ssize_t>& path, size_t path_index, std::unordered_set<size_t>& visited_src_dst, std::unordered_set<size_t>& global);
};


#endif //CLASSIFIERS_ADJACENCY_GRAPH_H
