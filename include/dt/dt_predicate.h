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
#include <unordered_map>
#include <ostream>
using union_minimal = std::variant<std::string,double>;
using record = std::vector<std::pair<std::string,union_minimal>>;
using data_record = std::vector<record>;
using data_clazzes = std::vector<int>;

enum operand_type {
    SAME = 0,
    LOG10 = 1,
    LOG2 = 2,
    EXP = 3,
    SQRT = 4,
    EXP2 = 5,
    _2EXP = 6,
    LN = 7
};

struct dt_predicate {
    enum type {
        L_THAN,
        LEQ_THAN,
        G_THAN,
        GEQ_THAN,
        IN_SET,
        NOT_IN_SET,
        OBL_PREDICATE_LEQ,
        OBL_PREDICATE_GT
    };

    std::string field;
    union_minimal value;
    std::unordered_set<std::string> categoric_set;
    std::unordered_map<std::string,double> category_weights;
    type pred;
    operand_type argfun = operand_type::SAME;

    dt_predicate() = default;
    dt_predicate(const dt_predicate&) = default;
    dt_predicate(dt_predicate&& ) = default;
    dt_predicate& operator=(const dt_predicate&) = default;
    dt_predicate& operator=(dt_predicate&& ) = default;

    dt_predicate(const std::string& value, const std::string&field) : field{field}, pred{IN_SET} {
        categoric_set.insert(value);
        pred = IN_SET;
    }

    dt_predicate(const std::unordered_map<std::string,double>& w, const double&bias) : category_weights{w}, value{bias}, pred(OBL_PREDICATE_LEQ) {

    }

    bool operator==(const dt_predicate& x) const {
        if (argfun != x.argfun) return false;
        if (pred != x.pred) return false;
        if (field != x.field) return false;
        if (value != x.value) return false;
        if (categoric_set != x.categoric_set) return false;
        std::unordered_set<std::string> keys;
        for (const auto&[k,v] : category_weights) {
            if (!x.category_weights.contains(k)) return false;
            keys.emplace(k);
        }
        for (const auto&[k,v] : x.category_weights) {
            if (!keys.contains(k)) return false;
        }
        for (const auto& z : keys)
            if (category_weights.at(z) != x.category_weights.at(z))
                return false;
        return true;
    }
    friend std::ostream& operator<<(std::ostream& os, const dt_predicate &predicate);

    bool operator()(const union_minimal& val) const;
    bool operator()(const std::vector<std::pair<std::string,union_minimal >>& record) const;
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

#include<cmath>

static inline double do_operation(double val, operand_type ot) {
    {
        switch (ot) {

            case operand_type::LOG10:
                return std::log10(std::abs(val)+1);
                break;
            case operand_type::LOG2:
                return std::log2(std::abs(val)+1);
                break;
            case operand_type::EXP:
                return std::exp(val);
                break;
            case operand_type::SQRT:
                return std::sqrt(std::abs(val));
                break;
            case operand_type::EXP2:
                return std::pow(val,2);
                break;
            case operand_type::_2EXP:
                return std::pow(2,val);
                break;
            case operand_type::LN:
                return std::log(std::abs(val)+1);
                break;
            default: // case operand_type::SAME
                return val;
                break;
        }
    }
}

#endif //DISTANCE_DT_PREDICATE_H
