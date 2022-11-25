//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_BASE_H
#define KNOBAB_SERVER_BASE_H

#include <string>
#include <set>
#include <unordered_map>
#include <yaucl/structures/set_operations.h>
#include <vector>
#include <map>

using clause_id = size_t;
using clause_name = std::string;
using activity_label = std::string;
using activation_label = activity_label;
using target_label = activity_label;
using atomized_data_conditions = std::set<std::string>;
template <typename K, typename V> using umap = std::unordered_map<K,V>;
template <typename K, typename V> using map = std::map<K,V>;
using N_argument = size_t;
template <typename K, typename V> using keysetmap = std::map<std::set<K>, V, set_less<K>>;
using dataconditions_to_ids = keysetmap<std::string, std::vector<clause_id>>;

#endif //KNOBAB_SERVER_BASE_H
