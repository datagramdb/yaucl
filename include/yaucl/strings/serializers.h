/*
 * serializer.h
 * This file is part of yaucl
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

#ifndef YAUCL_SERIALIZER_H
#define YAUCL_SERIALIZER_H

#include <utility>
#include <ostream>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>

template<typename T, typename V>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<T,V> &insertion) {
    for (const auto& cp : insertion) {
        os << "{" << cp.first << ": " << cp.second << "}";
    }
    return os;
}

template<typename T, typename V>
std::ostream &operator<<(std::ostream &os, const std::map<T,V> &insertion) {
    for (const auto& cp : insertion) {
        os << "{" << cp.first << ": " << cp.second << "}";
    }
    return os;
}

template<typename T, typename V>
std::ostream &operator<<(std::ostream &os, const std::pair<T,V> &insertion) {
    return os << "<" << insertion.first << ", " << insertion.second << ">";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &insertion) {
    os << '[';
    for (size_t i = 0, N = insertion.size(); i<N; i++) {
        os << insertion.at(i);
        if (i < (N-1)) os << ", ";
    }
    return os << ']';
}

#include <set>

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &insertion) {
    os << '{';
    auto it = insertion.begin();
    for (size_t i = 0, N = insertion.size(); i<N; i++) {
        os << *it;
        if (i < (N-1)) os << ", ";
        it++;
    }
    return os << '}';
}

#endif //YAUCL_SERIALIZER_H
