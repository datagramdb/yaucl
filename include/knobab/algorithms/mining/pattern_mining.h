//
// Created by giacomo on 17/09/22.
//

#ifndef KNOBAB_PATTERN_MINING_H
#define KNOBAB_PATTERN_MINING_H

#include <vector>
#include <knobab/server/declare/DeclareDataAware.h>
#include <yaucl/bpm/structures/commons.h>

enum TargetCase {
    IsTargetPrev,
    IsTargetNext,
    IsTargetData,
    NoTargetPresent
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
