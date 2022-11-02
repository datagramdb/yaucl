//
// Created by giacomo on 16/07/21.
//

#include "yaucl/graphs/adjacency_entry.h"

adjacency_entry::adjacency_entry(size_t id) : id{id}, is_starting{false}, is_final{false} {}

adjacency_entry::adjacency_entry(size_t id, bool is_starting, bool is_final) : id{id}, is_starting{is_starting}, is_final{is_final} {}

std::ostream &operator<<(std::ostream &os, const adjacency_entry &entry) {
    os << "\tNode #" << entry.id << ":" << std::endl;
    if (entry.is_starting)
        os << "\t -Starting Point" << std::endl;
    if (entry.is_final)
        os << "\t -Final Point"  << std::endl;
    for (const auto& cp : entry.outgoing_hash_to_outgoingNodeIdWithWeight) {
        for (const auto& edge : cp.second) {
            os << "\t  " << entry.id << " -[" << edge.second << "]-> " << edge.first << ':' << cp.first << std::endl;
        }
    }
    return os;
}
