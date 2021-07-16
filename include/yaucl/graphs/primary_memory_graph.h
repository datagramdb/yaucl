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
            std::unordered_map<std::string, size_t> single_content_to_id; //node-label bijection
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

            void removeAllNodes(std::vector<size_t>& nodes);

            friend std::ostream &operator<<(std::ostream &os, const primary_memory_graph &graph);

        private:
            bool removeAllEdges_unsafe(size_t src, size_t dst);
        };

    }
}


#endif //PRIMARY_MEMORY_LATTICE_PRIMARY_MEMORY_GRAPH_H
