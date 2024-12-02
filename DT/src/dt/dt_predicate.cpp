/*
 * dt_predicate.cpp
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

#include <dt/dt_predicate.h>

bool dt_predicate::operator()(const union_minimal &val) const {
    union_minimal v;
    if (std::holds_alternative<double>(val))
        v =do_operation(std::get<double>(val), this->argfun);
    else
        v = val;
    switch (pred) {
        case L_THAN:
            return v< value;
        case LEQ_THAN:
            return v <= value;
        case G_THAN:
            return v > value;
        case GEQ_THAN:
            return v >= value;
        case IN_SET:
            return categoric_set.contains(std::get<std::string>(val));
        case NOT_IN_SET:
            return !categoric_set.contains(std::get<std::string>(val));
        case OBL_PREDICATE_LEQ:
        case OBL_PREDICATE_GT:
            throw std::runtime_error("ERROR: UNSUPPORTED OPERATION: THIS REQUIRES AN ENTIRE RECORD!");
    }
    return false;
}

#include <dt/structures/find_argument.h>

bool dt_predicate::operator()(const std::vector<std::pair<std::string,union_minimal >>& record) const {
    if ((pred != OBL_PREDICATE_LEQ) && (pred != OBL_PREDICATE_GT)) {
        std::pair<std::string,union_minimal> cp{field, find_argument::min};
        auto it = std::lower_bound(record.begin(), record.end(), cp);
        if (it != record.end()) {
            return operator()(it->second);
        } else {
            return operator()(0.0);
        }
    } else {
        double compute = 0.0;
        for (const auto& [k,v] : record) {
            auto it = category_weights.find(k);
            if (it != category_weights.end()) {
                compute += (it->second * std::get<double>(v));
            }
        }
        if (pred == OBL_PREDICATE_LEQ) {
            return compute <= std::get<double>(value);
        } else {
            return compute > std::get<double>(value);
        }
    }
}

#include <iomanip>

std::ostream& operator<<(std::ostream& os, const dt_predicate &predicate) {
    auto t = predicate.pred;
    auto val = predicate.value;
    if ((predicate.pred == dt_predicate::OBL_PREDICATE_LEQ) || (predicate.pred == dt_predicate::OBL_PREDICATE_GT)) {
        val = 0.0;
        if (predicate.pred == dt_predicate::OBL_PREDICATE_LEQ) {
            t = dt_predicate::LEQ_THAN;
        } else {
            t = dt_predicate::G_THAN;
        }
        for (const auto[k,v] : predicate.category_weights) {
            os << std::quoted(k) << "*" << v << "+";
        }
        os << "(-(" << std::get<double>(predicate.value) << "))";
    } else {
        os << predicate.field;
    }
    os << (t == dt_predicate::L_THAN ? "<" : t == dt_predicate::LEQ_THAN ? "<=" : t == dt_predicate::G_THAN ? ">" : t == dt_predicate::GEQ_THAN ? ">=" : t == dt_predicate::IN_SET ? "∈{" : "∉{");
    switch (predicate.pred) {
        case dt_predicate::L_THAN:
        case dt_predicate::LEQ_THAN:
        case dt_predicate::G_THAN:
        case dt_predicate::GEQ_THAN:
            if (std::holds_alternative<std::string>(predicate.value))
                return os << std::get<std::string>(predicate.value);
            else
                return os << std::get<double>(predicate.value);
        case dt_predicate::IN_SET:
        case dt_predicate::NOT_IN_SET:
            for (auto it = predicate.categoric_set.begin(), en = predicate.categoric_set.end(); it != en; ) {
//                if (std::holds_alternative<std::string>(*it))
                    os << (*it);
//                else
//                    os << std::get<double>(*it);
                it++;
                if ((it)!=en) os << ",";
            }
            return os <<"}";
        default:
            return os;
    }
    return os;
}
