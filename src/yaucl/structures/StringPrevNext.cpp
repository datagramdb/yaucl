/*
 * StringPrevNext.cpp
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
// Created by giacomo on 11/03/21.
//

#include <yaucl/bpm/structures/commons/DataPredicate.h>
#include "yaucl/structures/StringPrevNext.h"

std::string StringPrevNext::getPrev(const std::string &elem) const {
    return PREV_STRING(elem);
}

std::string StringPrevNext::getNext(const std::string &elem) const {
    return NEXT_STRING(elem);
}

size_t StringPrevNext::distance(const std::string &l, const std::string &r) const {
    return (l == r) ? 0 : 1;
}
