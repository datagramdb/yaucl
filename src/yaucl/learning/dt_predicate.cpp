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

#include <yaucl/learning/dt_predicate.h>

bool dt_predicate::operator()(const union_minimal &val) const {
    switch (pred) {
        case L_THAN:
            return val < value;
        case LEQ_THAN:
            return val <= value;
        case G_THAN:
            return val > value;
        case GEQ_THAN:
            return val >= value;
        case IN_SET:
            return categoric_set.contains(val);
        case NOT_IN_SET:
            return !categoric_set.contains(val);
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const dt_predicate &predicate) {
    os << predicate.field;
    os << (predicate.pred == dt_predicate::L_THAN ? "<" : predicate.pred == dt_predicate::LEQ_THAN ? "<=" : predicate.pred == dt_predicate::G_THAN ? ">" : predicate.pred == dt_predicate::GEQ_THAN ? ">=" : predicate.pred == dt_predicate::IN_SET ? "∈{" : "∉{");
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
                if (std::holds_alternative<std::string>(*it))
                    os << std::get<std::string>(*it);
                else
                    os << std::get<double>(*it);
                it++;
                if ((it)!=en) os << ",";
            }
            return os <<"}";
    }
    return os;
}
