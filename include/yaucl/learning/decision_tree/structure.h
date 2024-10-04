/*
 * structure.h
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
// Created by giacomo on 31/07/23.
//

#ifndef KNOBAB_SERVER_STRUCTURE_H
#define KNOBAB_SERVER_STRUCTURE_H


#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <limits>
#include <iostream>

//namespace std {
//    template <typename T, typename K>
//    struct hash<std::pair<T, K>> {
//        std::hash<T> hT;
//        std::hash<K> hK;
//        std::size_t operator()(const std::pair<T, K>& k) const {
//            return hT(k.first) ^ hK(k.second);
//        }
//    };
//
//}

#include <yaucl/hashing/pair_hash.h>
#include <vector>
#include <array>
#include <string>
#include <variant>
#include <functional>
#include <span>

#include "dt_predicate.h"
#include "ForTheWin.h"

template <typename T>
std::pair<dt_predicate,double> evaluate_leq_predicate(typename std::vector<std::pair<T,int>>::iterator& begin,
                                                      typename std::vector<std::pair<T,int>>::iterator& end,
                                                      const std::function<union_minimal(const T&)>& F,
                                                      ForTheWin& forthegain,
                                                      int max_class,
                                                      ForTheWin::gain_measures measure) {
    std::sort(begin, end, [&F](const std::pair<T,int>& x, const std::pair<T,int>& y) {
        return F(x.first)<F(y.first) || (F(x.first)==F(y.first)&& (x.second<y.second));
    });
    std::vector<size_t> n(max_class+1, 0);
    std::unordered_set<union_minimal> M;
    std::unordered_map<std::pair<union_minimal,int>, size_t> N;
    std::pair<union_minimal,int> val;
    for (auto it = begin; it < end; it++) {
        n[it->second]++;
        if ((it+1)==end) break;
        auto current = F(it->first);
        auto next = F((it+1)->first);
        if (current!=next) {
            if (std::holds_alternative<double>(current) && std::holds_alternative<double>(next)) {
                val.first = (std::get<double>(current)+std::get<double>(next))/2.0;
            } else if (std::holds_alternative<std::string>(current) && std::holds_alternative<std::string>(next)) {
                std::string x = std::get<std::string>(current);
                if (x.empty()) {
                    val.first = std::string{static_cast<char>(static_cast<unsigned char>(std::get<std::string>(next)[0])/2)};
                } else {
                    unsigned char L = static_cast<unsigned char>(std::get<std::string>(next)[0]);
                    unsigned char R = static_cast<unsigned char>(std::get<std::string>(current)[0]);
                    std::string s;
                    s += (static_cast<char>((L+R)/2));
                    val.first = s;
                }
            }
            for (int i = 0; i<=max_class; i++) {
                val.second = i;
                N[val] = n.at(i);
            }
            M.insert(val.first);
        }
    }

    std::pair<dt_predicate, double> predicate_w_score;
    predicate_w_score.second = -std::numeric_limits<double>::max();
    predicate_w_score.first.pred = dt_predicate::LEQ_THAN;
    for (const auto& x : M) {
        val.first = x;
        double forthepos = 0.0, fortheneg = 0.0;
        for (int i = 0; i<=max_class; i++) {
            val.second = i;
            auto nv = N[val];
            forthepos += (double)nv;
            fortheneg += (double)(n.at(i)-nv);
        }
        forthegain.goodBad(forthepos, fortheneg);
        for (int i= 0; i<=max_class; i++) {
            val.second = i;
            forthegain.setP(i, (double)N[val], (double)(n.at(i)-N[val]));
        }
        double local_score = forthegain.getGain(measure);
        if (local_score > predicate_w_score.second) {
            predicate_w_score.second = local_score;
            predicate_w_score.first.value = x;
        }
    }
    return predicate_w_score;
}



#include <cmath>

//// Returns which bits are on in the integer a
//std::vector<int> getOnLocations(int a);

#include <yaucl/structures/set_operations.h>

template <typename T>
std::pair<dt_predicate,double> evaluate_inset_predicate(typename std::vector<std::pair<T,int>>::iterator& begin,
                                                        typename std::vector<std::pair<T,int>>::iterator& end,
                                                        const std::function<union_minimal(const T&)>& F,
                                                        ForTheWin& forthegain,
                                                        int max_class,
                                                        ForTheWin::gain_measures measure,
                                                        size_t l) {

    std::vector<size_t> n(max_class+1, 0);
    std::vector<union_minimal> M; // dom
    std::unordered_map<std::pair<union_minimal,int>, size_t> N;
    std::pair<union_minimal,int> val;
    for (auto it = begin; it != end; it++) M.emplace_back(F(it->first));
    std::sort( M.begin(), M.end() );
    M.erase( unique( M.begin(), M.end() ), M.end() );
    for (int i = 0; i<=max_class; i++) {
        val.second = i;
        for (const auto& ref : M) {
            val.first = ref;
            N[val] = 0;
        }
    }
    for (auto it = begin; it != end; it++) {
        val.first = F(it->first);
        val.second = it->second;
        N[val]++;
    }

    std::pair<dt_predicate, double> predicate_w_score;
    predicate_w_score.second = -std::numeric_limits<double>::max();
    predicate_w_score.first.pred = dt_predicate::IN_SET;
    double forthepos, fortheneg;
    double localPos, localNeg;
    for (const std::unordered_set<union_minimal>& V : powerSet(M, l)) {
        fortheneg = 0.0;
        forthepos = 0.0;
        std::unordered_set<union_minimal> VCompl;
        for (int i = 0; i<=max_class; i++) {
            val.second = i;
            for (const auto& item : M) {
                val.first = item;
                if (V.contains(item)) {
                    forthepos += N[val];
                } else {
                    VCompl.insert(item);
                    fortheneg += N[val];
                }
            }
        }
        forthegain.goodBad(forthepos, fortheneg);
        for (int i= 0; i<=max_class; i++) {
            localPos= 0.0; localNeg = 0.0;
            val.second = i;
            for (const auto& item :V) {
                val.first = item;
                localPos += N[val];
            }
            for (const auto& item: VCompl) {
                val.first = item;
                localNeg += N[val];
            }
            forthegain.setP(i, localPos, localNeg);
            double local_score = forthegain.getGain(measure);
            if (local_score > predicate_w_score.second) {
                predicate_w_score.second = local_score;
                predicate_w_score.first.categoric_set = V;
            }
        }
    }
    return predicate_w_score;
}

template <typename T>
std::pair<dt_predicate,double> evaluate_predicate(typename std::vector<std::pair<T,int>>::iterator& begin,
                                                  typename std::vector<std::pair<T,int>>::iterator& end,
                                                  const std::function<union_minimal(const T&)>& F,
                                                  ForTheWin& forthegain,
                                                  int max_class,
                                                  ForTheWin::gain_measures measure,
                                                  const dt_predicate& pred) {

    std::vector<size_t> n(max_class+1, 0);
    std::vector<union_minimal> M; // dom
    std::unordered_map<std::pair<union_minimal,int>, size_t> N;
    std::pair<union_minimal,int> val;
    for (auto it = begin; it != end; it++) M.emplace_back(F(it->first));
    std::sort( M.begin(), M.end() );
    M.erase( unique( M.begin(), M.end() ), M.end() );
    for (int i = 0; i<=max_class; i++) {
        val.second = i;
        for (const auto& ref : M) {
            val.first = ref;
            N[val] = 0;
        }
    }
    size_t total = 0;
    for (auto it = begin; it != end; it++) {
        val.first = F(it->first);
        val.second = it->second;
        total++;
        N[val]++;
    }

    std::pair<dt_predicate, double> predicate_w_score;
    predicate_w_score.second = -std::numeric_limits<double>::max();
    double forthepos, fortheneg;
    double localPos, localNeg;
    fortheneg = 0.0;
    forthepos = 0.0;
    for (int i = 0; i<=max_class; i++) {
        val.second = i;
        for (const auto& item : M) {
            val.first = item;
            if (pred(item))
                forthepos += N[val];
            else
                fortheneg += (total-N[val]);
        }
    }
    forthegain.goodBad(forthepos, fortheneg);
    for (int i= 0; i<=max_class; i++) {
        val.second = i;
        forthegain.setP(i, N[val], (total-N[val]));
        double local_score = forthegain.getGain(measure);
        if (local_score > predicate_w_score.second) {
            predicate_w_score.second = local_score;
            predicate_w_score.first = pred;
        }
    }
    return predicate_w_score;
}

template <typename T>
class DecisionTree {
    int         majority_class;
    double      majority_class_precision;
    std::function<union_minimal(const T&, const std::string&)> F;
    std::vector<DecisionTree<T>> children;
    bool isLeaf;
    bool is_root;
    std::span<std::pair<T,int>> leaf;
    double total_weights = std::numeric_limits<double>::min();
public:

    double goodness = std::numeric_limits<double>::min();
    std::pair<dt_predicate,double> candidate;
    bool operator()(const T& object) {
        auto tmp = F(object, candidate.first.field);
        return candidate.first(tmp);
    }
    const DecisionTree<T>* getSatisfyingConditionSplit() const {
        if (isLeaf) return nullptr;
        return &children.at(0);
    }
    const DecisionTree<T>* getUnsatisfyingConditionSplit() const {
        if (isLeaf) return nullptr;
        return &children.at(1);
    }
    bool isLeafNode() const { return isLeaf; }
    size_t getMajorityClass() const { return majority_class; }
    size_t getLeafNodeSize() const {
        if (!isLeaf) return 0;
        return leaf.size();
    }
    const std::span<std::pair<T,int>>& getDataNodes() const {
        return leaf;
    }

    void print_rec(std::ostream& os, size_t depth) const {
        os << std::string(2*depth, '.');
        if (isLeaf) {
            os << "p(y=" << majority_class << ")=" << majority_class_precision << std::endl;
        } else {
            os << candidate.first << "(y/n)" << std::endl;
            for (const auto& child : children)
                child.print_rec(os, depth+1);
        }

        if(is_root) {
            os << "GOODNESS: " << goodness << std::endl;
        }
    }

    void populate_children_predicates2(std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred) const {
        std::vector<dt_predicate> memo;
        populate_children_predicates2(decision_to_pred, memo);
    }

    void populate_children_predicates2(std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred,
                                       std::vector<dt_predicate>& current_stack) const {
        if (!isLeaf) {
            const auto sat_ptr = getSatisfyingConditionSplit();
            const auto viol_ptr = getUnsatisfyingConditionSplit();

            if (viol_ptr) {
                auto unsat_copy = current_stack;
                auto& cpy = unsat_copy.emplace_back(candidate.first);
                switch(cpy.pred){
                    case dt_predicate::LEQ_THAN:
                        cpy.pred = dt_predicate::G_THAN;
                        break;
                    case dt_predicate::IN_SET:
                        cpy.pred = dt_predicate::NOT_IN_SET;
                        break;
                    case dt_predicate::L_THAN:
                    case dt_predicate::G_THAN:
                    case dt_predicate::GEQ_THAN:
                    case dt_predicate::NOT_IN_SET:
                        /* We should never get here, assign copies only */
                        break;
                }
                viol_ptr->populate_children_predicates2(decision_to_pred, unsat_copy);
            }
            if (sat_ptr) {
                current_stack.emplace_back(candidate.first);
                sat_ptr->populate_children_predicates2(decision_to_pred, current_stack);
            }
        } else {
            auto it = decision_to_pred.find(majority_class);
            decision_to_pred[majority_class].emplace_back(majority_class_precision, current_stack);
        }
    }

    void populate_children_predicates(std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred,
                                      std::vector<dt_predicate>* current = nullptr,
                                      bool negate = false) const {
        populate_children_predicates2(decision_to_pred);
// @author: Samuel Appleby
// This implementation was bugged!
//        if(!isLeaf) {
//            const DecisionTree* l = negate ? getSatisfyingConditionSplit() : getUnsatisfyingConditionSplit();
//            const DecisionTree* r = negate ? getUnsatisfyingConditionSplit() : getSatisfyingConditionSplit();
//            assert(l || r);
//
//            if(l) {
//                dt_predicate cpy = candidate.first;
//
//                switch(cpy.pred){
//                    case dt_predicate::LEQ_THAN:
//                        cpy.pred = dt_predicate::G_THAN;
//                        break;
//                    case dt_predicate::IN_SET:
//                        cpy.pred = dt_predicate::NOT_IN_SET;
//                        break;
//                    case dt_predicate::L_THAN:
//                    case dt_predicate::G_THAN:
//                    case dt_predicate::GEQ_THAN:
//                    case dt_predicate::NOT_IN_SET:
//                        /* We should never get here, assign copies only */
//                        break;
//                }
//
//                if (!current) {
//                    current = new std::vector<dt_predicate>{cpy};
//                }
//                else {
//                    current->push_back(cpy);
//                }
//
//                l->populate_children_predicates(decision_to_pred, current, negate);
//            }
//            if(r) {
//                if (!current) {
//                    current = new std::vector<dt_predicate>{candidate.first};
//                }
//                else {
//                    current->push_back(candidate.first);
//                }
//
//                r->populate_children_predicates(decision_to_pred, current, negate);
//            }
//        } else {
//            auto it = decision_to_pred.find(majority_class);
//            std::pair<double,std::vector<dt_predicate>> cp{majority_class_precision, *current};
//            /* Below will only happen on labels occurring > 1 (hence why we need vector of vectors */
//            if (it != decision_to_pred.end()) {
//                it->second.emplace_back(cp);
//            } else {
//                decision_to_pred[majority_class].emplace_back(cp);
//            }
//
//            current->pop_back();
//        }
    }


    DecisionTree(typename  std::vector<std::pair<T,int>>::iterator& begin,
                 typename  std::vector<std::pair<T,int>>::iterator& end,
                 int max_class_id,
                 const std::function<union_minimal(const T&, const std::string&)>& f,
                 std::unordered_set<dt_predicate>& candidates,
                 ForTheWin::gain_measures measure,
                 double pi,
                 const std::size_t l,
                 const std::size_t visitors,
                 const std::size_t eta = 1,
                 bool use_leq_for_categorical = true,
                 double *goodness = nullptr,
                 double* total_weights = nullptr,
                 size_t height = std::numeric_limits<size_t>::max()) : F{f} {
        ForTheWin forthegain(max_class_id);
        std::size_t N = std::distance(begin, end);
        is_root = N == visitors;

        if (!goodness) goodness = &this->goodness;
        if (!total_weights) total_weights = &this->total_weights;

        double purity = 0.0;
        int clazz = -1;
        auto it = begin;
        for (std::size_t i = 0; i<N; i++) {
            int curr_clazz = it->second;
            auto tmp = (double)(forthegain.countClass(curr_clazz));
            if (tmp > purity) {
                purity = tmp;
                clazz = curr_clazz;
            }
            it++;
        }
        forthegain.normalizeCountClass();
        const double current_weight = purity / visitors;
        purity = purity/((double) N);
        if ((N<=eta) || (purity >= pi) || (height == 0)) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }
        isLeaf = false;
        candidate.second = -1;

        for (const auto& candidate_x: candidates) {
            std::function<union_minimal(const T&)> wrap = [&f,&candidate_x](const T& x) {
                return f(x,candidate_x.field);
            };
            std::pair<dt_predicate,double> result = evaluate_predicate(begin, end, wrap, forthegain, max_class_id, measure, candidate_x);
            if (result.second > candidate.second) {
                std::swap(candidate, result);
            }
        }


        if (candidate.second == -1) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }

        auto it2 = std::stable_partition(begin, end, [this](const std::pair<T,int>& obj) {
            return operator()(obj.first);
        });

        if ((begin == it2) || (it2 == end)) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }

        {
            auto tmp = candidates;
            tmp.erase(candidate.first);
            children.emplace_back(begin, it2, max_class_id, f, tmp, measure, pi, l, visitors, eta, use_leq_for_categorical, goodness, total_weights, height-1);
            children.emplace_back(it2, end, max_class_id, f, tmp, measure, pi, l, visitors, eta, use_leq_for_categorical, goodness, total_weights, height-1);
        }


        if(is_root) {
            /* Only on the very last iteration, the original candidate */
            *goodness /= *total_weights;
        }
    }

    DecisionTree(typename  std::vector<std::pair<T,int>>::iterator& begin,
                 typename  std::vector<std::pair<T,int>>::iterator& end,
                 int max_class_id,
                 const std::function<union_minimal(const T&, const std::string&)>& f,
                 const std::unordered_set<std::string>& numeric_attributes,
                 const std::unordered_set<std::string>& categorical_attributes,
                 ForTheWin::gain_measures measure,
                 double pi,
                 const std::size_t l,
                 const std::size_t visitors,
                 const std::size_t eta = 1,
                 bool use_leq_for_categorical = true,
                 double *goodness = nullptr,
                 double* total_weights = nullptr,
                 size_t height = std::numeric_limits<size_t>::max()) : F{f} {
        if (begin == end) {
            return;
        }
        ForTheWin forthegain(max_class_id);
        std::size_t N = std::distance(begin, end);
        is_root = N == visitors;

        if (!goodness) goodness = &this->goodness;
        if (!total_weights) total_weights = &this->total_weights;

        double purity = 0.0;
        int clazz = -1;
        auto it = begin;
        for (std::size_t i = 0; i<N; i++) {
            int curr_clazz = it->second;
            auto tmp = (double)(forthegain.countClass(curr_clazz));
            if (tmp > purity) {
                purity = tmp;
                clazz = curr_clazz;
            }
            it++;
        }
        forthegain.normalizeCountClass();
        const double current_weight = purity / visitors;
        purity = purity/((double) N);
        if ((N<=eta) || (purity >= pi) || (height == 0)) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }
        isLeaf = false;
        candidate.second = -1;
        for (const std::string& attribute : numeric_attributes) {
            std::function<union_minimal(const T&)> wrap = [&f,&attribute](const T& x) {
                return f(x,attribute);
            };
            std::pair<dt_predicate,double> result = evaluate_leq_predicate(begin, end, wrap, forthegain, max_class_id, measure);
            if (result.second > candidate.second) {
                result.first.field = attribute;
                std::swap(candidate, result);
            }
        }

        for (const std::string& attribute : categorical_attributes) {
            std::function<union_minimal(const T&)> wrap = [&f,&attribute](const T& x) {
                return f(x,attribute);
            };
            if (use_leq_for_categorical) {
                std::pair<dt_predicate,double> result = evaluate_leq_predicate(begin, end, wrap, forthegain, max_class_id, measure);
                if (result.second > candidate.second) {
                    result.first.field = attribute;
                    std::swap(candidate, result);
                }
            }
            std::pair<dt_predicate,double> result2 = evaluate_inset_predicate(begin, end, wrap, forthegain, max_class_id, measure, l);
            if (result2.second > candidate.second) {
                result2.first.field = attribute;
                std::swap(candidate, result2);
            }
        }

        if (candidate.second == -1) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }

        auto it2 = std::stable_partition(begin, end, [this](const std::pair<T,int>& obj) {
            return operator()(obj.first);
        });

        if ((begin == it2) || (it2 == end)) {
            isLeaf = true;
            majority_class = clazz;
            majority_class_precision = forthegain.getClassPrecision(clazz);
            leaf = {begin, end};
            *total_weights += current_weight;
            *goodness += (majority_class_precision * current_weight);
            return;
        }

        children.emplace_back(begin, it2, max_class_id, f, numeric_attributes, categorical_attributes, measure, pi, l, visitors, eta, use_leq_for_categorical, goodness, total_weights, height-1);
        children.emplace_back(it2, end, max_class_id, f, numeric_attributes, categorical_attributes, measure, pi, l, visitors, eta, use_leq_for_categorical, goodness, total_weights, height-1);

        if(is_root) {
            /* Only on the very last iteration, the original candidate */
            *goodness /= *total_weights;
        }
    }
};


#endif //KNOBAB_SERVER_STRUCTURE_H
