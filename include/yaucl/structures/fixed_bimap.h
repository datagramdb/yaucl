/*
 * fixed_bimap.h
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 29/12/18.
//

#ifndef INCONSISTENCY_FIXED_BIMAP_H
#define INCONSISTENCY_FIXED_BIMAP_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include <functional>
#include <sstream>
#include <optional>

namespace yaucl {
    namespace structures {
        /**
 * A fixed bimap will help to store a bijection as a map of hash keys
 * @tparam K
 * @tparam V
 */
        template <typename K, typename V> class fixed_bimap {
            /**
             * This maps each key or value hash into the vector offset where they are stored in the vector.
             * By doing so, each element should be represented once, that is the offset where they appear.
             */
            std::unordered_map<std::string, size_t> map;
            std::vector<std::pair<K, V>> elements;

            size_t size = 0;

        public:
            fixed_bimap() = default;
            fixed_bimap(const fixed_bimap& ) = default;
            fixed_bimap(fixed_bimap&& ) = default;
            fixed_bimap& operator=(const fixed_bimap&) = default;

            void clear() {
                map.clear();
                elements.clear();
                size = 0;
            }

            const std::vector<std::pair<K, V>>& getEntryPoints() const {
                return elements;
            }

            /**
             * Always assumes that the key and the value are always with a bimap.
             * This method is implemented to reduce the amounts of checks for a boolean variable.
             *
             * @param key
             * @param value
             */
            void put(K key, V value) {
                elements.push_back(std::make_pair(key, value));
                std::ostringstream oss{}, vos{};
                oss << "k_" << key;
                vos << "v_" << value;
                map[oss.str()] = size;
                map[vos.str()] = size;
                size++;
            }

            /**
             * Updates the bimap iff. there was no previously set key with another value
             * @param key
             * @param value
             * @return          Either the new value, or the previously associated one.
             */
            V putWithKeyCheck(K key, V value) {
                std::ostringstream oss{}, vos{};
                oss << "k_" << key;
                std::string keyS = oss.str();
                std::unordered_map<std::string, size_t>::iterator it = map.find(keyS);
                if ( it == map.cend()) { // =>
                    elements.push_back(std::make_pair(key, value));
                    vos << "v_" << value;
                    map[keyS] = size;
                    map[vos.str()] = size;
                    size++;
                    return value;
                } else {
                    return elements.at(it->second);
                }
            }


            std::pair<K,V>* putWithRefKeyCheck(K key, V value) {
                std::ostringstream oss{}, vos{};
                oss << "k_" << key;
                std::string keyS = oss.str();
                std::unordered_map<std::string, size_t>::iterator it = map.find(keyS);
                if ( it == map.cend()) { // =>
                    elements.push_back(std::make_pair(key, value));
                    vos << "v_" << value;
                    map[keyS] = size;
                    map[vos.str()] = size;
                    size++;
                    return (elements.empty()) ? nullptr : &elements[elements.size()-1];

                } else {
                    return &elements[it->second];
                }
            }

            std::optional<const std::pair<K, V>*> hasKey(K key) {
                std::ostringstream oss{};
                oss << "k_" << key;
                const auto it = map.find(oss.str());
                if (it == map.end()) {
                    return {};
                } else {
                    return {&elements[it->second]};
                }
            }

            std::optional<const std::pair<K, V>*> hasValue(V key) {
                std::ostringstream oss{};
                oss << "v_" << key;
                const auto it = map.find(oss.str());
                if (it == map.end()) {
                    return {};
                } else {
                    return {&elements[(it->second)]};
                }
            }



            V getValue(K key) {
                std::ostringstream oss{};
                oss << "k_" << key;
                return elements[map[oss.str()]].second;
            }

            K getKey(V value) const {
                std::ostringstream vos{};
                vos << "v_" << value;
                return elements.at(map.at(vos.str())).first;
            }

        };


    }

}
#endif //INCONSISTENCY_FIXED_BIMAP_H
