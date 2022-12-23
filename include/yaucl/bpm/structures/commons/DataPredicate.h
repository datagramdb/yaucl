/*
 * DataPredicate.h
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
// Created by giacomo on 10/03/21.
//

#ifndef BPM21_DATAPREDICATE_H
#define BPM21_DATAPREDICATE_H

#include <string>
#include <variant>
#include <ostream>
#include <vector>

#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/uset_hash.h>

/*namespace std {
    template <>
    struct hash<std::variant<std::string, double>>
    {
        std::size_t operator()(const union_minimal& k) const {
            return std::holds_alternative<double>(k) ?
                   (13*std::hash<double>{}(std::get<double>(k))) :
                   (31*std::hash<std::string>{}(std::get<std::string>(k)));
        }
    };

}*/

#include <unordered_set>
#include <yaucl/hashing/hash_combine.h>
#include <cmath>
#include <cfloat>
#include <set>
#include <yaucl/structures/default_constructors.h>

enum numeric_atom_cases {
    INTERVAL,
    LT,
    GT,
    LEQ,
    GEQ,
    EQ,
    NEQ,
    TTRUE
};

numeric_atom_cases invert_predicate_direction(numeric_atom_cases val);

std::string prev_char(const std::string& val, size_t max_size);
std::string next_char(const std::string& val, size_t max_size);

#define     PREV_DOUBLE(x_val)  (std::nextafter((x_val), -DBL_MAX))
#define     NEXT_DOUBLE(x_val)  (std::nextafter((x_val),  DBL_MAX))

#define MAXIMUM_STRING_LENGTH       (10)

using union_type = std::variant<double, size_t, long long, std::string, bool>;
using union_minimal = std::variant<std::string, double>;

struct DataPredicate {
    static double      MIN_DOUBLE;
    static double      MAX_DOUBLE;
    static std::string MIN_STRING;
    static std::string MAX_STRING;
    static size_t      msl;

    std::string                       label;
    std::string                       var;
    numeric_atom_cases                casusu;
    union_minimal                     value;
    std::string                       labelRHS;
    std::string                       varRHS;
    union_minimal                     value_upper_bound;
    std::set<union_minimal>           exceptions;
    std::vector<DataPredicate>        BiVariableConditions;
    bool                              wasReversed;

    static union_minimal prev_of(const union_minimal& x);
    static union_minimal next_of(const union_minimal& x);

    bool isStringPredicate() const;
    bool isDoublePredicate() const;
    bool isBiVariableCondition() const;
    std::variant<std::vector<std::pair<std::string, std::string>>,
            std::vector<std::pair<double, double>>> decompose_single_variable_into_intervals() const;


    std::variant<std::vector<std::pair<std::string, std::string>>,
            std::vector<std::pair<double, double>>> decompose_single_variable_into_intervals_with_missing() const;

    DataPredicate();
    DEFAULT_COPY_ASSGN(DataPredicate)
    
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const union_minimal &valueconst, const std::string &label = "");
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::string &value, const std::string &label = "");
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const double &value, const std::string &label = "");

    DataPredicate(const std::string& var, const std::string& varRHS, numeric_atom_cases casusu, const std::string& llabel = "", const std::string& rlabel = "") : var{var}, varRHS{varRHS}, casusu{casusu}, label{llabel}, labelRHS(rlabel) {}
    DataPredicate(const std::string& label, const std::string& var, double lb, double ub);
    DataPredicate(const std::string& label, const std::string& var, const std::string& lb, const std::string& ub);
    DataPredicate(const std::string &label, const std::string &var, union_minimal lb, union_minimal ub);

    DataPredicate flip() const {
        DataPredicate result;
        result.var = varRHS;
        result.varRHS = var;
        result.casusu = casusu;
        result.label = labelRHS;
        result.labelRHS = label;
        return result;
    }

    friend std::ostream &operator<<(std::ostream &os, const DataPredicate &predicate);
    void asInterval();
    bool intersect_with(const DataPredicate& predicate);
    bool testOverSingleVariable(const  std::string& val) const;
    bool testOverSingleVariable(double       val) const;

    DataPredicate instantiateRHSWith(const union_minimal& val) const;
    DataPredicate reverseBiVariablePredicate() const;

    bool operator==(const DataPredicate &rhs) const;
    bool operator!=(const DataPredicate &rhs) const;
};

#define     PREV_STRING(str)    (prev_char((str), DataPredicate::msl))
#define     NEXT_STRING(str)    (next_char((str), DataPredicate::msl))

namespace std {
    template <>
    struct hash<DataPredicate>
    {
        std::size_t operator()(const DataPredicate& k) const {
            size_t seed = 31;
            using yaucl::hashing::hash_combine;
            seed = hash_combine(seed, k.value);
            seed = hash_combine(seed, k.label);
            seed = hash_combine(seed, (unsigned int)k.casusu);
            seed = hash_combine(seed, k.var);
            if (k.casusu == INTERVAL) {
                seed = hash_combine(seed, k.value_upper_bound);
                {
                    size_t x = 17;
                    for (const auto& k : k.exceptions)
                        x = hash_combine(x, std::holds_alternative<double>(k) ?
                                        (13*std::hash<double>{}(std::get<double>(k))) :
                                        (31*std::hash<std::string>{}(std::get<std::string>(k))));
                    seed = hash_combine(seed, x);
                }
            }
            return seed;
        }
    };

}

#endif //BPM21_DATAPREDICATE_H
