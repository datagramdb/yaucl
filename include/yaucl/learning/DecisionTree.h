/*
 * DecisionTree.h
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

#ifndef DISTANCE_DECISIONTREE_H
#define DISTANCE_DECISIONTREE_H

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
        return F(x.first)<F(y.first);
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
                N[val] = n.at(it->second);
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

// Returns which bits are on in the integer a
std::vector<int> getOnLocations(int a);

template<typename T>
std::vector<std::unordered_set<T> > powerSet(const std::vector<T>& set, size_t maxSize) {
    std::vector<std::unordered_set<T> > result;
    int numPowerSets = static_cast<int>(std::pow(2.0, static_cast<double>(set.size())));
    for (size_t i = 0; i < numPowerSets; ++i) {
        std::vector<int> onLocations = getOnLocations(i);
        if (onLocations.size() > maxSize) continue;
        if (onLocations.empty()) continue;
        std::unordered_set<T> subSet;
        for (size_t j = 0; j < onLocations.size(); ++j) {
            subSet.insert(set.at(onLocations.at(j)));
        }
        result.push_back(subSet);
    }
    return result;
}

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
            for (const auto& child : children) child.print_rec(os, depth+1);
        }

        if(is_root) {
            os << "GOODNESS: " << goodness << std::endl;
        }
    }

    void populate_children_predicates(std::unordered_map<int, std::vector<std::vector<dt_predicate>>> &decision_to_pred,
                                      std::vector<dt_predicate>* current = nullptr,
                                      bool negate = false) const {
        if(!isLeaf) {
            const DecisionTree* l = negate ? getSatisfyingConditionSplit() : getUnsatisfyingConditionSplit();
            const DecisionTree* r = negate ? getUnsatisfyingConditionSplit() : getSatisfyingConditionSplit();
            assert(l || r);

            if(l) {
                dt_predicate cpy = candidate.first;

                switch(cpy.pred){
                    case dt_predicate::LEQ_THAN:
                        cpy.pred = dt_predicate::G_THAN;
                        break;
                    case dt_predicate::IN_SET:
                        cpy.pred = dt_predicate::NOT_IN_SET;
                        break;
                    case dt_predicate::GEQ_THAN:
                    case dt_predicate::NOT_IN_SET:
                        /* We should never get here, assign copies only */
                        break;
                }

                if (!current) {
                    current = new std::vector<dt_predicate>{cpy};
                }
                else {
                    current->push_back(cpy);
                }

                l->populate_children_predicates(decision_to_pred, current, negate);
            }
            if(r) {
                if (!current) {
                    current = new std::vector<dt_predicate>{candidate.first};
                }
                else {
                    current->push_back(candidate.first);
                }

                r->populate_children_predicates(decision_to_pred, current, negate);
            }
        } else {
            auto it = decision_to_pred.find(majority_class);
            /* Below will only happen on labels occurring > 1 (hence why we need vector of vectors */
            if (it != decision_to_pred.end()) {
                it->second.push_back(*current);
            }
            else {
                decision_to_pred.insert({majority_class, { *current }});
            }

            current->pop_back();
        }
    }

    DecisionTree(typename  std::vector<std::pair<T,int>>::iterator& begin,
                 typename  std::vector<std::pair<T,int>>::iterator& end,
                 size_t max_class_id,
                 const std::function<union_minimal(const T&, const std::string&)>& f,
                 const std::unordered_set<std::string>& numeric_attributes,
                 const std::unordered_set<std::string>& categorical_attributes,
                 ForTheWin::gain_measures measure,
                 double pi,
                 const std::size_t l,
                 const uint16_t visitors,
                 const std::size_t eta = 1,
                 double *goodness = nullptr,
                 double* total_weights = nullptr) : F{f} {
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
        if ((N<=eta) || (purity >= pi)) {
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
            std::pair<dt_predicate,double> result = evaluate_leq_predicate(begin, end, wrap, forthegain, max_class_id, measure);
            if (result.second > candidate.second) {
                result.first.field = attribute;
                std::swap(candidate, result);
            }
            std::pair<dt_predicate,double> result2 = evaluate_inset_predicate(begin, end, wrap, forthegain, max_class_id, measure, l);
            if (result2.second > candidate.second) {
                result2.first.field = attribute;
                std::swap(candidate, result2);
            }
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

        children.emplace_back(begin, it2, max_class_id, f, numeric_attributes, categorical_attributes, measure, pi, l, visitors, eta, goodness, total_weights);
        children.emplace_back(it2, end, max_class_id, f, numeric_attributes, categorical_attributes, measure, pi, l, visitors, eta, goodness, total_weights);

        if(is_root){
            /* Only on the very last iteration, the original candidate */
            *goodness /= *total_weights;
        }
    }
};


#endif //DISTANCE_DECISIONTREE_H
