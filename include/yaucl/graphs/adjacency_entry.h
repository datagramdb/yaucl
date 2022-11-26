/*
 * adjacency_entry.h
 * This file is part of yaucl
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl. If not, see <http://www.gnu.org/licenses/>.
 */


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
