//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_MAXSATPIPELINE_H
#define KNOBAB_SERVER_MAXSATPIPELINE_H


#include <knobab/server/tables/KnowledgeBase.h>
#include <yaucl/numeric/ssize_t.h>
#include <yaucl/numeric/ssize_t.h>
#include <knobab/server/declare/CNFDeclareDataAware.h>
#include <knobab/server/algorithms/atomization/AtomizingPipeline.h>
#include <knobab/server/operators/LTLfQuery.h>
#include "LTLfQueryManager.h"

//#define MAXSatPipeline_PARALLEL

#ifdef MAXSatPipeline_PARALLEL
#include <thread_pool.hpp>
#define PARALLELIZE_LOOP_BEGIN(pool, lower, upper, varA, varB)    (pool).parallelize_loop((lower), (upper), [&](const size_t varA, const size_t varB) {
#define PARALLELIZE_LOOP_END                                      });
#else
#define PARALLELIZE_LOOP_BEGIN(pool, lower, upper, varA, varB)    do { auto varA = (lower); auto varB = (upper);
#define PARALLELIZE_LOOP_END                                      } while(0);
#endif

enum EnsembleMethods {
    PerDeclareSupport,
    TraceMaximumSatisfiability,
    TraceIntersection,
    Nothing
};

enum OperatorQueryPlan {
    AbidingLogic,
    FastOperator_v1,
    NoQueryRunning
};

struct MAXSatPipeline {
    LTLfQueryManager qm;

#ifdef MAXSatPipeline_PARALLEL
    // A global thread pool object, automatically determining the threads with the number of the architecture ones
    thread_pool pool;
#endif

    // Input
    double declare_to_ltlf_time = 0.0;
    double ltlf_query_time = 0.0;
    std::unordered_map<std::string, LTLfQuery>* ptr = nullptr;
    std::vector<LTLfQuery*> declare_to_query;

    CNFDeclareDataAware* declare_model = nullptr;

    //std::unordered_map<declare_templates, ltlf> ltlf_semantics;
    std::unordered_map<std::string , std::vector<size_t>> atomToFormulaId;
    size_t maxFormulaId = 0;
    std::vector<LTLfQuery*> fomulaidToFormula;

    MAXSatPipeline(std::unordered_map<std::string, LTLfQuery>* ptr, size_t nThreads);


#ifdef MAXSatPipeline_PARALLEL
    MAXSatPipeline() : MAXSatPipeline{"", "", 1} {}
    MAXSatPipeline(const MAXSatPipeline& x) : qm{x.qm}, pool{1}, declare_to_ltlf_time{x.declare_to_ltlf_time},
                                              ltlf_query_time{x.ltlf_query_time}, dqlp{x.dqlp}, ptr{x.ptr},
                                              declare_to_query{x.declare_to_query}, atomToFormulaId{x.atomToFormulaId},
                                              maxFormulaId{x.maxFormulaId}, fomulaidToFormula{x.fomulaidToFormula},
                                              final_ensemble{x.final_ensemble}, operators{x.operators}, support_per_declare{x.support_per_declare},
                                              max_sat_per_trace{x.max_sat_per_trace}, maxPartialResultId{x.maxPartialResultId},
                                              data_offset{x.data_offset}, data_accessing{x.data_accessing}, data_accessing_range_query_to_offsets{x.data_accessing_range_query_to_offsets},
                                              declare_atomization{x.declare_atomization}, atomToResultOffset{x.atomToResultOffset}, toUseAtoms{x.toUseAtoms},
                                              barrier_to_range_queries{x.barrier_to_range_queries}, barriers_to_atfo{x.barriers_to_atfo}, atomicPartIntersectionResult{x.atomicPartIntersectionResult} {}
    MAXSatPipeline& operator=(const MAXSatPipeline&) = default;
#else
    DEFAULT_COPY_ASSGN(MAXSatPipeline)
#endif

    EnsembleMethods final_ensemble;
    OperatorQueryPlan operators;

    // Ensemble methods' results
    Result result;
    std::vector<double> support_per_declare;
    std::vector<double> max_sat_per_trace;

    // DATA
    ssize_t maxPartialResultId = -1;
    std::unordered_map<DataQuery, size_t> data_offset;
    std::vector<std::pair<DataQuery, std::vector<std::pair<std::pair<trace_t, event_t>, double>>>> data_accessing;
    std::unordered_map<std::string, std::unordered_map<std::string,std::vector<size_t>>> data_accessing_range_query_to_offsets;
    std::unordered_map<DeclareDataAware, size_t> declare_atomization;
    std::vector<std::set<size_t>> atomToResultOffset;
    std::vector<std::string> toUseAtoms; // This is to ensure the insertion of unique elements to the map!
    size_t barrier_to_range_queries, barriers_to_atfo;
    std::vector<std::vector<std::pair<std::pair<trace_t, event_t>, double>>> atomicPartIntersectionResult;

    void pipeline(CNFDeclareDataAware* model,
                  const AtomizingPipeline& atomization,
                  const KnowledgeBase& kb);

    void clear();


    std::string generateGraph() const { return qm.generateGraph(); }

private:
    void data_chunk(CNFDeclareDataAware* model, const AtomizingPipeline& atomization, const KnowledgeBase& kb);
    std::vector<PartialResult> subqueriesRunning(const KnowledgeBase &kb);
    void abidinglogic_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb);
    void fast_v1_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb);
    size_t pushAtomDataQuery(const DataQuery &q, bool directlyFromCache);
    LTLfQuery *pushAtomicQueries(const AtomizingPipeline &atomization, LTLfQuery *formula);
};



#endif //KNOBAB_SERVER_MAXSATPIPELINE_H
