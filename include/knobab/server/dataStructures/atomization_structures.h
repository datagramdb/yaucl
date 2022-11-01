//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_ATOMIZATION_STRUCTURES_H
#define KNOBAB_SERVER_ATOMIZATION_STRUCTURES_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <yaucl/bpm/structures/commons/DataPredicate.h>

using label_var_atoms_map_t = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<DataPredicate>>>;
using label_set_t = std::unordered_set<std::string>;
using semantic_atom_set = std::unordered_set<std::string>;

#endif //KNOBAB_SERVER_ATOMIZATION_STRUCTURES_H
