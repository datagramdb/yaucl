//
// Created by giacomo on 16/07/21.
//

#ifndef AUTOMATA_JOIN_ADJACENCY_ENTRY_H
#define AUTOMATA_JOIN_ADJACENCY_ENTRY_H

#include <map>
#include <string>
#include <vector>
#include <ostream>

/**
 * Data Structure providing the adjacency list entry for one single node
 */
struct adjacency_entry {
    size_t id;
    std::map<std::string, std::vector<std::pair<size_t, double>>> outgoing_hash_to_outgoingNodeIdWithWeight;
    bool is_starting;
    bool is_final;

    virtual ~adjacency_entry() {
        outgoing_hash_to_outgoingNodeIdWithWeight.clear();
    };
    adjacency_entry(size_t id);
    adjacency_entry(size_t id, bool is_starting, bool is_final);
    adjacency_entry() = default;
    adjacency_entry(const adjacency_entry&) = default;
    adjacency_entry& operator=(const adjacency_entry&) = default;

    friend std::ostream &operator<<(std::ostream &os, const adjacency_entry &entry);
};


#endif //AUTOMATA_JOIN_ADJACENCY_ENTRY_H
