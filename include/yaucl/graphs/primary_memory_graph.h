//
// Created by giacomo on 06/07/2020.
//

#ifndef PRIMARY_MEMORY_LATTICE_PRIMARY_MEMORY_GRAPH_H
#define PRIMARY_MEMORY_LATTICE_PRIMARY_MEMORY_GRAPH_H



#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>
#include "adjacency_list_entry.h"
#include <ostream>
#include <set>
#include <queue>

namespace yaucl {
    namespace graphs {

        struct primary_memory_graph {
            size_t n = 0;
            std::unordered_map<size_t, size_t> id_to_vector_offset;
            std::vector<adjacency_list_entry> vertices;
            std::unordered_map<std::string, size_t> single_content_to_id;
            size_t max_vertex_id = 0;
            size_t edge_size = 0;
            size_t maxBranch = 0;

            primary_memory_graph() {}

            /**
             * Reading a lattice as represented in a file
             * @param file          Filename associated to the hierarchy
             * @param s             Regex splitting the component of the file into source label, target label and destination label
             */
            primary_memory_graph(std::ifstream& file, const std::string &s, bool skipFirstLine = false);



            size_t add_vertex(size_t id, const std::vector<std::string>& content, const std::vector<size_t>& nesting);
            size_t add_vertex_generate_id(const std::vector<std::string>& content, const std::vector<size_t>& nesting);
            bool addEdge(size_t src, size_t dst, double weight);
            bool addEdge(size_t src, size_t dst, const edge_collateral& content);
            bool removeAllEdges(size_t src, size_t dst);

            const adjacency_list_entry& read_vertex(size_t id) const;
            adjacency_list_entry& get_vertex(size_t id);

            size_t getVertexSize() const;
            size_t getEdgeSize() const;

            void removeAllNodes(std::vector<size_t>& nodes) {
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

            friend std::ostream &operator<<(std::ostream &os, const primary_memory_graph &graph);

        private:
            bool removeAllEdges_unsafe(size_t src, size_t dst);
        };

    }
}


#endif //PRIMARY_MEMORY_LATTICE_PRIMARY_MEMORY_GRAPH_H
