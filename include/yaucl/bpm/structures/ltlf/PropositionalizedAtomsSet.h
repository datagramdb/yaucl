/*
 * bpm21
 * This file is part of <program name>
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * <program name> is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * <program name> is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with <program name>. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 20/02/21.
//

#ifndef CLASSIFIERS_PROPOSITIONALIZEDATOMSSET_H
#define CLASSIFIERS_PROPOSITIONALIZEDATOMSSET_H

#include <string>
#include <yaucl/hashing/pair_hash.h>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <optional>



class PropositionalizedAtomsSet {
public:
    using ItemWithTerminality = std::pair<std::string, bool>;
    PropositionalizedAtomsSet() {}
    PropositionalizedAtomsSet(const PropositionalizedAtomsSet&) = default;
    PropositionalizedAtomsSet& operator=(const PropositionalizedAtomsSet&) = default;

    bool insert(const std::string& atom, bool isTerminal);
    std::unordered_set<std::string>::iterator begin();
    std::unordered_set<std::string>::iterator end();
    std::optional<bool> hasTerminalCondition(const std::string& atom);

    bool containsInSet(const std::string& atom) const {
        return set.contains(atom);
    }

    std::string different_label() const {
        return sstr;
    }

private:
    std::string sstr;
    std::unordered_set<std::string> set;
    std::unordered_map<std::string, bool> terminality_condition;
};


#endif //CLASSIFIERS_PROPOSITIONALIZEDATOMSSET_H
