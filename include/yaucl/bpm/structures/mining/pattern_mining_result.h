//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_PATTERN_MINING_RESULT_H
#define KNOBAB_SERVER_PATTERN_MINING_RESULT_H

#include <iostream>
#include <yaucl/structures/default_constructors.h>

template <typename T>
struct pattern_mining_result {
    T clause;
    double           support_generating_original_pattern;
    double           support_declarative_pattern;
    double           confidence_declarative_pattern;

    pattern_mining_result(const T& clause) : pattern_mining_result(1.0, clause) {}
    pattern_mining_result(double support,
                          const T &clause) : pattern_mining_result(clause, support, support, -1.0) {}
    pattern_mining_result(const T &clause,
                          double supportGeneratingOriginalPattern,
                          double supportDeclarativePattern,
                          double confidenceDeclarativePattern) : clause(clause),
                                                                 support_generating_original_pattern(
                                                                         supportGeneratingOriginalPattern),
                                                                 support_declarative_pattern(
                                                                         supportDeclarativePattern),
                                                                 confidence_declarative_pattern(confidenceDeclarativePattern) {}


    DEFAULT_CONSTRUCTORS(pattern_mining_result);
    friend std::ostream &operator<<(std::ostream &os, const pattern_mining_result &result){
        os << "Clause: " << result.clause << std::endl
           << "\t - Pattern Maching Support: " << result.support_generating_original_pattern << std::endl;
        if (result.support_declarative_pattern >= 0.0)
            os << "\t - Declarative Pattern's Support: " << result.support_declarative_pattern << std::endl;
        if (result.confidence_declarative_pattern >= 0.0)
            os << "\t - Declarative Pattern's Confidence: " << result.confidence_declarative_pattern << std::endl;
        return os << std::endl;
    }

};


#endif //KNOBAB_SERVER_PATTERN_MINING_RESULT_H
