//
// Created by giacomo on 18/11/22.
//

#include "Step2Mining.h"

//void moveToTrivialView(ConjunctiveModelViewForMining& source, ConjunctiveModelView& target) {
//    target.original_data = &source.original_data->model;
//    target.indices.swap(source.indices);
//}
//
//void nullifyTargetView(ConjunctiveModelViewForMining& source, ConjunctiveModelView& target) {
//    target.original_data = &source.original_data->model;
//    target.indices.swap(source.indices);
//}

std::vector<ConjunctiveModelViewForMining> None2Step(ConjunctiveModelViewForMining& model) {
    return {model};
}

std::vector<ConjunctiveModelViewForMining> PerTraceMaxSat(std::unordered_map<std::string, LTLfQuery>* plans,
                                                          ConjunctiveModelViewForMining& model,
                                                          Environment& log) {
    // Running the MaxSatQuery over the log
    log.set_maxsat_parameters(1, TracesToClauses, Hybrid);
    std::vector<ConjunctiveModelViewForMining> R;

    // TODO: set a proper muiltithreaded query plan, while extending the syntax with block, scheduling type, and number of threads
    MAXSatPipeline ref(plans, 1, BLOCK_STATIC_SCHEDULE, 3);
    ref.final_ensemble = TracesToClauses;
    ref.operators = Hybrid;
    std::vector<std::set<size_t>> setOfSets;
    {
//        moveToTrivialView(model, tmp);
        ref.pipeline(model, log.ap, log.db);
//        nullifyTargetView(model, tmp);
    }
    for (const auto& clausesId : ref.traces_to_clauses) {
        std::set<size_t> s{clausesId.begin(), clausesId.end()};
        setOfSets.emplace_back(s);
    }
    remove_duplicates(setOfSets);
    auto S = partition_sets(setOfSets).minimal_common_subsets;
    if (!S.empty()) {
        auto solution = model.resolveIndices();
        for (const auto& result :
                partition_sets(setOfSets).minimal_common_subsets) {
            std::list<size_t> ls;
            for (size_t x : result)
                ls.emplace_back(solution.at(x));
            R.emplace_back(model.original_data, ls);
        }
    }
    // Preferring the largest models for better coverage
    std::sort(R.begin(), R.end(), [](const ConjunctiveModelViewForMining& l,
                                     const ConjunctiveModelViewForMining& r){
        return l.indices.size() > r.indices.size();
    });
    return R;
}

std::vector<ConjunctiveModelViewForMining> PerClauseMetric(std::unordered_map<std::string, LTLfQuery>* plans,
                                                          ConjunctiveModelViewForMining& model,
                                                          Environment& log,
                                                          PerClauseMethods dpcm) {
    // Running the MaxSatQuery over the log
    EnsembleMethods em;
    switch (dpcm) {
        case PerClauseSupport:
            em = PerDeclareSupport;
            break;
        case PerClauseConfidence:
            em = PerDeclareConfidence;
            break;
    }
    log.set_maxsat_parameters(1, em, Hybrid);
    std::vector<ConjunctiveModelViewForMining> R;

    // TODO: set a proper muiltithreaded query plan, while extending the syntax with block, scheduling type, and number of threads
    MAXSatPipeline ref(plans, 1, BLOCK_STATIC_SCHEDULE, 3);
    ref.final_ensemble = em;
    ref.operators = Hybrid;
    {
//        ConjunctiveModelView tmp;
//        moveToTrivialView(model, tmp);
        ref.pipeline(model, log.ap, log.db);
//        nullifyTargetView(model, tmp);
    }
    std::map<double, std::vector<size_t>> objToScore;
    for (size_t i = 0, N = ref.support_per_declare.size(); i<N; i++) {
        objToScore[ref.support_per_declare.at(i)].emplace_back(i);
    }
    if (!objToScore.empty()) {
        auto solution = model.resolveIndices();
        for (auto iter = objToScore.rbegin(); iter != objToScore.rend(); ++iter) {
            std::list<size_t> ls;
            for (size_t x : iter->second)
                ls.emplace_back(solution.at(x));
            R.emplace_back(model.original_data, ls);
        }
    }
    return R;
}

std::vector<ConjunctiveModelViewForMining> NonVacuity(std::unordered_map<std::string, LTLfQuery>* plans,
                                                          ConjunctiveModelViewForMining& model,
                                                          Environment& log) {
    log.set_maxsat_parameters(1, NonVacuouslySatisfied, Hybrid);
    std::vector<ConjunctiveModelViewForMining> R;
    MAXSatPipeline ref(plans, 1, BLOCK_STATIC_SCHEDULE, 3);
    ref.final_ensemble = NonVacuouslySatisfied;
    ref.operators = Hybrid;
    {
//        ConjunctiveModelView tmp;
//        moveToTrivialView(model, tmp);
        ref.pipeline(model, log.ap, log.db);
//        nullifyTargetView(model, tmp);
    }
    if (!ref.activated_clauses.empty()) {
        auto solution = model.resolveIndices();
        std::list<size_t> ls;
        for (size_t x : ref.activated_clauses)
            ls.emplace_back(solution.at(x));
        R.emplace_back(model.original_data, ls);
    }
    return R;
}