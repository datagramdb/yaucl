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

template <typename K, typename V>
std::ostream& operator<< (std::ostream& out, const std::pair<K, V>& v) {
    return out << "«" << v.first << ", " << v.second << "»";
}

#endif //YAUCL_SERIALIZER_H
