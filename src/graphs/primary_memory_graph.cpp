//
// Created by giacomo on 05/07/21.
//

#include "yaucl/graphs/primary_memory_graph.h"
using namespace yaucl::graphs;

//
// Created by giacomo on 06/07/2020.
//

#include <yaucl/graphs/primary_memory_graph.h>
#include <iostream>

yaucl::graphs::primary_memory_graph::primary_memory_graph(std::ifstream &file, const std::string &s,bool skipFirstLine) {
    std::string line;
    std::regex rgx (s);
    size_t n = 0;
    std::unordered_map<std::string, size_t> mapp;
    if (skipFirstLine) std::getline(file, line);
    std::unordered_map<size_t, size_t> branchingEvaluator;
    while (std::getline(file, line)) {
        std::sregex_token_iterator iter(line.begin(), line.end(), rgx, -1);
        std::sregex_token_iterator end;

        std::string child_label = *iter++;
        std::string parent_label = *iter++;
        auto child = mapp.emplace(child_label, n + 1);
        if (child.second) {
            n++;
            single_content_to_id.emplace(child_label, n);
            add_vertex(n, {child_label}, {});
        }
        auto parent = mapp.emplace(parent_label, n + 1);
        if (parent.second) {
            n++;
            single_content_to_id.emplace(parent_label, n);
            add_vertex(n, {parent_label}, {});
        }
        auto it = branchingEvaluator.find(parent.first->second);
        if (it == branchingEvaluator.end()) {
            maxBranch = std::max(maxBranch, (size_t)1);
            branchingEvaluator.insert(std::make_pair(parent.first->second, 1));
        } else {
            maxBranch = std::max(++it->second, maxBranch);
        }
        double weight = std::stod(*iter++);
        addEdge(child.first->second, parent.first->second, weight);
    }
}

size_t
yaucl::graphs::primary_memory_graph::add_vertex(size_t id, const std::vector<std::string> &content, const std::vector<size_t> &nesting) {
    auto it = id_to_vector_offset.insert(std::make_pair(id, n));
    max_vertex_id = std::max(id, max_vertex_id);
    if (it.second) {
        vertices.emplace_back(id, content, nesting);
        size_t toReturn = n++;
        return toReturn;
    } else
        return it.first->second;
}


bool yaucl::graphs::primary_memory_graph::addEdge(size_t src, size_t dst, const edge_collateral &w) {
    auto src_it = id_to_vector_offset.find(src);
    if (src_it != id_to_vector_offset.cend()) {
        auto dst_it = id_to_vector_offset.find(dst);
        if (dst_it != id_to_vector_offset.cend()) {
            edge_size++;
            adjacency_list_entry& s = vertices.at(src_it->second);
            adjacency_list_entry& t = vertices.at(dst_it->second);
            s.outgoing.emplace(dst, w);
            t.ingoing.emplace(src, w);
            return true;
        }
    }
    return false;
}

bool yaucl::graphs::primary_memory_graph::addEdge(size_t src, size_t dst, double weight) {
    edge_collateral w{weight};
    return addEdge(src, dst, w);
}

const yaucl::graphs::adjacency_list_entry &primary_memory_graph::read_vertex(size_t id) const {
    // TODO: n == id_to_vector_offset
    return vertices.at(id_to_vector_offset.at(id));
}

adjacency_list_entry &yaucl::graphs::primary_memory_graph::get_vertex(size_t id) {
    // TODO: n == id_to_vector_offset
    return vertices.at(id_to_vector_offset.at(id));
}

size_t yaucl::graphs::primary_memory_graph::getVertexSize() const {
    return vertices.size();
}

size_t yaucl::graphs::primary_memory_graph::getEdgeSize() const {
    return edge_size;
}

bool yaucl::graphs::primary_memory_graph::removeAllEdges(size_t src, size_t dst) {
    adjacency_list_entry& src_v = get_vertex(src);
    auto src_it = src_v.outgoing.find(dst);
    if (src_it != src_v.outgoing.end()) {
        adjacency_list_entry& dst_v = get_vertex(dst);
        auto dst_it = dst_v.ingoing.find(src);
        if (dst_it != dst_v.ingoing.end()) {
            src_v.outgoing.erase(src_it);
            dst_v.ingoing.erase(dst_it);
            edge_size--;
            return true;
        }
    }
    return false;
}

size_t yaucl::graphs::primary_memory_graph::add_vertex_generate_id(const std::vector<std::string> &content,
                                                    const std::vector<size_t> &nesting) {
    size_t it = max_vertex_id+1;
    add_vertex(it, content, nesting);
    return it;
}

std::ostream &operator<<(std::ostream &os, const yaucl::graphs::primary_memory_graph &graph) {
    os << "|V| = " << graph.n << " |E| = " << graph.edge_size << std::endl;
    for (const auto& x : graph.vertices) {
        for (const auto& y : x.outgoing) {
            os << x.id << "-[" << y.second.weight << "]->" << y.first << std::endl;
        }
    }
    return os;
}

bool yaucl::graphs::primary_memory_graph::removeAllEdges_unsafe(size_t src, size_t dst) {
    bool doRemove = false;

    auto src_offset = id_to_vector_offset.find(src);
    if (src_offset != id_to_vector_offset.end()) {
        adjacency_list_entry& src_v = vertices.at(src_offset->second);
        auto src_it = src_v.outgoing.find(dst);
        if (src_it != src_v.outgoing.end()) {
            src_v.outgoing.erase(src_it);
            doRemove = true;
        }
    }

    auto dst_offset = id_to_vector_offset.find(dst);
    if (dst_offset != id_to_vector_offset.end()) {
        adjacency_list_entry& dst_v = vertices.at(dst_offset->second);
        auto dst_it = dst_v.ingoing.find(src);
        if (dst_it != dst_v.ingoing.end()) {
            dst_v.ingoing.erase(dst_it);
            doRemove = true;
        }
    }

    if (doRemove) edge_size--;
    return false;
}

void primary_memory_graph::removeAllNodes(std::vector<size_t> &nodes) {
    //std::queue<size_t> elements;
    std::set<size_t> set;
    for (const size_t& x : nodes) {
        auto y = id_to_vector_offset.at(x);
        //elements.emplace(y);
        set.emplace(y);
    }

    // Setting all the edges to remove
    std::set<std::pair<size_t, size_t>> edgesToRemove;
    for (const size_t& v : nodes) {
        auto& vv = vertices[id_to_vector_offset.at(v)];
        for (const auto& dst : vv.outgoing) {
            edgesToRemove.emplace(v, dst.first);
        }
        for (const auto& dst : vv.ingoing) {
            edgesToRemove.emplace(dst.first, v);
        }
    }

    // Removing all the vertices
    size_t k = 0;
    for (const size_t& x: set) {
        vertices.erase(vertices.begin() + x - k);
        k++;
    }
    for (const size_t& x : nodes) {
        id_to_vector_offset.erase(id_to_vector_offset.find(x));
    }

    // Restructuring the ids
    size_t j = 0;
    auto it = set.begin();
    if (it != set.end()) {
        size_t array_offset_start = *it;
        size_t n = vertices.size();
        if (array_offset_start < vertices.size()) {
            for ( ; array_offset_start<n; array_offset_start++) {
                id_to_vector_offset[vertices[array_offset_start].id] = array_offset_start;
            }
        }
    }
    /*while (!set.empty()) {
        size_t val = elements.front();
        elements.pop();
        j++;
        size_t max;
        if (elements.empty()) {
            max = vertices.size();
        } else {
            max = elements.front();
        }
        for (size_t i = val; i<max; i++) {
            size_t id = vertices[val - j].id;
            id_to_vector_offset[id] = id_to_vector_offset[id]-j;
        }
    }*/

    // Removing all the edges
    for (const std::pair<size_t, size_t>& cp : edgesToRemove) {
        removeAllEdges_unsafe(cp.first, cp.second);
    }

    n -= set.size();
}

