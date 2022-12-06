/*
 * adjacency_entry.cpp
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
