/*
 * DeclareDataAware.cpp
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

#include <ios>
#include <magic_enum.hpp>
#include <yaucl/data/xml.h>
#include <yaucl/functional/assert.h>
#include <knobab/server/declare/DeclareDataAware.h>

inline void print_conj(std::ostream &os, const std::unordered_map<std::string, DataPredicate> &map) {
    static std::string AND{" ∧ "};
    size_t i = 0, N = map.size();
    for (const std::pair<std::string, DataPredicate>& elem : map) {
        os << elem.second;
        if (i != (N-1)) {
            os << AND;
        }
        i++;
    }
}

inline void print_dnf(std::ostream &os, const std::vector<std::unordered_map<std::string, DataPredicate>> &map) {
    static std::string OR{" ∨ "};
    for (size_t i = 0, N = map.size(); i<N; i++) {
        const std::unordered_map<std::string, DataPredicate>& elem = map.at(i);
        if (N>1) os << '(';
        print_conj(os, elem);
        if (N>1) os << ')';
        if (i != (N-1)) {
            os << OR;
        }
    }
}




std::ostream &operator<<(std::ostream &os, const DeclareDataAware &aware) {
    os << /*magic_enum::enum_name*/(aware.casusu);
    os << '(' << aware.left_act << ", ";
    if (aware.dnf_left_map.empty())
        os << "true";
    else
        print_dnf(os, aware.dnf_left_map);
    os << ", ";
    if (!aware.right_act.empty()) {
        os << aware.right_act << ", ";
        if (aware.dnf_right_map.empty())
            os << "true";
        else
            print_dnf(os, aware.dnf_right_map);
    } else {
        os << aware.n;
    }
    os << " )";
    if (!aware.conjunctive_map.empty()) {
        os << " where ";
        print_dnf(os, aware.conjunctive_map);
    }

    if (!aware.left_decomposed_atoms.empty()) {
        os << " {leftAtoms = ";
        for (auto it = aware.left_decomposed_atoms.begin(), en = aware.left_decomposed_atoms.end(); it != en; ) {
            os << *it;
            it++;
            if (it != en) os << ", ";
        }
        os << " }";
    }
    if (!aware.right_decomposed_atoms.empty()) {
        os << " {rightAtoms = ";
        for (auto it = aware.right_decomposed_atoms.begin(), en = aware.right_decomposed_atoms.end(); it != en; ) {
            os << *it;
            it++;
            if (it != en) os << ", ";
        }
        os << " }";
    }

    return os;
}

bool isUnaryPredicate(const declare_templates& type) {
    return type == "Existence" || type == "Absence" || type == "End" || type == "Init";
}

bool isPredicateNegative(const declare_templates& type) {
    return type == "NotCoExistence" || type == "NegSuccession" || type == "NegChainSuccession" || type == "Absence" || type == "ExlChoice" || type == "AltPrecedence" || type == "AltResponse" || type == "AltSuccession";
}

DeclareDataAware DeclareDataAware::doExistence(size_t n, const std::string &left_act,
                                               const std::vector<std::unordered_map<std::string, DataPredicate>> &dnf_left_map) {
    DeclareDataAware sol;
    sol.n = n;
    sol.casusu = "Existence";
    sol.left_act = left_act;
    sol.dnf_left_map = dnf_left_map;
    return sol;
}

DeclareDataAware DeclareDataAware::doAbsence(size_t n, const std::string &left_act,
                                             const std::vector<std::unordered_map<std::string, DataPredicate>> &dnf_left_map) {
    DeclareDataAware sol;
    sol.n = n;
    sol.casusu = "Absence";
    sol.left_act = left_act;
    sol.dnf_left_map = dnf_left_map;
    return sol;
}

DeclareDataAware DeclareDataAware::parse_declare_non_data_string(const std::string &line) {
    DeclareDataAware pattern;
    std::string nextLine;

    auto pos = line.find('[');
    DEBUG_ASSERT(pos != std::string::npos);
    std::string pattern_name = line.substr(0, pos);
    pattern.casusu = pattern_name;//magic_enum::enum_cast<declare_templates>(pattern_name).value();

    pattern_name = line.substr(pos+1);
    pos = pattern_name.find(',');
    DEBUG_ASSERT(pos != std::string::npos);
    pattern.left_act = pattern_name.substr(0, pos);
    STRIP_ALL_SPACES(pattern.left_act);
    TO_LOWER(pattern.left_act);

    pattern_name = pattern_name.substr(pos+1);
    pos = pattern_name.find(']');
    DEBUG_ASSERT(pos != std::string::npos);
    std::string second_or_number = pattern_name.substr(0, pos);
    STRIP_ALL_SPACES(second_or_number);

    char* p;
    unsigned long converted = strtoul(second_or_number.c_str(), &p, 10);
    if (*p) {
        pattern.right_act = second_or_number;
        STRIP_ALL_SPACES(pattern.right_act);
        TO_LOWER(pattern.right_act);
        pattern.n = 1;
    } else {
        pattern.n = converted;
    }

    return pattern;
}

#include <fstream>

std::vector<DeclareDataAware> DeclareDataAware::load_simplified_declare_model(std::istream &file) {
    std::string line;
    std::vector<DeclareDataAware> V;
    while (std::getline(file, line)) {
        V.emplace_back(DeclareDataAware::parse_declare_non_data_string(line));
    }
    return V;
}

bool DeclareDataAware::operator==(const DeclareDataAware &rhs) const {
    return casusu == rhs.casusu &&
           n == rhs.n &&
           left_act == rhs.left_act &&
           right_act == rhs.right_act;
}

bool DeclareDataAware::operator!=(const DeclareDataAware &rhs) const {
    return !(rhs == *this);
}

DeclareDataAware DeclareDataAware::unary(const declare_templates& t, const std::string &argument, size_t n = 1) {
    DEBUG_ASSERT(isUnaryPredicate(t));
    DeclareDataAware result;
    result.casusu = t;
    result.left_act = argument;
    result.n = n;
    return result;
}

DeclareDataAware DeclareDataAware::binary(const declare_templates& t, const std::string &left, const std::string right) {
    DEBUG_ASSERT(!isUnaryPredicate(t));
    DeclareDataAware result;
    result.casusu = t;
    result.left_act = left;
    result.right_act = right;
    result.n = 2;
    return result;
}

DeclareDataAware DeclareDataAware::binary_for_testing(const declare_templates& t, const std::string &left, const std::string& right) {
    auto result = binary(t, left, right);
    result.left_decomposed_atoms.insert(left);
    result.right_decomposed_atoms.insert(right);
    return result;
}

#include <knobab/server/tables/KnowledgeBase.h>

bool DeclareDataAware::checkValidity(const env &e1, const env &e2) const {
    if (conjunctive_map.empty()) return true;
    for(const auto& pred_withConj : conjunctive_map){
        bool result = true;
        for (const auto& predDummy : pred_withConj) {
            for (const auto& pred : predDummy.second.BiVariableConditions) {
                if(!test_decomposed_data_predicate(e1, e2, pred.var, pred.varRHS, pred.casusu)){
                    result = false;
                    break;
                }
            }
        }
        if (result) return true;
    }
    return false;
}

bool DeclareDataAware::checkValidity(uint32_t t1, uint16_t e1, const env &e2) const {
    if (conjunctive_map.empty()) return true;
    for(const auto& pred_withConj : conjunctive_map){
        bool result = true;
        for (const auto & predDummy : pred_withConj) {
            for (const auto& pred : predDummy.second.BiVariableConditions) {
                bool test = false;
                auto temp1 = e2.find(pred.varRHS);
                if (temp1 == e2.end())
                    test = false;
                else {
                    auto temp2_a = kb->attribute_name_to_table.find(pred.var);
                    if (temp2_a != kb->attribute_name_to_table.end()) {
                        size_t offset = kb->act_table_by_act_id.getBuilder().trace_id_to_event_id_to_offset.at(t1).at(e1);
                        std::optional<union_minimal> data = temp2_a->second.resolve_record_if_exists2(offset);
                        if (data.has_value()) {
                            auto lhs = data.value();
                            switch (pred.casusu) {
                                case LT:
                                    test = lhs < temp1->second ; break;
                                case LEQ:
                                    test = lhs <= temp1->second; break;
                                case GT:
                                    test = lhs > temp1->second; break;
                                case GEQ:
                                    test =  lhs >= temp1->second; break;
                                case EQ:
                                    test =  lhs == temp1->second; break;
                                case NEQ:
                                    test =  lhs != temp1->second; break;
                                case TTRUE:
                                    test =  true; break;
                                default:
                                    test =  false; break;
                            }
                        } else
                            test = false;
                    } else {
                        test = false;
                    }
                }
                if(!test){
                    result = false;
                    break;
                }
            }
        }
        if (result) return true;
    }
    return false;
}

bool DeclareDataAware::checkValidity(const env &e1, uint32_t t2, uint16_t e2) const {
    if (conjunctive_map.empty()) return true;
    for(const auto& pred_withConj : conjunctive_map){
        bool result = true;
        for (const auto& predDummy : pred_withConj) {
            for (const auto& pred : predDummy.second.BiVariableConditions) {
                bool test = false;
                auto temp1 = e1.find(pred.var);
                if (temp1 == e1.end())
                    test = false;
                else {
                    auto temp2_a = kb->attribute_name_to_table.find(pred.varRHS);
                    if (temp2_a != kb->attribute_name_to_table.end()) {
                        size_t offset = kb->act_table_by_act_id.getBuilder().trace_id_to_event_id_to_offset.at(t2).at(e2);
                        std::optional<union_minimal> data = temp2_a->second.resolve_record_if_exists2(offset);
                        if (data.has_value()) {
                            auto rhs = data.value();
                            switch (pred.casusu) {
                                case LT:
                                    test = temp1->second < rhs; break;
                                case LEQ:
                                    test =  temp1->second <= rhs; break;
                                case GT:
                                    test =  temp1->second > rhs; break;
                                case GEQ:
                                    test =  temp1->second >= rhs; break;
                                case EQ:
                                    test =  temp1->second == rhs; break;
                                case NEQ:
                                    test =  temp1->second != rhs; break;
                                case TTRUE:
                                    test =  true; break;
                                default:
                                    test =  false; break;
                            }
                        } else
                            test = false;
                    } else {
                        test = false;
                    }
                }
                if(!test){
                    result = false;
                    break;
                }
            }
        }
        if (result) return true;
    }
    return false;
}

env DeclareDataAware::GetPayloadDataFromEvent(const std::pair<uint32_t, uint16_t> &pair) const {
    env environment;

    for(const auto& p : kb->attribute_name_to_table){
        size_t offset = kb->act_table_by_act_id.getBuilder().trace_id_to_event_id_to_offset.at(pair.first).at(pair.second);
        std::optional<union_minimal> data = p.second.resolve_record_if_exists2(offset);
        if(data.has_value()) {
            environment[p.first] = data.value();
        }
    }

    return environment;
}

env DeclareDataAware::GetPayloadDataFromEvent(uint32_t first, uint16_t second, bool isLeft, std::unordered_set<std::string>& cache) const {
    env environment;

    if (cache.empty()) {
        for(const auto& pred_withConj : conjunctive_map){
            for (const auto & predListing : pred_withConj) {
                for (const auto& pred : predListing.second.BiVariableConditions) {
                    cache.insert(isLeft ? pred.var : pred.varRHS);
                }
            }
        }
    }

    for (const auto& x : cache) {
        auto it = kb->attribute_name_to_table.find(x);
        if (it != kb->attribute_name_to_table.end()) {
            size_t offset = kb->act_table_by_act_id.getBuilder().trace_id_to_event_id_to_offset.at(first).at(second);
            std::optional<union_minimal> data = it->second.resolve_record_if_exists2(offset);
            if(data.has_value()) {
                environment[x] = data.value();
            }
        }
    }

    return environment;
}

DeclareDataAware::DeclareDataAware(const std::vector<std::vector<DataPredicate>> &predicate, const KnowledgeBase *kb) : kb{kb} {
    for (const auto& ref : predicate) {
        auto& res = conjunctive_map.emplace_back();
        for (const auto& x : ref) {
            res[x.var].BiVariableConditions.emplace_back(x);

        }
    }
}

DeclareDataAware DeclareDataAware::flip() const {
    //Flipping only the relevant part
    DeclareDataAware result = *this;
    result.conjunctive_map.clear();
    for (const auto& ref : conjunctive_map) {
        auto inner = result.conjunctive_map.emplace_back();
        for (const auto& ref2 : ref) {
            for (const auto& pred : ref2.second.BiVariableConditions) {
                auto cpy = ref2.second.flip();
                inner.emplace(cpy.var, cpy);
            }
        }
    }
    result.kb = this->kb;
    return result;
}
