//
// Created by giacomo on 18/11/22.
//

#ifndef KNOBAB_SERVER_STEP2MINING_H
#define KNOBAB_SERVER_STEP2MINING_H

#include "knobab/mining/PatternMiningConfiguration.h"
#include <knobab/server/query_manager/Environment.h>


/// TODO: case switch on the function that needs to be called for refining the model
enum Step2Mining {
    PER_TRACE_MAXSAT,
    PER_CLAUSE_METRIC,
    INCONSISTENCY_DRIVEN,
    REWRITING,
    SIMPLIFY,
    NON_VACUITY,
    No2Step
};

std::vector<ConjunctiveModelViewForMining> None2Step(ConjunctiveModelViewForMining& model);

/**
 * This function generates from model multiple possible models by grouping those by clusters of minimal models satisfied
 * between traces. Among those, the first being returned is the greatest set of clauses satisfied among clauses
 *
 * @param plans     Query plan, for performing any query
 * @param model     Model to be queried
 * @param log       Log over which perforing the queries
 * @return          Set of QueryPlans associated to it
 */
std::vector<ConjunctiveModelViewForMining> PerTraceMaxSat(std::unordered_map<std::string, LTLfQuery>* plans,
                                                          ConjunctiveModelViewForMining& model,
                                                          Environment& log);

/**
 * This function generates multiple possible models, where each model has the same Support/Confidence value. The models
 * are then returned by decreasing scoring function
 * @param plans     Query plan, for performing any query
 * @param model     Model to be queried
 * @param log       Log over which perforing the queries
 * @param dpcm      Metric function to be exploited
 * @return          Set of QueryPlans associated to it
 */
std::vector<ConjunctiveModelViewForMining> PerClauseMetric(std::unordered_map<std::string, LTLfQuery>* plans,
                                                           ConjunctiveModelViewForMining& model,
                                                           Environment& log,
                                                           PerClauseMethods dpcm);

/**
 * This function returns a subset of the model containing only the clauses that were not vacuously satisfied at least
 * once, thus allowing to test the condition at most once within the dataset
 *
 * @param plans     Query plan, for performing any query
 * @param model     Model to be queried
 * @param log       Log over which perforing the queries
 * @return          Set of QueryPlans associated to it
 */
std::vector<ConjunctiveModelViewForMining> NonVacuity(std::unordered_map<std::string, LTLfQuery>* plans,
                                                           ConjunctiveModelViewForMining& model,
                                                           Environment& log);

/// TODO: rewriting
/// TODO: simplify
/// TODO: InconsistencyDriven

#endif //KNOBAB_SERVER_STEP2MINING_H
