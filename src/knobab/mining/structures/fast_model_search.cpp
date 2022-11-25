//
// Created by giacomo on 25/11/22.
//

#include "knobab/mining/structures/fast_model_search.h"


size_t fast_model_search::emplace_back(const pattern_mining_result<DeclareDataAware>& clause) {
    clause_name name = clause.clause.casusu;
    activation_label activation = clause.clause.left_act;
    if (clause.clause.right_act.empty()) {
        // Unary Clause
        unary_clauses.unary_activation_dataless[name][activation][clause.clause.n].emplace_back(id);
        unary_clauses.unary_activation_withdata[name][activation][clause.clause.left_decomposed_atoms][clause.clause.n].emplace_back(id);
    } else {
        activation_label target = clause.clause.right_act;
        binary_clauses.activation_dataless[name][activation].emplace_back(id);
        binary_clauses.activation_withdata[name][activation][clause.clause.left_decomposed_atoms].emplace_back(id);
        binary_clauses.dataless_binary_clause[name][activation][target].emplace_back(id);
        binary_clauses.no_targ_data[name][activation][clause.clause.left_decomposed_atoms][target].emplace_back(id);
        binary_clauses.by_targetlabel[name][target].emplace_back(id);
        binary_clauses.targ_data[name][activation][clause.clause.left_decomposed_atoms][target][clause.clause.right_decomposed_atoms].emplace_back(id);
        binary_clauses.by_targetlabel_and_atomisation[name][target][clause.clause.right_decomposed_atoms].emplace_back(id);
    }
    return ++id;
}

std::vector<size_t>  fast_model_search::exists(const DeclareDataAware& query,
                                               QueryRelation num ,
                                               QueryRelation left ,
                                               QueryRelation right ) const {
    std::vector<size_t> current;
    if (query.right_act.empty()) {
        // Unary Clause
        if (query.left_decomposed_atoms.empty()) {
            DEBUG_ASSERT((!query.casusu.empty()) && (!query.left_act.empty()));
            auto it1 = unary_clauses.unary_activation_dataless.find(query.casusu);//////
            if (it1 == unary_clauses.unary_activation_dataless.end()) return current;
            auto it2 = it1->second.find(query.left_act);
            if (it2 == it1->second.end()) return current;
            auto cp = iterator_resolve(it2->second.begin(), it2->second.end(), it2->second.find(query.n), num);
            for (auto it = cp.first; it!=cp.second; it++) {
                if (int_compare(query.n, it->first, num))
                    current.insert(current.end(), it->second.begin(), it->second.end());
            }
            remove_duplicates(current);
            return current;
        } else {
            DEBUG_ASSERT((!query.casusu.empty()) && (!query.left_act.empty()) && (!query.left_decomposed_atoms.empty()));
            auto it1 = unary_clauses.unary_activation_withdata.find(query.casusu);///////
            if (it1 == unary_clauses.unary_activation_withdata.end()) return current;
            auto it2 = it1->second.find(query.left_act);
            if (it2 == it1->second.end()) return current;
            auto cp1 = iterator_resolve(it2->second.begin(), it2->second.end(), it2->second.find(query.left_decomposed_atoms), left);
            for (auto it = cp1.first; it!=cp1.second; it++) {
                if (!set_compare(query.left_decomposed_atoms, it->first, left)) continue;
                auto cp = iterator_resolve(it->second.begin(), it->second.end(), it->second.find(query.n), num);
                for (auto it_ = cp.first; it_!=cp.second; it_++) {
                    if (!int_compare(query.n, it_->first, left)) continue;
                    current.insert(current.end(), it_->second.begin(), it_->second.end());
                }
            }
            remove_duplicates(current);
            return current;
        }
    } else {
        // Binary Clause
        if (query.left_act.empty()) {
            if (query.right_decomposed_atoms.empty()) {
                DEBUG_ASSERT((!query.casusu.empty()) && (!query.right_act.empty()));
                auto it1 = binary_clauses.by_targetlabel.find(query.casusu);////////
                if (it1 == binary_clauses.by_targetlabel.end()) return current;
                auto it2 = it1->second.find(query.right_act);
                if (it2 == it1->second.end()) return current;
                else return it2->second;
            } else {
                DEBUG_ASSERT((!query.casusu.empty()) && (!query.right_act.empty()) && (!query.right_decomposed_atoms.empty()));
                auto it1 = binary_clauses.by_targetlabel_and_atomisation.find(query.casusu);/////
                if (it1 == binary_clauses.by_targetlabel_and_atomisation.end()) return current;
                auto it2 = it1->second.find(query.right_act);
                if (it2 == it1->second.end()) return current;
                auto cp1 = iterator_resolve(it2->second.begin(), it2->second.end(), it2->second.find(query.right_decomposed_atoms), right);
                for (auto it = cp1.first; it!=cp1.second; it++) {
                    if (!set_compare(query.right_decomposed_atoms, it->first, right)) continue;
                    current.insert(current.end(), it->second.begin(), it->second.end());
                }
                remove_duplicates(current);
                return current;
            }
        } else {
            // Left not empty

            if (query.right_act.empty()) {
                // query.right_decomposed_atoms ignored
                if (query.right_decomposed_atoms.empty()) {
                    DEBUG_ASSERT((!query.casusu.empty()) && (!query.left_act.empty()));
                    auto it1 = binary_clauses.activation_dataless.find(query.casusu);/////
                    if (it1 == binary_clauses.activation_dataless.end()) return current;
                    auto it2 = it1->second.find(query.left_act);
                    if (it2 == it1->second.end()) return current;
                    else return it2->second;
                } else {
                    DEBUG_ASSERT((!query.casusu.empty()) && (!query.right_act.empty()) && (!query.right_decomposed_atoms.empty()));
                    auto it1 = binary_clauses.activation_withdata.find(query.casusu);/////
                    if (it1 == binary_clauses.activation_withdata.end()) return current;
                    auto it2 = it1->second.find(query.left_act);
                    if (it2 == it1->second.end()) return current;
                    auto cp1 = iterator_resolve(it2->second.begin(), it2->second.end(), it2->second.find(query.left_decomposed_atoms), left);
                    for (auto it = cp1.first; it!=cp1.second; it++) {
                        if (!set_compare(query.left_decomposed_atoms, it->first, left)) continue;
                        current.insert(current.end(), it->second.begin(), it->second.end());
                    }
                    remove_duplicates(current);
                    return current;
                }
            } else {
                // Right not totally empty
                if (query.left_decomposed_atoms.empty()) {
                    if (query.right_decomposed_atoms.empty()) {
                        auto it1 = binary_clauses.dataless_binary_clause.find(query.casusu);/////
                        if (it1 == binary_clauses.dataless_binary_clause.end()) return current;
                        auto it2 = it1->second.find(query.left_act);
                        if (it2 == it1->second.end()) return current;
                        auto it3 = it2->second.find(query.right_act);
                        if (it3 == it2->second.end()) return current;
                        else return it3->second;
                    } else {
                        auto it1 = binary_clauses.dataless_binary_clausev2.find(query.casusu);
                        if (it1 == binary_clauses.dataless_binary_clausev2.end()) return current;
                        auto it2 = it1->second.find(query.left_act);
                        if (it2 == it1->second.end()) return current;
                        auto it3 = it2->second.find(query.right_act);
                        if (it3 == it2->second.end()) return current;
                        auto cp1 = iterator_resolve(it3->second.begin(), it3->second.end(), it3->second.find(query.right_decomposed_atoms), right);
                        for (auto it = cp1.first; it!=cp1.second; it++) {
                            if (!set_compare(query.right_decomposed_atoms, it->first, right)) continue;
                            current.insert(current.end(), it->second.begin(), it->second.end());
                        }
                        remove_duplicates(current);
                        return current;
                    }
                } else {
                    // With queryRight not empty
                    auto it1 = binary_clauses.targ_data.find(query.casusu);
                    if (it1 == binary_clauses.targ_data.end()) return current;
                    auto it2 = it1->second.find(query.left_act);
                    if (it2 == it1->second.end()) return current;
                    auto cp1 = iterator_resolve(it2->second.begin(), it2->second.end(), it2->second.find(query.left_decomposed_atoms), left);
                    for (auto it3 = cp1.first; it3!=cp1.second; it3++) {
                        if (!set_compare(query.left_decomposed_atoms, it3->first, left)) continue;
                        auto it4 = it3->second.find(query.right_act);
                        if (it4 != it3->second.end()) {
                            auto cp2 = iterator_resolve(it4->second.begin(), it4->second.end(), it4->second.find(query.right_decomposed_atoms), right);
                            for (auto it = cp2.first; it!=cp2.second; it++) {
                                if (!set_compare(query.right_decomposed_atoms, it->first, right)) continue;
                                current.insert(current.end(), it->second.begin(), it->second.end());
                            }
                        }
                    }
                    remove_duplicates(current);
                    return current;
                }
            }
        }
    }
}