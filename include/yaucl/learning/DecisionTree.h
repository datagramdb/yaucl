//
// Created by giacomo on 02/12/24.
//

#ifndef DT_DECISIONTREE_H
#define DT_DECISIONTREE_H

#include <map>
#include <iostream>
#include <variant>
#include <vector>
#include <stack>

#include <unordered_set>
#include <yaucl/learning/dt/commons.h>
#include <yaucl/data/csv/csv.h>





#include <algorithm>
//#include <yaucl/learning/dt/utils.h>
#include <yaucl/learning/dt/structures/DataRepo.h>


struct DecisionTree {
    DataRepo dr;
    std::vector<Nodes> children;
    double goodness;
    double total_weights;
    const std::unordered_set<std::string>& numerical;
    const std::unordered_set<std::string>&categorical;

    DecisionTree(const data_record& records,
                 const data_clazzes& records_classes,
                 const std::unordered_set<std::string>& numerical,
                 const std::unordered_set<std::string>& categorical,
                 size_t max_height = std::numeric_limits<size_t>::max(),
                 double maxPrec = 1.0,
                 size_t l = 1,
                 size_t eta = 1) : dr{records,records_classes,max_height,maxPrec,l,eta}, numerical{numerical}, categorical{categorical} {
        goodness = total_weights = 0;
    }


    void splitTree();

    void populate_children_predicates2(std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred) const {
        std::vector<dt_predicate> memo;
        populate_children_predicates2(0, decision_to_pred, memo);
    }

private:
    void populate_children_predicates2(size_t nodeid,
                                       std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred,
                                       std::vector<dt_predicate>& current_stack) const;
};


#endif //DT_DECISIONTREE_H
