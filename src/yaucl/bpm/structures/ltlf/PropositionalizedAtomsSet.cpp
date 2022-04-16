/*
 * PropositionalizedAtomsSet.cpp
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
// Created by giacomo on 18/02/21.
//

#include <yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.h>

bool PropositionalizedAtomsSet::insert(const std::string &atom, bool isTerminal) {
    if (set.insert(atom).second) {
        sstr += (atom + "_");
        terminality_condition.emplace(atom, isTerminal);
        return true;
    } else {
        auto it = terminality_condition.find(atom);
        bool result = it->second != isTerminal;
        it->second = it->second && isTerminal;
        return result;
    }
}

std::unordered_set<std::string>::iterator PropositionalizedAtomsSet::begin() {
    return set.begin();
}

std::unordered_set<std::string>::iterator PropositionalizedAtomsSet::end() {
    return set.end();
}

std::optional<bool> PropositionalizedAtomsSet::hasTerminalCondition(const std::string &atom) {
    auto it = terminality_condition.find(atom);
    return (it == terminality_condition.end()) ? std::optional<bool>() : std::optional<bool>{it->second};
}
