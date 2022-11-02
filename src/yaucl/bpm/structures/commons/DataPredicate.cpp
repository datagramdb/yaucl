/*
 * DataPredicate.cpp
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
 *0
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */
//
// Created by giacomo on 10/03/21.
//

#include <iomanip>
#include "yaucl/bpm/structures/commons/DataPredicate.h"

numeric_atom_cases invert_predicate_direction(numeric_atom_cases val) {
    switch (val) {
        case LT:
            return GT;
        case GT:
            return LT;
        case LEQ:
            return GEQ;
        case GEQ:
            return LEQ;
        case EQ:
        case NEQ:
        case TTRUE:
            return val;
        case INTERVAL:
            throw std::runtime_error("ERROR: you cannot invert a relationship which is just an interval! An interval is not a valid predicate, rather than a combination of intervals. There is some flaw in the logic!");
    }
}

double      DataPredicate::MIN_DOUBLE = -std::numeric_limits<double>::max();
double      DataPredicate::MAX_DOUBLE =  std::numeric_limits<double>::max();
std::string DataPredicate::MIN_STRING = "";
std::string DataPredicate::MAX_STRING =  std::string(MAXIMUM_STRING_LENGTH, std::numeric_limits<char>::max());
size_t      DataPredicate::msl = MAXIMUM_STRING_LENGTH;

DataPredicate::DataPredicate() : casusu{TTRUE}, wasReversed{false} {}


DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const union_minimal &value, const std::string &label) : var(
        var), casusu(casusu), value(value), wasReversed{false}, label(label)  {}

DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::string &value, const std::string &label) : var(var), label(label) , wasReversed{false} , casusu(casusu), value(value) {}

DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const double &value, const std::string &label) : var(var), label(label), wasReversed{false} , casusu(casusu), value(value) {}


#include <cassert>
#include <yaucl/functional/assert.h>


std::ostream &operator<<(std::ostream &os, const DataPredicate &predicate) {
    if ((predicate.casusu == TTRUE) ) {
        if (predicate.BiVariableConditions.empty())
            os << "true";
    } else if (predicate.casusu == INTERVAL) {
        double isString = std::holds_alternative<std::string>(predicate.value);
        DEBUG_ASSERT(predicate.varRHS.empty() && predicate.labelRHS.empty());

        if (isString)
            os << std::quoted(std::get<std::string>(predicate.value).c_str());
        else
            os << std::get<double>(predicate.value);
        os << " ≤ ";
        if (!predicate.label.empty())
            os <<  predicate.label << '.';
        os << predicate.var << " ≤ ";
        if (isString)
            os << std::quoted(std::get<std::string>(predicate.value_upper_bound).c_str());
        else
            os << std::get<double>(predicate.value_upper_bound);
        if (!predicate.exceptions.empty()) {
            os << "\\ {";
            size_t N = predicate.exceptions.size();
            size_t i = 0;
            for (const auto& x : predicate.exceptions) {
                if (isString)
                    os << std::quoted(std::get<std::string>(x).c_str());
                else
                    os << std::get<double>(x);
                if (i != (N-1)) {
                    os << ", ";
                    i++;
                }
            }
            os << "}";
        }
        return os;
    } else {
        if (!predicate.label.empty())
            os  << predicate.label << '.';
        os << predicate.var;
        switch (predicate.casusu) {
            case LT:
                os << " < "; break;
            case GT:
                os << " > "; break;
            case LEQ:
                os << " ≤ "; break;
            case GEQ:
                os << " ≥ "; break;
            case EQ:
                os << " = "; break;
            case NEQ:
                os << " ≠ "; break;
        }
        if (!predicate.varRHS.empty())
            os << predicate.labelRHS << '.' << predicate.varRHS;
        else if (std::holds_alternative<std::string>(predicate.value))
            os << std::quoted(std::get<std::string>(predicate.value).c_str());
        else
            os << std::get<double>(predicate.value);
    }
    if (!predicate.BiVariableConditions.empty()) {
        static std::string AND{" ∧ "};
        if (predicate.casusu != TTRUE) os << AND << '(';
        size_t i = 0, N = predicate.BiVariableConditions.size();
        for (const DataPredicate& elem : predicate.BiVariableConditions) {
            os << elem;
            if (i != (N-1)) {
                os << AND;
            }
            i++;
        }
        if (predicate.casusu != TTRUE) os << ')';
    }
    return os;
}

std::string prev_char(const std::string &val, size_t max_size) {
    static const char MIN_CHAR = static_cast<char>(std::numeric_limits<unsigned char>::min()+1);
    static const char MAX_CHAR = static_cast<char>(std::numeric_limits<unsigned char>::max());
    if (val.empty()) return val;

    std::string result = val;
    size_t idx = val.size()-1;
    char last_char = val.at(idx);
    if (last_char == MIN_CHAR) {
        result.pop_back();
    } else {
        result[val.size()-1] = last_char-1;
        result += std::string(max_size - result.size(), MAX_CHAR);
    }
    return result;
}

std::string next_char(const std::string &val, size_t max_size) {
    static const char MIN_CHAR = static_cast<char>(std::numeric_limits<unsigned char>::min()+1);
    static const char MAX_CHAR = static_cast<char>(std::numeric_limits<unsigned char>::max());
    if (val == DataPredicate::MAX_STRING) return val;

    std::string next = val;
    size_t idx = next.size();
    if (idx < max_size) {
        next += MIN_CHAR;
    } else {
        idx--;
        if (next.at(idx) != MAX_CHAR) {
            next[idx]++;
            return next;
        } else {
            while (!next.empty()) {
                if (next.at(idx) != MAX_CHAR) {
                    next[idx]++;
                    return next;
                } else {
                    next.pop_back();
                    idx--;
                }
            }
            if (next.empty())
                return DataPredicate::MAX_STRING;
        }

    }
    return next;
}


#include <cassert>
bool DataPredicate::intersect_with(const DataPredicate& predicate) {
    if (predicate.casusu == TTRUE)
        return true;
    if ((casusu == TTRUE) && (!predicate.isBiVariableCondition())) {
        *this = predicate;
        return true;
    }
    DEBUG_ASSERT(var == predicate.var && label == predicate.label);
    if (predicate.isBiVariableCondition()) {
        BiVariableConditions.emplace_back(predicate);
        return true;
    } else {
        if (casusu == predicate.casusu) {
            switch (casusu) {
                case LT:
                case LEQ:
                    value = std::min(value, predicate.value);
                    break;

                case GT:
                case GEQ:
                    value = std::max(value, predicate.value);
                    break;

                case EQ:
                    // The intersection of equality should consider that the equivalence is among identical values
                    DEBUG_ASSERT(value == predicate.value);
                    break;

                case NEQ:
                    casusu = INTERVAL;
                    exceptions.insert(value);
                    exceptions.insert(predicate.value);
                    break;

                case INTERVAL:// (1)
                    value = std::max(value, predicate.value);
                    value_upper_bound = std::min(value_upper_bound, predicate.value_upper_bound);
                    if (value > value_upper_bound) {
                        return false;
                    }

                    std::set<union_minimal> S;
                    for (const auto& x : exceptions) {
                        if ((value <= x) && (x <= value_upper_bound))
                            S.insert(x);
                    }
                    for (const auto& x : predicate.exceptions) {
                        if ((value <= x) && (x <= value_upper_bound))
                            S.insert(x);
                    }
                    exceptions = S;
                    break;
            }
            return true;
        } else {
            asInterval();
            DataPredicate rightCopy = predicate;
            rightCopy.asInterval();
            return intersect_with(rightCopy); // (1)
        }
    }
}

void DataPredicate::asInterval() {
    if ((casusu == INTERVAL) || (casusu == TTRUE) || (isBiVariableCondition())) return;

    bool isString = std::holds_alternative<std::string>(value);
    union_minimal prev, next;
    union_minimal min, max;
    if (isString)  {
        std::string s = std::get<std::string>(value);
        min = MIN_STRING;
        max = MAX_STRING;
        prev = PREV_STRING(s);
        next = NEXT_STRING(s);
    } else {
        double x_val = std::get<double>(value);
        min = MIN_DOUBLE;
        max = MAX_DOUBLE;
        prev = PREV_DOUBLE(x_val);
        next = NEXT_DOUBLE(x_val);
    }

    switch (casusu) {
        case LT:
            value = min;
            value_upper_bound = prev;
            break;
        case GT:
            value = next;
            value_upper_bound = max;
            break;
        case LEQ:
            value_upper_bound = value;
            value = min;
            break;
        case GEQ:
            value_upper_bound = max;
            break;
        case EQ:
            value_upper_bound = value;
            break;
        case NEQ:
            exceptions.insert(value);
            value = min;
            value_upper_bound = max;
            break;
    }

    casusu = INTERVAL;
}

bool DataPredicate::testOverSingleVariable(const std::string &val) const {
    DEBUG_ASSERT(!isBiVariableCondition());
    bool isString = std::holds_alternative<std::string>(value);
    if (!isString) return false;
    std::string current = std::get<std::string>(value);
    switch (casusu) {
        case LT:
            return val < current;
        case GT:
            return val > current;
        case LEQ:
            return val <= current;
        case GEQ:
            return val >= current;
        case EQ:
            return val == current;
        case NEQ:
            return val != current;
        case INTERVAL: {
            std::variant<std::string,double> curr{current};
            if (exceptions.contains(curr)) return false;
            return ((current <= val)
                    && (val <= std::get<std::string>(value_upper_bound)));
        }
    }
}

bool DataPredicate::testOverSingleVariable(double val) const {
    DEBUG_ASSERT(!isBiVariableCondition());
    bool isDouble = std::holds_alternative<double>(value);
    if (!isDouble) return false;
    double current = std::get<double>(value);
    switch (casusu) {
        case LT:
            return val < current;
        case GT:
            return val > current;
        case LEQ:
            return val <= current;
        case GEQ:
            return val >= current;
        case EQ:
            return val == current;
        case NEQ:
            return val != current;
        case INTERVAL: {
            std::variant<std::string,double> curr{current};
            if (exceptions.contains(curr)) return false;
            return ((current <= val)
                    && (val <= std::get<double>(value_upper_bound)));
        }
    }
}

bool DataPredicate::operator==(const DataPredicate &rhs) const {
    bool tmp= label == rhs.label &&
           var == rhs.var &&
           casusu == rhs.casusu &&
           value == rhs.value &&
           value_upper_bound == rhs.value_upper_bound &&
           exceptions == rhs.exceptions &&
           BiVariableConditions == rhs.BiVariableConditions;
    return tmp;
}

bool DataPredicate::operator!=(const DataPredicate &rhs) const {
    return !(rhs == *this);
}

union_minimal DataPredicate::prev_of(const union_minimal &x) {
    if (std::holds_alternative<std::string>(x))
        return {PREV_STRING(std::get<std::string>(x))};
    else
        return {PREV_DOUBLE(std::get<double>(x))};
}

union_minimal DataPredicate::next_of(const union_minimal &x) {
    if (std::holds_alternative<std::string>(x))
        return {NEXT_STRING(std::get<std::string>(x))};
    else
        return {NEXT_DOUBLE(std::get<double>(x))};
}

#include <cassert>

bool DataPredicate::isStringPredicate() const {
    if (casusu == TTRUE)
        return false;
    bool isString = std::holds_alternative<std::string>(value);
    if (casusu == INTERVAL)
        DEBUG_ASSERT(isString == std::holds_alternative<std::string>(value_upper_bound));
    return isString;
}

bool DataPredicate::isDoublePredicate() const {
    if (casusu == TTRUE)
        return false;
    bool isString = std::holds_alternative<double>(value);
    if (casusu == INTERVAL)
        DEBUG_ASSERT(isString == std::holds_alternative<double>(value_upper_bound));
    return isString;
}


std::variant<std::vector<std::pair<std::string, std::string>>,
        std::vector<std::pair<double, double>>> DataPredicate::decompose_single_variable_into_intervals() const {
    DEBUG_ASSERT(!isBiVariableCondition());
    bool isString = isStringPredicate();
    std::variant<std::vector<std::pair<std::string, std::string>>,
            std::vector<std::pair<double, double>>> result;
    if (isString) {
        result = std::vector<std::pair<std::string, std::string>>{};
    } else {
        result = std::vector<std::pair<double, double>>{};
    }

    union_minimal prev, next;
    union_minimal min, max;
    if (isString)  {
        std::string s = std::get<std::string>(value);
        min = MIN_STRING;
        max = MAX_STRING;
        prev = PREV_STRING(s);
        next = NEXT_STRING(s);
    } else {
        double x_val = std::get<double>(value);
        min = MIN_DOUBLE;
        max = MAX_DOUBLE;
        prev = PREV_DOUBLE(x_val);
        next = NEXT_DOUBLE(x_val);
    }

    switch (casusu) {
        case LT:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(prev));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(prev));
            }
            return result;

        case GT:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(next), std::get<0>(max));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(next), std::get<1>(max));
            }
            return result;

        case LEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(value));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(value));
            }
            return result;

        case GEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(max));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(max));
            }
            return result;

        case EQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(value));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(value));
            }
            return result;

        case NEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(prev));
                std::get<0>(result).emplace_back(std::get<0>(next), std::get<0>(max));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(prev));
                std::get<1>(result).emplace_back(std::get<1>(next), std::get<1>(max));
            }
            return result;

        case INTERVAL:
            if (exceptions.empty()) {
                if (isString) {
                    std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(value_upper_bound));
                } else {
                    std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(value_upper_bound));
                }
            } else {
                size_t i = 0, N = exceptions.size();
                union_minimal prev = value;
                for (const auto& val : exceptions) {
                    if (i == 0) {
                        if (isString)
                            std::get<0>(result).emplace_back(std::get<0>(value), PREV_STRING(std::get<0>(val)));
                        else
                            std::get<1>(result).emplace_back(std::get<1>(value), PREV_DOUBLE(std::get<1>(val)));
                    } /*else if (i == (N-1)) {

                    }*/ else {
                        if (isString)
                            std::get<0>(result).emplace_back(PREV_STRING(std::get<0>(prev)), PREV_STRING(std::get<0>(val)));
                        else
                            std::get<1>(result).emplace_back(PREV_DOUBLE(std::get<1>(prev)), PREV_DOUBLE(std::get<1>(val)));
                    }
                    prev = val;
                    i++;
                }
                if (isString)
                    std::get<0>(result).emplace_back(PREV_STRING(std::get<0>(prev)), (std::get<0>(value_upper_bound)));
                else
                    std::get<1>(result).emplace_back(PREV_DOUBLE(std::get<1>(prev)), (std::get<1>(value_upper_bound)));
            }
            return result;
    }
}

DataPredicate::DataPredicate(const std::string &label, const std::string &var, double lb, double ub) : label{label}, wasReversed{false} , var{var}, casusu{INTERVAL} {
    value = lb;
    value_upper_bound = ub;
}

DataPredicate::DataPredicate(const std::string &label, const std::string &var, const std::string &lb,
                             const std::string &ub) : label{label}, wasReversed{false} , var{var}, casusu{INTERVAL} {
    value = lb;
    value_upper_bound = ub;
}

std::variant<std::vector<std::pair<std::string, std::string>>,
        std::vector<std::pair<double, double>>> DataPredicate::decompose_single_variable_into_intervals_with_missing() const {
    DEBUG_ASSERT(!isBiVariableCondition());
    bool isString = isStringPredicate();
    std::variant<std::vector<std::pair<std::string, std::string>>,
            std::vector<std::pair<double, double>>> result;
    if (isString) {
        result = std::vector<std::pair<std::string, std::string>>{};
    } else {
        result = std::vector<std::pair<double, double>>{};
    }

    union_minimal prev, next;
    union_minimal min, max;
    if (isString)  {
        std::string s = std::get<std::string>(value);
        min = MIN_STRING;
        max = MAX_STRING;
        prev = PREV_STRING(s);
        next = NEXT_STRING(s);
    } else {
        double x_val = std::get<double>(value);
        min = MIN_DOUBLE;
        max = MAX_DOUBLE;
        prev = PREV_DOUBLE(x_val);
        next = NEXT_DOUBLE(x_val);
    }

    switch (casusu) {
        case LT:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(prev));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(prev));
            }
            return result;

        case GT:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(next), std::get<0>(max));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(next), std::get<1>(max));
            }
            return result;

        case LEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(value));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(value));
            }
            return result;

        case GEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(max));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(max));
            }
            return result;

        case EQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(prev));
                std::get<0>(result).emplace_back(std::get<0>(next), std::get<0>(max));
                std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(value));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(prev));
                std::get<1>(result).emplace_back(std::get<1>(next), std::get<1>(max));
                std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(value));
            }
            return result;

        case NEQ:
            if (isString) {
                std::get<0>(result).emplace_back(std::get<0>(min), std::get<0>(prev));
                std::get<0>(result).emplace_back(std::get<0>(next), std::get<0>(max));
                std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(value));
            } else {
                std::get<1>(result).emplace_back(std::get<1>(min), std::get<1>(prev));
                std::get<1>(result).emplace_back(std::get<1>(next), std::get<1>(max));
                std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(value));
            }
            return result;

        case INTERVAL:
            if (exceptions.empty()) {
                if (isString) {
                    std::get<0>(result).emplace_back(std::get<0>(value), std::get<0>(value_upper_bound));
                } else {
                    std::get<1>(result).emplace_back(std::get<1>(value), std::get<1>(value_upper_bound));
                }
            } else {
                size_t i = 0, N = exceptions.size();
                union_minimal prev = value;
                for (const auto& val : exceptions) {
                    if (i == 0) {
                        if (isString)
                            std::get<0>(result).emplace_back(std::get<0>(value), PREV_STRING(std::get<0>(val)));
                        else
                            std::get<1>(result).emplace_back(std::get<1>(value), PREV_DOUBLE(std::get<1>(val)));
                    } else if (i == (N-1)) {
                        if (isString)
                            std::get<0>(result).emplace_back(PREV_STRING(std::get<0>(prev)), (std::get<0>(value_upper_bound)));
                        else
                            std::get<1>(result).emplace_back(PREV_DOUBLE(std::get<1>(prev)), (std::get<1>(value_upper_bound)));
                    } else {
                        if (isString)
                            std::get<0>(result).emplace_back(PREV_STRING(std::get<0>(prev)), PREV_STRING(std::get<0>(val)));
                        else
                            std::get<1>(result).emplace_back(PREV_DOUBLE(std::get<1>(prev)), PREV_DOUBLE(std::get<1>(val)));
                    }
                    prev = val;
                    i++;
                }
            }
            return result;
    }
}

bool DataPredicate::isBiVariableCondition() const {
    return !varRHS.empty();
}

DataPredicate::DataPredicate(const std::string &label, const std::string &var, union_minimal lb, union_minimal ub) : label{label}, wasReversed{false} , var{var}, casusu{INTERVAL} {
    value = lb;
    value_upper_bound = ub;
}

DataPredicate DataPredicate::instantiateRHSWith(const union_minimal& val) const {
    // This RHS instantiation is not applicable to intervals, which have no RHS variable
    DEBUG_ASSERT(casusu != INTERVAL);
    // In order to run this method, I shall have the rhs variable
    DEBUG_ASSERT(!varRHS.empty());
    DataPredicate cpy = *this;
    cpy.varRHS.clear();
    cpy.labelRHS.clear();
    cpy.value = val;
    return cpy;
}

DataPredicate DataPredicate::reverseBiVariablePredicate() const {
    DEBUG_ASSERT(casusu != INTERVAL);
    DEBUG_ASSERT(!varRHS.empty());
    DataPredicate cpy = *this;
    std::swap(cpy.var, cpy.varRHS);
    std::swap(cpy.label, cpy.labelRHS);
    cpy.casusu = invert_predicate_direction(cpy.casusu);
    cpy.wasReversed = !wasReversed;
    return cpy;
}
