//
// Created by giacomo on 17/09/22.
//

#ifndef KNOBAB_PATTERN_MINING_H
#define KNOBAB_PATTERN_MINING_H

#include <vector>
#include "knobab/server/tables/KnowledgeBase.h"
#include "knobab/server/declare/DeclareDataAware.h"

enum TargetCase {
    IsTargetPrev,
    IsTargetNext,
    IsTargetData,
    NoTargetPresent
};

template <typename T>
struct pattern_mining_result {
    T clause;
    double           support_generating_original_pattern;
    double           support_declarative_pattern;
    double           confidence_declarative_pattern;

//    std::vector<ActTable::record *> activations{};
//    std::vector<ActTable::record *> targets{};
//    TargetCase target_casusu = NoTargetPresent;

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


std::pair<std::vector<pattern_mining_result<DeclareDataAware>>,double> pattern_mining(const KnowledgeBase& kb,
                                                                    double support,
                                                                    bool naif,
                                                                    bool init_end,
                                                                    bool special_temporal_patterns,
                                                                    bool only_precise_temporal_patterns,
                                                                    bool negative_patterns);

//std::vector<pattern_mining_result<DeclareDataAware>> classifier_mining(const KnowledgeBase& pos,
//                                                                       const KnowledgeBase& neg,
//                                                                    double support,
//                                                                    bool naif,
//                                                                    bool init_end,
//                                                                       bool special_temporal_patterns,
//                                                                       bool only_precise_temporal_patterns);

#endif //KNOBAB_PATTERN_MINING_H
