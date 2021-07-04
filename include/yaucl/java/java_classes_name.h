/*
 * java_classes_name.h
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
// Created by giacomo on 26/09/18.
//

#ifndef EDGEINDEXING_CLASSES_H
#define EDGEINDEXING_CLASSES_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>

/**
 * Easing the java rewriting
 */
typedef  std::string                    String;
template <typename N> using ArrayList = std::vector<N>;
template <typename N> using HashSet = std::unordered_set<N>;
template <typename K, typename V> using HashMap = std::unordered_map<K, V>;
template <typename K, typename V> using TreeMultimap = std::multimap<K, V>;
template <typename K, typename V> using HashMultimap = std::unordered_multimap<K, V>;


#endif //EDGEINDEXING_CLASSES_H
