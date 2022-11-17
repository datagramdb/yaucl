//
// Created by giacomo on 10/11/22.
//

#ifndef KNOBAB_SERVER_PATTERNMININGCONFIGURATION_H
#define KNOBAB_SERVER_PATTERNMININGCONFIGURATION_H

#include <yaucl/bpm/structures/log/data_loader.h>
#include <fstream>
#include "knobab/server/query_manager/Environment.h"
#include "knobab/algorithms/mining/pattern_mining.h"

struct PatternMiningConfiguration;

struct InputFormat {
    log_data_format type;
    std::string path;
    bool loadData = true;
    bool doPreliminaryFill = true;
    bool ignoreActForAttributes = false;
    bool creamOffSingleValues = true;
    GroundingStrategyConf::pruning_strategy ps = GroundingStrategyConf::NO_EXPANSION;
    std::string fresh_atom_label = "p";
    bool setMaximumStrLen;
    size_t mslength = MAXIMUM_STRING_LENGTH;
    AtomizationStrategy strategy = AtomizeEverythingIfAnyDataPredicate;

    InputFormat(const struct PatternMiningConfiguration& x, const std::string& path);

    DEFAULT_CONSTRUCTORS(InputFormat)
    void   openFile(std::ifstream& fileOpenAsPath) const;
    double openEnvironment(Environment& env) const;
};

struct PatternMiningConfiguration {
    // Model and data loading parameters
    log_data_format type;
    bool doStats = false;
    bool loadData = true;
    bool doPreliminaryFill = true;
    bool ignoreActForAttributes = false;
    bool creamOffSingleValues = true;
    GroundingStrategyConf::pruning_strategy ps = GroundingStrategyConf::NO_EXPANSION;
    std::string fresh_atom_label = "p";
    bool setMaximumStrLen;
    size_t mslength = MAXIMUM_STRING_LENGTH;
    AtomizationStrategy strategy = AtomizeEverythingIfAnyDataPredicate;


    //
    double support = 0.01;
    bool naif = false;
    bool init_end = true;
    bool special_temporal_patterns = true;
    bool only_precise_temporal_patterns = false;
    bool negative_patterns = false;

    InputFormat positive_traces;
    InputFormat negative_traces;

    void setup(const std::string& pos, const std::string& neg) {
        positive_traces = {*this, pos};
        negative_traces = {*this, neg};
    }
    void load_data(Environment& env_pos, Environment& env_neg) {
        posDataLoadingTime = positive_traces.openEnvironment(env_pos);
        negDataLoadingTime = negative_traces.openEnvironment(env_neg);
    }

    DEFAULT_CONSTRUCTORS(PatternMiningConfiguration)

    void set_grounding_parameters(bool doPreliminaryFill, bool ignoreActForAttributes, bool creamOffSingleValues, GroundingStrategyConf::pruning_strategy strategy);
    void set_atomization_parameters(const std::string &fresh_atom_label, size_t mslength, AtomizationStrategy strategy);

    /// Logging information
    double posDataLoadingTime, negDataLoadingTime;
    double posModelMiningTime, negModelMiningTime;
};

#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/vector_hash.h>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/hashing/umap_hash.h>

using clause_id = size_t;
using clause_name = std::string;
using activity_label = std::string;
using activation_label = activity_label;
using target_label = activity_label;
template <typename K, typename V> using umap = std::unordered_map<K,V>;

struct collected_clauses_unary {
    umap<clause_name, umap<activation_label, std::vector<clause_id>>>                        activation_dataless;
    umap<clause_name, umap<activation_label, umap<data_conditions, std::vector<clause_id>>>> activation_withdata;

    void fit(size_t id, const pattern_mining_result<DeclareDataAware>& x) {
        clause_name name = x.clause.casusu;
        activation_label activation = x.clause.left_act;
        activation_dataless[name][activation].emplace_back(id);
        if (!x.clause.dnf_left_map.empty()) {
            activation_withdata[name][activation][x.clause.dnf_left_map].emplace_back(id);
        }
    }

    DEFAULT_CONSTRUCTORS(collected_clauses_unary)
};

struct collected_clauses_binary {
    collected_clauses_unary common;
    umap<clause_name, umap<activation_label, umap<data_conditions, umap<target_label, std::vector<clause_id>>>>> no_targ_data;
    umap<clause_name, umap<activation_label, umap<target_label, std::vector<clause_id>>>> dataless_binary_clause;
    umap<clause_name, umap<activation_label, umap<target_label, umap<data_conditions, std::vector<clause_id>>>>> no_act_targ_but_corr;

    void fit(size_t id, const pattern_mining_result<DeclareDataAware>& x) {
        DEBUG_ASSERT(!x.clause.right_act.empty());
        common.fit(id, x);
        clause_name name = x.clause.casusu;
        activation_label activation = x.clause.left_act;
        activation_label target = x.clause.right_act;
        dataless_binary_clause[name][activation][target].emplace_back(id);
        if (!x.clause.conjunctive_map.empty()) {
            no_act_targ_but_corr[name][activation][target][x.clause.conjunctive_map].emplace_back(id);
        }
        if (!x.clause.dnf_left_map.empty()) {
            no_targ_data[name][activation][x.clause.dnf_left_map][target].emplace_back(id);
        }
    }

    DEFAULT_CONSTRUCTORS(collected_clauses_binary)
};

#endif //KNOBAB_SERVER_PATTERNMININGCONFIGURATION_H
