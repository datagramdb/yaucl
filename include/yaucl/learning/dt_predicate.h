/*
 * dt_predicate.h
 * This file is part of yaucl-learning
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl-learning is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl-learning is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl-learning. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 03/11/22.
//

#ifndef DISTANCE_DT_PREDICATE_H
#define DISTANCE_DT_PREDICATE_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_set>
#include <ostream>


using simple_data = std::variant<std::string, double>;

struct dt_predicate {
    enum type {
        LEQ_THAN,
        IN_SET
    };
    std::string field;
    simple_data value;
    std::unordered_set<simple_data> categoric_set;
    type pred;

    dt_predicate() = default;
    dt_predicate(const dt_predicate&) = default;
    dt_predicate(dt_predicate&& ) = default;
    dt_predicate& operator=(const dt_predicate&) = default;
    dt_predicate& operator=(dt_predicate&& ) = default;


    friend std::ostream& operator<<(std::ostream& os, const dt_predicate &predicate);

    bool operator()(const simple_data& val) const;
};

#endif //DISTANCE_DT_PREDICATE_H
