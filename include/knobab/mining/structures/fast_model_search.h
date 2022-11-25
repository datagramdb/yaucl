//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_FAST_MODEL_SEARCH_H
#define KNOBAB_SERVER_FAST_MODEL_SEARCH_H

#include <vector>
#include <knobab/server/declare/DeclareDataAware.h>
#include <yaucl/bpm/structures/commons.h>

struct collected_clauses_unary {
    umap<clause_name, umap<activation_label, map<N_argument,std::vector<clause_id>>>>                        unary_activation_dataless;
    umap<clause_name, umap<activation_label, keysetmap<std::string, map<N_argument,std::vector<clause_id>>>>> unary_activation_withdata;
    DEFAULT_CONSTRUCTORS(collected_clauses_unary)
};

struct collected_clauses_binary {
    umap<clause_name, umap<activation_label, std::vector<clause_id>>>                                                     activation_dataless;///
    umap<clause_name, umap<activation_label, dataconditions_to_ids>>                     activation_withdata;///
    umap<clause_name, umap<activation_label, keysetmap<std::string, umap<target_label, std::vector<clause_id>>>>> no_targ_data;
    umap<clause_name, umap<activation_label, keysetmap<std::string, umap<target_label, dataconditions_to_ids>>>> targ_data;
    umap<clause_name, umap<activation_label, umap<target_label, std::vector<clause_id>>>>                                 dataless_binary_clause;///
    umap<clause_name, umap<activation_label, umap<target_label, dataconditions_to_ids>>>                                 dataless_binary_clausev2;///

    umap<clause_name, umap<target_label, std::vector<clause_id>>> by_targetlabel;///
    umap<clause_name, umap<target_label, dataconditions_to_ids>> by_targetlabel_and_atomisation;///
    DEFAULT_CONSTRUCTORS(collected_clauses_binary)
};

/**
 * The actual <Data> representation of the model to perform fast operations!
 */
struct fast_model_search {
    struct collected_clauses_unary                        unary_clauses;
    struct collected_clauses_binary                       binary_clauses;
    std::vector<pattern_mining_result<DeclareDataAware>>  model;
    size_t                                                id = 0;

    size_t size() const { return id; }
    const pattern_mining_result<DeclareDataAware>& at(const size_t i) const { return model.at(i); }
    pattern_mining_result<DeclareDataAware>& operator[](const size_t i) { return model[i]; }
    /// Constructor initialising the model with the conjunctive one
    fast_model_search(const std::vector<pattern_mining_result<DeclareDataAware>> & M) : model{M} { for (const auto& clause : M) { emplace_back(clause); } }

    /**
     * This query system also support a pattern matching, where some of the parts of the model might be missing.
     * Under the assumption that the query is as a declarative clause should be, the exact match is also returned.
     *
     * If num,left,right is set to a different value, then we perform a query over the specific setting
     */
    std::vector<size_t> exists(const DeclareDataAware& query,
                               QueryRelation num = QR_EQ,
                               QueryRelation left_atoms = QR_EQ,
                               QueryRelation right_atoms = QR_EQ) const ;
    size_t emplace_back(const pattern_mining_result<DeclareDataAware>& clause);

    DEFAULT_CONSTRUCTORS(fast_model_search)
};


#endif //KNOBAB_SERVER_FAST_MODEL_SEARCH_H
