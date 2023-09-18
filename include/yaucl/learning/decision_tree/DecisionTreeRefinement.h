/*
 * DecisionTreeRefinement.h
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

#ifndef KNOBAB_SERVER_DECISIONTREEREFINEMENT_H
#define KNOBAB_SERVER_DECISIONTREEREFINEMENT_H

#include <yaucl/learning/decision_tree/structure.h>
#include <yaucl/hashing/umap_hash.h>

using concrete_dt_cell = std::pair<std::string,int>;
using concrete_dt_row = std::unordered_map<std::string, int> ;
using concrete_dt_table_for_classes = std::vector<concrete_dt_row >;
using concrete_dt_table = std::unordered_map<int, concrete_dt_table_for_classes >;
using decision_tree_map = std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>>;
using decision_tree_collection = std::vector<concrete_dt_table >;

//// Functions for representing the decision functions as tables
//// Coming from each other decision class

#include <optional>

static inline
std::optional<concrete_dt_cell> concrete_dt_pair(const dt_predicate& x,
                                                 bool isMetaClassifier,
                                                 std::unordered_map<std::string, std::unordered_set<int>>& map) {
    if (isMetaClassifier) {
        if (x.pred==dt_predicate::IN_SET) {
            int val = (int)std::get<double>(*x.categoric_set.begin());
            auto& S = map[x.field];
            if (S.contains(val)) {
                if (S.size() > 1) {
                    S.clear();
                    S.insert(val);
                }
            } else
                map.erase(x.field);
        } else if (x.pred ==dt_predicate::NOT_IN_SET) {
            int val = (int)std::get<double>(*x.categoric_set.begin());
            auto& S = map[x.field];
            S.erase(val);
        }
        return {};
    } else {
        if (x.pred==dt_predicate::IN_SET)
            return {concrete_dt_cell{x.field, (int)std::get<double>(*x.categoric_set.begin())}};
        else if (x.pred ==dt_predicate::NOT_IN_SET) {
            auto val = (int)std::get<double>(*x.categoric_set.begin());
            if ((val < -1) || (val > 1))
                exit(2); // ERROR
            return {concrete_dt_cell{x.field, val==-1?1:-1}};
        } else
            exit(3);
    }

}

#include "yaucl/structures/set_operations.h"

static inline
std::vector<concrete_dt_row> concrete_dt_vector(const std::vector<dt_predicate>& vector,
                                                bool isMetaClassifier,
                                                std::unordered_map<std::string, std::unordered_set<int>>& meta_map) {

    if (!isMetaClassifier) {
        std::unordered_map<std::string, int> map;
        for (const auto& x : vector) {
            auto cp = concrete_dt_pair(x, isMetaClassifier, meta_map);
            if ((!isMetaClassifier) && cp.has_value())
                map[cp->first] = cp->second;
        }
        return {map};
    } else {
        auto copy_meta = meta_map;
        std::vector<std::string> schema_result;
        std::vector<std::vector<int>> schema_numbers;
        std::unordered_set<std::string> to_retain;
        for (const auto& x : vector) {
            to_retain.emplace(x.field);
            concrete_dt_pair(x, isMetaClassifier, copy_meta);
        }
        for (auto& [k,v] : copy_meta) {
            if ((!v.empty()) && (to_retain.contains(k))) {
                schema_result.emplace_back(k);
                schema_numbers.emplace_back(v.begin(), v.end());
            }
        }
        std::vector<concrete_dt_row> result;
        cartesian_product<int>(schema_numbers, [&schema_result,&result](const std::vector<int>& v) {
            auto& item = result.emplace_back();
            for (size_t i = 0, N = v.size(); i<N; i++) {
                const auto& x = schema_result.at(i);
                item[x] = v.at(i);
            }
        });
        return result;
    }
}

/***
 *
 * @param map
 * @param isMetaClassifier
 * @param meta_map
 * @author: Giacomo Bergami
 */
static inline
concrete_dt_table concrete_dt_table_from_class_result(const std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>>& map,

                                                      bool isMetaClassifier,
                                                      std::unordered_map<std::string, std::unordered_set<int>>& meta_map) {
    concrete_dt_table result;
    for (const auto& [k,v] : map) {
        auto& resulted = result[k];
        for (const auto& [score,table] : v) {
            for (const auto& x : concrete_dt_vector(table,  isMetaClassifier, meta_map))
                resulted.emplace_back(x);
        }
    }
    return result;
}

/***
 * Classifies a record given a tabular representation of the decision tree
 *
 * @tparam T                        Record representation
 * @param decision_tree_slow        Tabular representation of a decision tree
 * @param accessor                  Accessor for a field associated to a record
 * @param record                    Record to classify
 * @return  Classification outcome
 */
template <typename T>
int classification_outcome(const std::unordered_map<int, concrete_dt_table_for_classes >& decision_tree_slow,
                           const std::function<union_minimal(const T&, const std::string&)>& accessor,
                           const T& record) {
    for (const auto& [k,v] : decision_tree_slow) {
        for (const auto& decisor : v) {
            bool match = true;
            for (const auto& [field,expected] : decisor) {
                if ((((int)std::get<double>(accessor(record,field)))!=expected)) {
                    match = false;
                    break;
                }
            }
            if (match) return k;
        }
    }
    return -1;
}

/***
 * Combines the classification outcome from two classifiers for the same class detection task
 *
 * @param left                  Left Table record
 * @param right                 Right Table record
 * @param result                Container for the record combination
 * @param generalisation        Function defining how to deal with conflicting classification. This function takes
 *                               - Data representation to potentially extend
 *                               - Key to be inserted in the data, if we are able to resolve the conflict
 *                               - Class coming from the left table
 *                               - Class coming from the right table
 * @param intersect             Whether we want to retain the classification outcome from both classifiers or if they
 *                              provide different views on the data, or just retain the intersection
 */
static inline
void oplus_concrete_dt_vectors(const concrete_dt_row& left,
                               const concrete_dt_row& right,
//                                        const std::unordered_set<std::string>& S,
                               concrete_dt_table_for_classes& result,
                               const std::function<bool(concrete_dt_row&, const std::string& key, int left, int right)>& generalisation,
                               bool intersect = false
)  {
    concrete_dt_row map;
    std::unordered_set<std::string> S;
    for (const auto& [k,v] : left) S.emplace(k);
    for (const auto& [k,v] : right) S.emplace(k);
    for (const auto& key : S) {
        auto itL = left.find(key);
        auto itR = right.find(key);
        if ((itL != left.end()) && (itR != right.end())) {
            if (itL->second != itR->second) {
                if (generalisation(map,key,itL->second,itR->second))
                    continue;
                else
                    return;
            } else
                map[key] = itL->second;
        } else if ((!intersect) && (itL != left.end())) {
            map[key] = itL->second;
        } else if ((!intersect) && (itR != right.end())) {
            map[key] = itR->second;
        }
    }
    if (!map.empty())
        result.emplace_back(map);
}

/***
 * Combines two tables describing the same class from two different classifiers
 *
 * @tparam T
 * @param leftTable             Left table
 * @param rightTable            Right table
 * @param result                Resulting combined table
 * @param generalisation        Function defining how to deal with conflicting classification. This function takes
 *                               - Data representation to potentially extend
 *                               - Key to be inserted in the data, if we are able to resolve the conflict
 *                               - Class coming from the left table
 *                               - Class coming from the right table
 * @param intersect             Whether we want to retain the classification outcome from both classifiers or if they
 *                              provide different views on the data, or just retain the intersection
 */
template <typename T>
void oplus_concrete_dt_tables(const T& leftTable,
                              const concrete_dt_table_for_classes& rightTable,
//                               const std::unordered_set<std::string>& S,
                              concrete_dt_table_for_classes& result,
                              const std::function<bool(concrete_dt_row&, const std::string& key, int left, int right)>& generalisation,
                              bool intersect = false
)  {
    for (const auto& left : leftTable) {
        for (const auto& right : rightTable) {
            oplus_concrete_dt_vectors(left, right, result, generalisation, intersect);
        }
    }
    {
        std::unordered_set<concrete_dt_row > S{result.begin(), result.end()};
        result.clear();
        result.insert(result.begin(), S.begin(), S.end());
    }
}

#include "yaucl/hashing/umap_hash.h"

/***
 *
 *
 * @param vector_metaclassifier                         Ensemble classifier combining all classes together, and
 *                                                      associating the outcome for each classifier to the expected
 *                                                      class
 * @param collected_single_classifiers                  Result of the classification for each concrete classifier,
 *                                                      already transformed from a map
 * @param all_classifiers_progressive_int_to_string     String-id
 * @param generalisation                                Function defining how to deal with conflicting classification. This function takes
 *                                                       - Data representation to potentially extend
 *                                                       - Key to be inserted in the data, if we are able to resolve
 *                                                         the conflict
 *                                                       - Class coming from the left table
 *                                                       - Class coming from the right table
 * @param meta_map                                      Result of the classification associated to the meta-
 *                                                      classifier as a decision tree, where each field remarks
 *                                                      using IN_SET which is...
 * @return
 */
static inline concrete_dt_table_for_classes instantiate_meta_classifier_row(const std::vector<dt_predicate>& vector_metaclassifier,
                                                                            const decision_tree_collection& collected_single_classifiers,
                                                                            const std::unordered_set<std::string>& all_classifiers_progressive_int_to_string,
                                                                            const std::function<bool(concrete_dt_row&, const std::string& key, int left, int right)>& generalisation,
                                                                            std::unordered_map<std::string, std::unordered_set<int>>& meta_map) {
    auto meta_rows = concrete_dt_vector(vector_metaclassifier,  true, meta_map);
    concrete_dt_table_for_classes current;
    for (const auto& meta_row : meta_rows) {
        bool isFirst = true;
        concrete_dt_table_for_classes result;
        std::cout << "\t" << meta_row << std::endl;
        for (const auto& [k,v] : meta_row) {
            size_t classifier_id = std::stoull(k);
            const concrete_dt_table_for_classes& rhs = collected_single_classifiers.at(classifier_id).at(v);
            if (isFirst) {
                result = rhs;
                isFirst = false;
            } else {
                concrete_dt_table_for_classes tmp;
                oplus_concrete_dt_tables(result, rhs, tmp, generalisation);
                std::swap(tmp, result);
                if (result.empty()) break;
            }
        }
        {
            std::unordered_set<concrete_dt_row > S{result.begin(), result.end()};
            result.clear();
            current.insert(current.begin(), S.begin(), S.end());
        }

    }
    return current;
}

/**
 * Provides an explainable representation of the meta-classifier merged with the results of each single classifier,
 * where the resulting table describes the merged overall classification interpretation across models
 *
 * @param meta_map                                      Result of the classification associated to the meta-classifier as a decision tree, where each field remarks using IN_SET which is
 * @param collected_single_classifiers                  Result of the classification for each concrete classifier, already transformed from a map
 * @return                                              Merged representation of all the tables
 */
static inline concrete_dt_table instantiate_meta_classifier_result(const std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>>& meta_map,
                                                                   const decision_tree_collection& collected_single_classifiers,
                                                                   const std::function<bool(concrete_dt_row&, const std::string& key, int left, int right)>& generalisation,
                                                                   std::unordered_map<std::string, std::unordered_set<int>>& meta_maps

) {
    std::unordered_set<std::string> all_classifiers_progressive_int_to_string;
    all_classifiers_progressive_int_to_string.reserve(collected_single_classifiers.size());
    for (size_t i = 0, N = collected_single_classifiers.size(); i<N; i++)
        all_classifiers_progressive_int_to_string.emplace(std::to_string(i));
    concrete_dt_table resulted;
    for (const auto& [final_class, disjunctions] : meta_map) {
        std::cout << final_class << std::endl;
        concrete_dt_table_for_classes result;
        for (const auto& [score, row] : disjunctions) {
            std::cout << " * " << row << std::endl;
            auto c = instantiate_meta_classifier_row(row, collected_single_classifiers, all_classifiers_progressive_int_to_string, generalisation, meta_maps);
            if (!c.empty())
                result.insert(result.begin(), c.begin(), c.end()); // TODO: move inserter
        }
        {
            std::unordered_set<concrete_dt_row> S{result.begin(), result.end()};
            result.clear();
            result.insert(result.begin(), S.begin(), S.end());
        }
        resulted[final_class] = result;
    }
    return resulted;
}

#include "yaucl/strings/serializers.h"

static inline
bool semantics_ignore(concrete_dt_row&, const std::string& key, int left, int right) {
    return true;
}

static inline
bool semantics_discard(concrete_dt_row&, const std::string& key, int left, int right) {
    return false;
}

#endif //KNOBAB_SERVER_DECISIONTREEREFINEMENT_H
