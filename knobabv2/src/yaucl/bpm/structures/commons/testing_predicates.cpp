//
// Created by giacomo on 03/01/2022.
//

#include "yaucl/bpm/structures/commons/testing_predicates.h"

bool test_minimal_data_predicate(const DataPredicate &dp, const union_minimal &val) {
    if (std::holds_alternative<std::string>(val)) {
        return dp.testOverSingleVariable(std::get<std::string>(val));
    } else {
        if (std::holds_alternative<double>(val))
            return dp.testOverSingleVariable(std::get<double>(val));
        else
            throw std::runtime_error(std::to_string(val.index())+" Unexpected index type");
    }
}