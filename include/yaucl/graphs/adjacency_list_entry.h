//
// Created by giacomo on 06/07/2020.
//

#ifndef PRIMARY_MEMORY_LATTICE_ADJACENCY_LIST_ENTRY_H
#define PRIMARY_MEMORY_LATTICE_ADJACENCY_LIST_ENTRY_H

#include <string>
#include <vector>
#include <unordered_map>

namespace yaucl {
    namespace graphs {

        struct edge_collateral {
            double weight;
            size_t merges = 1;

            void merge_with(const struct edge_collateral& out) {
                merges++;
                weight += out.weight;
            }
            edge_collateral& normalize() {
                weight = weight / ((double)merges);
                merges = 1;
                return *this;
            }
            edge_collateral() : weight{1.0}, merges{1} {}
            edge_collateral(double w) : weight{w}, merges{1} {}
            edge_collateral(const edge_collateral&) = default;
            edge_collateral& operator=(const edge_collateral&) = default;
        };

        struct adjacency_list_entry {
            size_t id;
            std::vector<std::string> content;
            std::vector<size_t> nesting;
            std::unordered_map<size_t, edge_collateral> ingoing;
            std::unordered_map<size_t, edge_collateral> outgoing;
            bool isHierarchyRoot = false;   /// <<@ Determines whether the current element is a root of a hierarchy
            bool isUpperHierarchy = false;  /// <<@ Determines if this element is part of the hierarchy (it has only one ancestor) or not.

            bool leaves_bot = false,lattice_spreading = false/*, root = false*/;

            adjacency_list_entry() : isHierarchyRoot{false}, isUpperHierarchy{false} {}
            adjacency_list_entry(size_t id, const std::vector<std::string> &content, const std::vector<size_t> &nesting,
                                 const std::unordered_map<size_t, edge_collateral> &ingoing,
                                 const std::unordered_map<size_t, edge_collateral> &outgoing, bool isHierarchyRoot,
                                 bool isUpperHierarchy);
            adjacency_list_entry(size_t id, const std::vector<std::string> &content, const std::vector<size_t> &nesting);
            adjacency_list_entry(const adjacency_list_entry& ) = default;
            adjacency_list_entry(adjacency_list_entry&& ) = default;
            adjacency_list_entry& operator=(const adjacency_list_entry&) = default;
        };

    }
}


#endif //PRIMARY_MEMORY_LATTICE_ADJACENCY_LIST_ENTRY_H
