//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_MAXSATPIPELINE_H
#define KNOBAB_SERVER_MAXSATPIPELINE_H

#include <yaucl/numeric/ssize_t.h>
#include <knobab/server/tables/KnowledgeBase.h>
#include <knobab/server/declare/CNFDeclareDataAware.h>
#include <knobab/server/algorithms/atomization/AtomizingPipeline.h>
#include "DataQuery.h"
#include <knobab/server/algorithms/querymanager/LTLfQueryManager.h>
#include "knobab/algorithms/parallelization/scheduling_types.h"
#include "LTLfQuery.h"
//#include <knobab/queries/DeclareQueryLanguageParser.h>
struct LTLfQueryManager;

//#define MAXSatPipeline_PARALLEL

#ifdef MAXSatPipeline_PARALLEL
#include <BS_thread_pool_light.hpp>
#define PARALLELIZE_LOOP_BEGIN(pool,schedulingType,blocks,data_accessing,f)    do { schedule(pool, schedulingType, blocks, data_accessing, f,[&](const std::vector<size_t>& idx) {for (size_t i: idx) {
#define PARALLELIZE_LOOP_END                                                   }}); (pool).wait_for_tasks(); } while(false);
#else
#define PARALLELIZE_LOOP_BEGIN(pool,schedulingType,blocks,data_accessing,f)    for (size_t i = 0, N = (data_accessing).size();i<N; i++) {
#define PARALLELIZE_LOOP_END                                      } while(0);
#endif

enum EnsembleMethods {
    PerDeclareSupport,
    PerDeclareConfidence,
    TraceMaximumSatisfiability,
    TraceIntersection,
    Nothing
};

enum OperatorQueryPlan {
    AbidingLogic,
    FastOperator_v1,
    Hybrid,
    NoQueryRunning
};

struct MAXSatPipeline {
    LTLfQueryManager qm;

#ifdef MAXSatPipeline_PARALLEL
    // A global thread pool object, automatically determining the threads with the number of the architecture ones
    BS::thread_pool_light pool;
    scheduling_type schedulingType;
    size_t blocks;
#endif

    // Input
    double declare_to_ltlf_time = 0.0;
    double ltlf_query_time = 0.0;
    //DeclareQueryLanguageParser dqlp;
    std::unordered_map<std::string, LTLfQuery>* xtLTLfTemplates = nullptr;
    std::vector<LTLfQuery*> declare_to_query;

    CNFDeclareDataAware* declare_model = nullptr;

    //std::unordered_map<declare_templates, ltlf> ltlf_semantics;
    std::unordered_map<std::string , std::vector<LTLfQuery*>> atomToFormulaId;
    size_t maxFormulaId = 0;
    //std::vector<LTLfQuery*> fomulaidToFormula;

    MAXSatPipeline(std::unordered_map<std::string, LTLfQuery>* ptrx,
                   size_t nThreads,
                   scheduling_type schedulingType,
                   size_t blocks);


     ~MAXSatPipeline() {
        clear();
    }

#ifdef MAXSatPipeline_PARALLEL
    MAXSatPipeline() : MAXSatPipeline{"", "", 1, BLOCK_STATIC_SCHEDULE, 1} {}
    MAXSatPipeline(const MAXSatPipeline& x) : qm{x.qm}, pool{pool.get_thread_count()}, declare_to_ltlf_time{x.declare_to_ltlf_time},
                                              ltlf_query_time{x.ltlf_query_time}, dqlp{x.dqlp}, //ptr{x.ptr},
                                              declare_to_query{x.declare_to_query}, atomToFormulaId{x.atomToFormulaId},
                                              maxFormulaId{x.maxFormulaId}, //fomulaidToFormula{x.fomulaidToFormula},
                                              final_ensemble{x.final_ensemble}, operators{x.operators}, support_per_declare{x.support_per_declare},
                                              max_sat_per_trace{x.max_sat_per_trace}, //maxPartialResultId{x.maxPartialResultId},
                                              data_offset{x.data_offset}, data_accessing{x.data_accessing}, data_accessing_range_query_to_offsets{x.data_accessing_range_query_to_offsets},
                                              declare_atomization{x.declare_atomization}, atomToResultOffset{x.atomToResultOffset}, toUseAtoms{x.toUseAtoms}, //barrier_to_range_queries{x.barrier_to_range_queries}, barriers_to_atfo{x.barriers_to_atfo},
                                              atomicPartIntersectionResult{x.atomicPartIntersectionResult}, schedulingType{x.schedulingType}, blocks{x.blocks} {

    }
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
    ///ssize_t maxPartialResultId = -1;
    std::unordered_map<DataQuery, size_t> data_offset;
    std::vector<std::pair<DataQuery, PartialResult>> data_accessing;
    std::unordered_map<std::string, std::unordered_map<std::string,std::vector<size_t>>> data_accessing_range_query_to_offsets;
    std::unordered_map<DeclareDataAware, size_t> declare_atomization;

    /// This other field, on the other hand, is required to speed-up the intersection between the data intervals
    std::vector<std::set<size_t>> atomToResultOffset;

    std::unordered_map<std::string, size_t> toUseAtoms; // This is to ensure the insertion of unique elements to the map!
    //size_t barrier_to_range_queries, barriers_to_atfo;
    std::vector<std::vector<std::pair<std::pair<trace_t, event_t>, double>>> atomicPartIntersectionResult;

    void pipeline(CNFDeclareDataAware* model,
                  const AtomizingPipeline& atomization,
                  const KnowledgeBase& kb);

    void clear();
    std::string generateGraph() const;
    size_t pushNonRangeQuery(const DataQuery &q, bool directlyFromCache = true);
    void pushDataRangeQuery(const LTLfQuery* query, const AtomizingPipeline &atomization, const std::string &atom);

    std::string generateJSONGraph() const { return qm.generateJSONGraph(); }

private:
    void data_chunk(CNFDeclareDataAware* model, const AtomizingPipeline& atomization, const KnowledgeBase& kb);
    std::vector<PartialResult> subqueriesRunning(const KnowledgeBase &kb);
    void abidinglogic_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb);
    void fast_v1_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb);
    void hybrid_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb);
//    LTLfQuery *pushAtomicQueries(const AtomizingPipeline &atomization, LTLfQuery *formula);

};


#endif //KNOBAB_MAXSATPIPELINE_H
