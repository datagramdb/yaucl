//
// Created by giacomo on 05/10/22.
//

#include <knobab/server/query_manager/Environment.h>
#include <knobab/algorithms/mining/pattern_mining.h>
#include "../../../PatternMiningConfiguration.h"


void just_mining(Environment& pos,
                 Environment& neg,
                 PatternMiningConfiguration& conf) {
    size_t negModelOffset, negModelSize;
    double posModelMiningTime, negModelMiningTime;

    // Generating both models, but dataless!
    std::vector<pattern_mining_result<DeclareDataAware>> posModel, negModel;
    std::tie(posModel, conf.posModelMiningTime) = pattern_mining(pos.db, conf.support, conf.naif, conf.init_end, conf.special_temporal_patterns, conf.only_precise_temporal_patterns, conf.negative_patterns);
    std::tie(negModel, conf.negModelMiningTime) = pattern_mining(neg.db, conf.support, conf.naif, conf.init_end, conf.special_temporal_patterns, conf.only_precise_temporal_patterns, conf.negative_patterns);
    negModelOffset = posModel.size();
    negModelSize = negModel.size();



    {
        // Applying the atomization on both, so the clause derivation and inference is easier!
        std::cout << "Copying the positive and negative model into one unique model" << std::endl;
        Environment global;


        global.conjunctive_model.reserve(negModelOffset+negModel.size());
        for (const auto& fromPos : posModel)
            global.conjunctive_model.emplace_back(fromPos.clause);
        for (const auto& fromNeg : negModel)
            global.conjunctive_model.emplace_back(fromNeg.clause);
        std::cout << "Setting up the global environment..." << std::endl;
        global.set_grounding_parameters(conf.doPreliminaryFill, conf.ignoreActForAttributes, conf.creamOffSingleValues, conf.ps);
        global.doGrounding();
        global.set_atomization_parameters(conf.fresh_atom_label, conf.mslength, conf.strategy);
        global.init_atomize_tables();
        global.first_atomize_model();

        // Moving back to the mined models the information concerning the atomization outcome
        std::cout << "Moving the atomisation outcome..." << std::endl;
        for (size_t i = 0; i<negModelOffset; i++) {
            std::swap(posModel[i].clause.left_decomposed_atoms, global.conjunctive_model[i].left_decomposed_atoms);
            std::swap(posModel[i].clause.right_decomposed_atoms, global.conjunctive_model[i].right_decomposed_atoms);
        }
        for (size_t i = 0; i<negModelSize; i++) {
            std::swap(negModel[i].clause.left_decomposed_atoms, global.conjunctive_model[i+negModelOffset].left_decomposed_atoms);
            std::swap(negModel[i].clause.right_decomposed_atoms, global.conjunctive_model[i+negModelOffset].right_decomposed_atoms);
        }
        std::cout << "WARNING! The Positive Environment will be the only one containing the atomisation information (this is for avoiding useless data duplication on both environments)" << std::endl;
        std::swap(pos.ap, global.ap);

        // Destroying the information pertaining to the global environment
    }

}

int main() {
    PatternMiningConfiguration conf;
    conf.doStats = false;
    conf.set_grounding_parameters(true, false, true,GroundingStrategyConf::NO_EXPANSION);
    conf.set_atomization_parameters("p", 20, AtomizeEverythingIfAnyDataPredicate);
    std::string pos = "/media/giacomo/OS/Users/berga/source/repos/knobab_server/data/benchmarking/1000_10000_log.tab";
    std::string neg = "/media/giacomo/OS/Users/berga/source/repos/knobab_server/data/benchmarking/10_10_log.tab";

    // Setting up the data loading information for both the positive and the negative model
    conf.setup(pos, neg);
    Environment epos, eneg;
    // Loading both the negative and the positive model
    conf.load_data(epos, eneg);
    just_mining(epos, eneg, conf);
    return 0;
}