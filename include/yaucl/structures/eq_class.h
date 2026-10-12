/*
 * eq_class.h
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
// Created by giacomo on 28/02/21.
//

#ifndef CLASSIFIERS_EQUIVALENCE_CLASS_H
#define CLASSIFIERS_EQUIVALENCE_CLASS_H

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <unordered_set>

template<typename T>
struct equivalence_class {
    std::set<std::pair<T,T>> equivalence_relationship;
    std::set<T> items;
    std::unordered_map<T, std::unordered_set<T>> class_id_to_members;

    equivalence_class() : hasTransition{false} {};

    void insert(const T& left, const T& right) {
        equivalence_relationship.emplace(left, right); //
        equivalence_relationship.emplace(right, left); // Symmetry
        equivalence_relationship.emplace(right, right); // Identity
        equivalence_relationship.emplace(left, left); // Identity
        items.insert(left);
        items.insert(right);
        hasTransition = false;
        class_id_to_members.clear();
    }

    void insert_monodirectional(const T& left, const T& right) {
        equivalence_relationship.emplace(left, right);
        equivalence_relationship.emplace(right, right); // Identity
        equivalence_relationship.emplace(left, left); // Identity
        items.insert(left);
        items.insert(right);
        hasTransition = false;
        class_id_to_members.clear();
    }

    std::unordered_map<T, std::unordered_set<T>> calculateEquivalenceClass() {
        if (!hasTransition) {
            // Transitive
            for (const auto& k : items) {
                for (const auto& i : items) {
                    for (const auto& j : items) {
                        if (equivalence_relationship.contains(std::make_pair(i, j)) || (equivalence_relationship.contains(std::make_pair(i, k)) && equivalence_relationship.contains(std::make_pair(k, j))))
                            equivalence_relationship.emplace(i, j);
                    }
                }
            }
            hasTransition = true;

            std::unordered_map<T, T> non_class_ref_element_to_class_id;

            for (auto it = equivalence_relationship.begin(); it != equivalence_relationship.end(); ) {
                if ((!non_class_ref_element_to_class_id.contains(it->second)) &&
                    (((items.contains(it->first)) || class_id_to_members.contains(it->first)))) {
                    items.erase(it->first);
                    items.erase(it->second);
                    class_id_to_members[it->first].emplace(it->second);
                    if (it->first != it->second) {
                        non_class_ref_element_to_class_id[it->second] = it->first;
                    }
                }
                it = equivalence_relationship.erase(it);
            }
        }
        return class_id_to_members;
    }

private:
    bool hasTransition = false;

};

template <typename T>
class equivalence_classv2 {
private:
    mutable std::vector<size_t> parent;
    std::vector<size_t> rank;
    std::unordered_map<T, size_t> item_to_id;
    // Keep track of the reverse mapping to reconstruct the elements from IDs
    std::vector<T> id_to_item;

    size_t get_or_create_id(const T& element) {
        auto it = item_to_id.find(element);
        if (it != item_to_id.end()) {
            return it->second;
        }
        
        size_t new_id = parent.size();
        parent.push_back(new_id);
        rank.push_back(0);
        item_to_id[element] = new_id;
        id_to_item.push_back(element); // Store reverse mapping
        return new_id;
    }

    size_t find_set(size_t i) const {
        if (parent[i] == i)
            return i;
        return parent[i] = find_set(parent[i]);
    }

public:
    void add_equality(const T& a, const T& b) {
        size_t root_a = find_set(get_or_create_id(a));
        size_t root_b = find_set(get_or_create_id(b));

        if (root_a != root_b) {
            if (rank[root_a] < rank[root_b]) {
                parent[root_a] = root_b;
            } else if (rank[root_a] > rank[root_b]) {
                parent[root_b] = root_a;
            } else {
                parent[root_b] = root_a;
                rank[root_a]++;
            }
        }
    }

    bool are_equal(const T& a, const T& b) const {
        auto it_a = item_to_id.find(a);
        auto it_b = item_to_id.find(b);
        if (it_a == item_to_id.end() || it_b == item_to_id.end()) {
            return a == b;
        }
        return find_set(it_a->second) == find_set(it_b->second);
    }

    // Extraction Method
    std::vector<std::vector<T>> get_equivalence_classes() const {
        // Map root ID -> list of elements belonging to that root's class
        std::unordered_map<size_t, std::vector<T>> groups;

        // Group every tracked item by its absolute root parent
        for (size_t id = 0; id < parent.size(); ++id) {
            size_t root = find_set(id);
            groups[root].push_back(id_to_item[id]);
        }

        // Flatten the map into a vector of vectors
        std::vector<std::vector<T>> equivalence_classes;
        equivalence_classes.reserve(groups.size());
        for (auto& [root, elements] : groups) {
            equivalence_classes.push_back(std::move(elements));
        }

        return equivalence_classes;
    }
};

#endif //CLASSIFIERS_EQUIVALENCE_CLASS_H
