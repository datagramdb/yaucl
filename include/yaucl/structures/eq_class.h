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

#endif //CLASSIFIERS_EQUIVALENCE_CLASS_H
