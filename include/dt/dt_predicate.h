/*
 * dt_predicate.h
 * This file is part of yaucl-learning
 *
 * Copyright (C) 2022 - Giacomo Bergami, Samuel Appleby
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
#include "commons.h"


struct dt_predicate {
    enum type {
        L_THAN,
        LEQ_THAN,
        G_THAN,
        GEQ_THAN,
        IN_SET,
        NOT_IN_SET
    };
    std::string field;
    union_minimal value;
    std::unordered_set<std::string> categoric_set;
    type pred;

    dt_predicate() = default;
    dt_predicate(const dt_predicate&) = default;
    dt_predicate(dt_predicate&& ) = default;
    dt_predicate& operator=(const dt_predicate&) = default;
    dt_predicate& operator=(dt_predicate&& ) = default;

    dt_predicate(const std::string& value, const std::string&field) : field{field} {
        categoric_set.insert(value);
        pred = IN_SET;
    }

    bool operator==(const dt_predicate& x) const {
        if (pred != x.pred) return false;
        if (field != x.field) return false;
        if (value != x.value) return false;
        return categoric_set != x.categoric_set;
    }
    friend std::ostream& operator<<(std::ostream& os, const dt_predicate &predicate);

    bool operator()(const union_minimal& val) const;
};

namespace std {
    template <>
    struct hash<dt_predicate> {
        const std::hash<std::string> f;
        size_t operator()(const dt_predicate& x) const {
            return ((size_t)x.pred) ^ f(x.field);
        }
    };
}

#endif //DISTANCE_DT_PREDICATE_H
