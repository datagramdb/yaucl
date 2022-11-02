//
// Created by giacomo on 16/04/2022.
//

#include <ios>
#include <magic_enum.hpp>
#include <knobab/server/operators/simple_ltlf_operators.h>
#include <yaucl/functional/assert.h>
#include <knobab/server/operators/fast_ltlf_operators.h>
#include <knobab/server/algorithms/querymanager/MAXSatPipeline.h>

MAXSatPipeline::MAXSatPipeline(std::unordered_map<std::string, LTLfQuery>* ptrx,
                               size_t nThreads,
                               scheduling_type schedulingType,
                               size_t blocks)
#ifdef MAXSatPipeline_PARALLEL
    : pool(nThreads),
      schedulingType{schedulingType}, blocks{blocks}
#endif
{
    xtLTLfTemplates = ptrx;
 // Equivalent to the old expansion of the declare formula into LTLF, and then into the negated normal form.
    /*   std::ifstream file{plan_file};
    dqlp.parse(file);
    auto it = dqlp.planname_to_declare_to_ltlf.find(plan);
    if (it == dqlp.planname_to_declare_to_ltlf.end()) {
        throw std::runtime_error(plan+": the plan is missing from the script file!");
    }
    xtLTLfTemplates = &it->second;*/
}

void MAXSatPipeline::clear() {
    std::unordered_set<LTLfQuery*> visited;

    /// TODO:
    /// If this worked for the current standard:
    /// for (auto & it : std::ranges::reverse_view(qm.Q))
    for(std::map<size_t, std::vector<LTLfQuery*>>::reverse_iterator it = qm.Q.rbegin(); it != qm.Q.rend(); ++it) {
        for(LTLfQuery* ptr : it->second) {
            if(visited.insert(ptr).second) {
                for(LTLfQuery* arg : ptr->args){
                    if(visited.insert(arg).second){
                        delete arg;
                    }
                }
                delete ptr;
            }
        }
    }

    atomToFormulaId.clear();
    declare_to_query.clear();
    data_accessing.clear();
    declare_atomization.clear();
    atomToResultOffset.clear();
    toUseAtoms.clear();
    atomicPartIntersectionResult.clear();
    result.clear();
    support_per_declare.clear();
}

static inline void partialResultIntersection(const PartialResult& lhs,
                                             const PartialResult& rhs,
                                             std::back_insert_iterator<PartialResult> d_first) {
    if (lhs.empty() || rhs.empty()) return;
    env e1, e2;
    auto first1 = lhs.begin(), last1 = lhs.end(), first2 = rhs.begin(), last2 = rhs.end();
    std::pair<uint32_t, uint16_t> pair, pair1;
    for (; first1 != last1; ++d_first) {
        if (first2 == last2) {
            return;
        }
        if (first1->first > first2->first) {
            *first2++;
        } else if (first1->first < first2->first) {
            *first1++;
        } else {
            pair.first = first1->first.first;
            pair1.first = first2->first.first;
            *d_first = std::make_pair(first1->first, std::max(first1->second, first2->second));
            first1++;
            first2++;
        }
    }
}

static inline
std::vector<std::pair<std::pair<trace_t, event_t>, double>> partialResultIntersection(const std::set<size_t> &vecs,
                                                                                      const std::vector<std::pair<DataQuery, std::vector<std::pair<std::pair<trace_t, event_t>, double>>>>& results) {
    if (vecs.empty()) return {};
    auto it = vecs.begin();
    auto last_intersection = results.at(*it).second;
    std::vector<std::pair<std::pair<trace_t, event_t>, double>> curr_intersection;
    for (std::size_t i = 1; i < vecs.size(); ++i) {
        it++;
        auto ref = results.at(*it).second;
        partialResultIntersection(last_intersection,
                                  ref,
                                  std::back_inserter(curr_intersection));
        std::swap(last_intersection, curr_intersection);
        curr_intersection.clear();
    }
    return last_intersection;
}

static inline PartialResult partialResultIntersection(const std::set<size_t> &vecs,
                                                      const std::vector<PartialResult>& results) {
    if (vecs.empty()) return {};
    auto it = vecs.begin();
    auto last_intersection = results.at(*it);
    PartialResult curr_intersection;
    for (std::size_t i = 1; i < vecs.size(); ++i) {
        it++;
        auto& ref = results.at(*it);
        partialResultIntersection(last_intersection,
                                  ref,
                                  std::back_inserter(curr_intersection));
        std::swap(last_intersection, curr_intersection);
        curr_intersection.clear();
    }

    return last_intersection;
}




void MAXSatPipeline::data_chunk(CNFDeclareDataAware *model,
                                const AtomizingPipeline& atomization,
                                const KnowledgeBase& kb) {

    if (!model) return;
    qm.current_query_id = 0;
    qm.pipeline = this;
    declare_model = model;
    std::vector<LTLfQuery*> W;
    //label_set_t visitedAtoms;
//    size_t declareId = 0;
    std::vector<LTLfQuery> tmpQuery;
    std::vector<size_t> declareToQuery;

    /// Instantiating the patterns as distinct trees before generating the graph.
    /// TODO: pattern instantiation and DAG generation in one shot
    for (auto& it : declare_model->singleElementOfConjunction) {
        for (auto& item : it.elementsInDisjunction) {
            // Skipping already-met definitions: those will only duplicate the code to be run!
//            declareId++;
            auto cp = declare_atomization.emplace(item, maxFormulaId);
            if (!cp.second) {
                declareToQuery.emplace_back(cp.first->second);
                /// TODO: PRESERVE

                continue;
            } else {
                declareToQuery.emplace_back(maxFormulaId);
            }

            // Setting the knowledge base, so to exploit it for join queries
            item.kb = &kb;

            // Getting the definition of Declare from the Query rewriter
            auto it2 = xtLTLfTemplates->find(item.casusu);
            if (it2 == xtLTLfTemplates->end()) {
                throw std::runtime_error(item.casusu+": missing from the loaded query decomposition");
            }
            item.flipLocal(); // ensuring that the inverse predicate is always computed, so that I can pass it as a const!

            // Caching the query, so to generate a pointer to an experssion that was already computed.
            // The query plan manager will identfy the common expressions, and will let represent those only ones
            // via caching and mapping.
            tmpQuery.emplace_back(qm.instantiate(atomization.act_atoms,
                           maxFormulaId,
                           it2->second,
                           item.isTruth() ? nullptr : (const DeclareDataAware *) &item,
                           atomization.data_query_atoms,
                           atomization.atom_universe,
                           item.left_decomposed_atoms,
                           item.right_decomposed_atoms));

            maxFormulaId++;
            qm.current_query_id++;
        }
    }
    std::vector<bool> WECTOR(declareToQuery.size(), false);

    qm.finalizeUnions();
    for (size_t i = 0, N = declareToQuery.size(); i<N; i++) {
        auto qId = declareToQuery.at(i);
        if (!WECTOR.at(qId)) {
            WECTOR[qId] = true;
            qm.activations.emplace_back();
            LTLfQuery* formula = qm.alloc(tmpQuery.at(qId), i);
            formula->isTop = true;

            /// Setting specific untimed atom queries, that can be run directly and separatedly
            /// So, any expansion of the formula by detecting whether the formula can directly
            /// access the tables or not should be done in a later stage, that is, on simplify!
            W.emplace_back(formula);
            declare_to_query.emplace_back(formula);
        } else {
            declare_to_query.emplace_back(declare_to_query.at(qId));
            qm.activations.emplace_back(qm.activations.at(qId));
        }
    }

    // Sorting the operators, so that the query can be run from the ending nodes.
    // Finalising the data structure representation
    qm.sort_query_plan_for_scheduling(atomization, W, (KnowledgeBase *) &kb); // Time Computational Complexity: Squared on the size of the atoms

    //remove_duplicates(toUseAtoms);
    for (auto& ref : atomToFormulaId)
        remove_duplicates(ref.second);
}

void MAXSatPipeline::pushDataRangeQuery(const LTLfQuery* query,
                                        const AtomizingPipeline &atomization,
                                        const std::string &atom) {
    // Remembers the formula-atom association, so that the final atom ID can be performed after the intersection
    // decomposition part of the pipeline
    static std::vector<std::pair<std::pair<trace_t, event_t>, double>> empty_result{};
    atomToFormulaId[atom].emplace_back((LTLfQuery*)query);
    if (!toUseAtoms.emplace(atom, toUseAtoms.size()).second) return;

    // CHECKING THAT THE ATOM IS WITHIN THE ATOMIZATION PIPELINE
    auto interval_to_interval_queries_to_intersect = atomization.atom_to_conjunctedPredicates.find(atom);
    if (interval_to_interval_queries_to_intersect == atomization.atom_to_conjunctedPredicates.end())
        throw std::runtime_error(atom + ": element not in map");

    std::vector<size_t> tmpRanges;
    for (const auto& interval_data_query : interval_to_interval_queries_to_intersect->second) {
        // Sanity Checks
        DEBUG_ASSERT(interval_data_query.casusu == INTERVAL);
        DEBUG_ASSERT(interval_data_query.BiVariableConditions.empty());

        //       splits the RangeQuery by clause.var (as they are going to be on different tables),
        //       clause.label (as the ranges are ordered by act and then by value) and then sort
        //       the remainder by interval. By doing so, we are going to pay the access to clause.label
        //       only once, and we are going to scan the whole range at most linearly instead of performing
        //       at most linear scans for each predicate.
        //       Then, the access canfomulaidToFormula be parallelized by variable name, as they are going to query separated tables
        auto q = DataQuery::RangeQuery(interval_data_query.label, interval_data_query.var, interval_data_query.value, interval_data_query.value_upper_bound);
        size_t tmpDataAccessingSize =  data_accessing.size();
        auto find = data_offset.emplace(q, tmpDataAccessingSize);
        if (find.second){
            tmpRanges.emplace_back(tmpDataAccessingSize);
            data_accessing_range_query_to_offsets[interval_data_query.var][interval_data_query.label].emplace_back(tmpDataAccessingSize);
            data_accessing.emplace_back(q, empty_result);
        } else {
            tmpRanges.emplace_back(find.first->second);
        }
    }

    atomToResultOffset.emplace_back(tmpRanges.begin(), tmpRanges.end());
//    return tmpRanges;
}

size_t MAXSatPipeline::pushNonRangeQuery(const DataQuery &q, bool directlyFromCache) {
    static std::vector<std::pair<std::pair<trace_t, event_t>, double>> empty_result{};
    size_t offset = data_accessing.size();
    auto find = data_offset.emplace(q, offset); // Determining whether the query already exists
    if (find.second){
        // Inserting the novel atomic query
        offset = data_accessing.size();
        data_accessing.emplace_back(q, empty_result);
    } else
        offset = (find.first->second); // Otherwise, taking the offset already being computed before
    if (!directlyFromCache) {
        DEBUG_ASSERT(false);
    }
    return offset;
}

std::vector<PartialResult> MAXSatPipeline::subqueriesRunning(const KnowledgeBase &kb) {
    // 1. Performing the query over each single predicate that we have extracted, so not to duplicate the data access
    PARALLELIZE_LOOP_BEGIN(pool,schedulingType,blocks,data_accessing,[](auto& x ){return 1;})
        auto &ref = data_accessing.at(i);
        // TODO: Given the query in ref.first, put the Result in ref.second
        switch (ref.first.type) {
            case ExistsQuery:
                ref.second
                    = kb.untimed_dataless_exists(kb.resolveCountingData(ref.first.label),
                                                                 ref.first.numeric_argument);
                break;
            case AtomQuery:
                ref.second = kb.timed_dataless_exists(ref.first.label);
                break;
            case AbsenceQuery:
                ref.second = kb.untimed_dataless_absence(kb.resolveCountingData(ref.first.label),
                                                                     ref.first.numeric_argument);
                break;
            case InitQuery:
                ref.second = kb.initOrig(ref.first.label);
                break;
           case EndsQuery:
                ref.second = kb.endsOrig(ref.first.label);
                break;
           case FirstQuery:
                ref.second = kb.getFirstLastOtherwise(true);
                break;
           case LastQuery:
                ref.second = kb.getFirstLastOtherwise(false);
                break;

           default:
                continue; // Skipping the other data range queries for a further parallelization task
                            ///DEBUG_ASSERT(false); // This should be dealt in (B)
        }
    PARALLELIZE_LOOP_END


    // 2. Performing the queries over the range queries
    if (!data_accessing_range_query_to_offsets.empty()) {
#ifdef MAXSatPipeline_PARALLEL
        // If this is a parallel execution, I can exploit the current library only if I have a vector. Otherwise,
        // the map data structures are not safe for iteration

        for (auto& rangeQueryRefs : data_accessing_range_query_to_offsets) {
            pool.push_task([kb,this](auto& x) {
                kb.exact_range_query(x.first, x.second, data_accessing);
            }, rangeQueryRefs);
        };
        pool.wait_for_tasks();
#else
        // If this is not in parallel mode, then there is no purpose to create a temporary vector for the parallelization:
        // therefore, I can directly iterate over the map.
        for (auto& rangeQueryRefs : data_accessing_range_query_to_offsets) {
            kb.exact_range_query(rangeQueryRefs.first, rangeQueryRefs.second, data_accessing);
        }
#endif
    }

    // This other solution avoids this intermediate computation! I.e., the thing that follows.

    // After accessing the data, we perform the interval intersection between these,
    // without the need to express this part on the DAG, explicitely. This saves time,
    // as with smaller data the graph traversal dominates the computational complexity
    // of the same given operations.
    // The computational complexity should be squared on the size of the atoms
    auto set_decomposition_result = partition_sets(atomToResultOffset);
    ///std::cout << set_decomposition_result << std::endl;
    size_t isFromFurtherDecomposition = set_decomposition_result.minimal_common_subsets.size();

    // result.minimal_common_subsets.size() + result.minimal_common_subsets_composition.size()
    std::vector<PartialResult> resultOfS(set_decomposition_result.minimal_common_subsets.size() + set_decomposition_result.minimal_common_subsets_composition.size());

    PARALLELIZE_LOOP_BEGIN(pool, schedulingType  ,blocks,set_decomposition_result.minimal_common_subsets,[](auto& x ){return 1;})
        auto& S = set_decomposition_result.minimal_common_subsets.at(i);
        resultOfS[i] = partialResultIntersection(S, data_accessing);
    PARALLELIZE_LOOP_END

//    PARALLELIZE_LOOP_BEGIN(pool, 0, set_decomposition_result.minimal_common_subsets_composition.size(), lb, ub)
//        for (size_t i = lb; i < ub; i++) {
//            auto& S = set_decomposition_result.minimal_common_subsets_composition.at(i);
//            // Perform the intersection among all of the elements in S,
//            // using the intermediate results from resultOfSSecond
//            resultOfS[isFromFurtherDecomposition + i] = partialResultIntersection(S, resultOfS);
////            for (const auto x : S) {
////                edges.emplace("resultOfS:"+std::to_string(x), "resultOfS:"+std::to_string(i));
////            }
//        }
//            PARALLELIZE_LOOP_END
//    for (size_t i = 0; i<resultOfS.size(); i++) {
//        std::cout << i << "->>->" << resultOfS.at(i) << std::endl;
//    }
    ///results_cache.resize(toUseAtoms.size());

    std::vector<PartialResult> results_cache(atomToResultOffset.size(), PartialResult{});
    PARALLELIZE_LOOP_BEGIN(pool, schedulingType  ,blocks,set_decomposition_result.decomposedIndexedSubsets,[](auto& x ){return 1;})
        auto& ref = set_decomposition_result.decomposedIndexedSubsets.at(i);
        results_cache[ref.first] = partialResultIntersection(*ref.second, resultOfS);
    PARALLELIZE_LOOP_END

//                        PARALLELIZE_LOOP_BEGIN(pool, 0, set_decomposition_result.decomposedIndexedSubsets.size(), lb, ub)
//        for (size_t j = lb; j < ub; j++) {
//            auto& ref = set_decomposition_result.decomposedIndexedSubsets.at(j);
////            std::cout << ref.first << "-@->" << *ref.second << std::endl;
//            // put the global intersection into a map representation.
//            // ref->first will correspond to the atom in that same position in toUseAtoms.
//            results_cache[ref.first] = partialResultIntersection(*ref.second, resultOfS);
////            for (const auto x : *ref.second) {
////                edges.emplace("resultOfS:"+std::to_string(x), "results_cache:"+std::to_string(ref.first));
////            }
//        }
//            PARALLELIZE_LOOP_END

    resultOfS.clear();
//    for (size_t i = 0; i<results_cache.size(); i++) {
//        std::cout << i << "-->" << results_cache.at(i) << std::endl;
//    }

    // Preparing the second phase of the pipeline, where the extracted data is going to be combined.
    for (const auto& cp : toUseAtoms) {
        auto& atom = cp.first;
        auto& i = cp.second;
        for (LTLfQuery* formulaId : atomToFormulaId.at(atom)) {
            // Associating the partial results from the cache to the atom of choice (partial_results)
            formulaId->range_query.emplace_back(i);
//            fomulaidToFormula.at(formulaId)->associateDataQueryIdsToFormulaByAtom(atom, i);// setting the partial results for the data pipeline
        }
    }

    return results_cache;

////
////    for (const auto& da : data_accessing) {
////        std::cout << da.first.label << '.' << da.first.var << " \\in [" << std::get<1>(da.first.lower_bound) << ',' << get<1>(da.first.upper_bound) << "] --> " << da.second << std::endl;
////    }
//
//    // After accessing the data, we perform the interval intersection between these
//// The computational complexity should be squared on the size of the atoms
//    auto set_decomposition_result = partition_sets(atomToResultOffset);
//    ///std::cout << set_decomposition_result << std::endl;
//    size_t isFromFurtherDecomposition = set_decomposition_result.minimal_common_subsets.size();
//
//
//    std::vector<PartialResult> resultOfS(set_decomposition_result.minimal_common_subsets.size() + set_decomposition_result.minimal_common_subsets_composition.size());
//    PARALLELIZE_LOOP_BEGIN(pool, 0, set_decomposition_result.minimal_common_subsets.size(), lb, ub)
//        for (size_t i = lb; i < ub; i++) {
//            auto& S = set_decomposition_result.minimal_common_subsets.at(i);
////            std::cout << i << " is " << S << std::endl;
//            // resultOfS for collecting the intermediate computations
//            resultOfS[i] = partialResultIntersection(S, data_accessing);
////            for (const auto x : S) {
////                edges.emplace("data_accessing:"+std::to_string(x), "resultOfS:"+std::to_string(i));
////            }
//        }
//            PARALLELIZE_LOOP_END
//
//    PARALLELIZE_LOOP_BEGIN(pool, 0, set_decomposition_result.minimal_common_subsets_composition.size(), lb, ub)
//        for (size_t i = lb; i < ub; i++) {
//            auto& S = set_decomposition_result.minimal_common_subsets_composition.at(i);
//            // Perform the intersection among all of the elements in S,
//            // using the intermediate results from resultOfSSecond
//            resultOfS[isFromFurtherDecomposition + i] = partialResultIntersection(S, resultOfS);
////            for (const auto x : S) {
////                edges.emplace("resultOfS:"+std::to_string(x), "resultOfS:"+std::to_string(i));
////            }
//        }
//            PARALLELIZE_LOOP_END
//
////    for (size_t i = 0; i<resultOfS.size(); i++) {
////        std::cout << i << "->>->" << resultOfS.at(i) << std::endl;
////    }
//
//    ///results_cache.resize(toUseAtoms.size());
//    std::vector<PartialResult> results_cache(std::max(toUseAtoms.size(), resultOfS.size()), PartialResult{});
//    PARALLELIZE_LOOP_BEGIN(pool, 0, set_decomposition_result.decomposedIndexedSubsets.size(), lb, ub)
//        for (size_t j = lb; j < ub; j++) {
//            auto& ref = set_decomposition_result.decomposedIndexedSubsets.at(j);
////            std::cout << ref.first << "-@->" << *ref.second << std::endl;
//            // put the global intersection into a map representation.
//            // ref->first will correspond to the atom in that same position in toUseAtoms.
//            results_cache[ref.first] = partialResultIntersection(*ref.second, resultOfS);
////            for (const auto x : *ref.second) {
////                edges.emplace("resultOfS:"+std::to_string(x), "results_cache:"+std::to_string(ref.first));
////            }
//        }
//            PARALLELIZE_LOOP_END
//
//    resultOfS.clear();
////    for (size_t i = 0; i<results_cache.size(); i++) {
////        std::cout << i << "-->" << results_cache.at(i) << std::endl;
////    }
//
//    // Preparing the second phase of the pipeline, where the extracted data is going to be combined.
//    for (size_t i = 0, N = toUseAtoms.size(); i < N; i++) {
//        auto& atom = toUseAtoms.at(i);
//        DEBUG_ASSERT(false);
///// TODO:
/////        for (size_t formulaId : atomToFormulaId.at(atom)) {
/////            // Associating the partial results from the cache to the atom of chhoice (partial_results)
/////            fomulaidToFormula.at(formulaId)->associateDataQueryIdsToFormulaByAtom(atom, i);// setting the partial results for the data pipeline
/////        }
//    }

//    return results_cache;
}


static inline void local_logic_intersection(const LTLfQuery* q, Result& last_union, bool isTimed = true) {
    size_t N = q->args.size();
    if ((!q) || (N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = q->args.at(0)->result;
    } else if (N == 2) {
        auto &arg1 = q->args.at(0);
        auto &arg2 = q->args.at(1);
        last_union.clear();
        if (isTimed)
            and_logic_timed(arg1->result,
                            arg2->result,
                            last_union, q->joinCondition);
        else
            and_logic_untimed(arg1->result,
                              arg2->result,
                              last_union, q->joinCondition);
    } else {
        DEBUG_ASSERT(q->joinCondition);
        auto it = q->args.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < N; ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                and_logic_timed(last_union, ref, curr_union);
            else
                and_logic_untimed(last_union, ref, curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}

static inline void local_logic_intersection(const std::unordered_set<LTLfQuery*>& q, Result& last_union, bool isTimed = true) {
    size_t N = q.size();
    if ((N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = (*q.begin())->result;
    } else {
        auto it = q.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < N; ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                and_fast_timed(last_union, ref, curr_union);
            else
                and_fast_untimed(last_union, ref, curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}


static inline void local_logic_union(const std::unordered_set<LTLfQuery*>& q, Result& last_union, bool isTimed = true) {
    size_t N = q.size();
    if ((N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = (*q.begin())->result;
    } else {
        auto it = q.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < N; ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                or_fast_timed(last_union, ref, curr_union);
            else
                or_fast_untimed(last_union, ref, curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}

static inline void local_fast_intersection(const LTLfQuery* q, Result& last_union, bool isTimed = true) {
    size_t N = q->args.size();
    if ((!q) || (N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = q->args.at(0)->result;
    } else if (N == 2) {
        auto &arg1 = q->args.at(0);
        auto &arg2 = q->args.at(1);
        last_union.clear();
        if (isTimed)
            and_fast_timed(arg1->result,
                           arg2->result,
                           last_union, q->joinCondition);
        else
            and_fast_untimed(arg1->result,
                             arg2->result,
                             last_union, q->joinCondition);
    } else {
        //DEBUG_ASSERT(q->joinCondition);
        auto it = q->args.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < N; ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                and_fast_timed(last_union, ref, curr_union);
            else
                and_fast_untimed(last_union, ref, curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}

#include <chrono>

static inline void partialResultUnion(const PartialResult& lhs,
                                      const PartialResult& rhs,
                                      std::back_insert_iterator<PartialResult> d_first) {

    if (lhs.empty() && rhs.empty()) return;
    env e1, e2;
    auto first1 = lhs.begin(), last1 = lhs.end(), first2 = rhs.begin(), last2 = rhs.end();
    std::pair<uint32_t, uint16_t> pair, pair1;

    for (; first1 != last1; ++d_first) {
        if (first2 == last2) {
            std::copy(first1, last1, d_first);
            return;
        }
        if (first1->first > first2->first) {
            *d_first = *first2++;
        } else if (first1->first < first2->first) {
            *d_first = *first1++;
        } else {
            pair.first = first1->first.first;
            pair1.first = first2->first.first;
            *d_first = std::make_pair(first1->first, std::max(first1->second, first2->second));
            first1++;
            first2++;
        }
    }
    std::copy(first2, last2, d_first);
}



//static inline Result local_intersection(const std::set<size_t> &vecs,
//                                        const std::vector<PartialResult>& results,
//                                        LeafType isLeaf) {
//    if (vecs.empty()) return {};
//    PartialResult last_intersection = partialResultIntersection(vecs, results);
//
//    // TODO: better done through views!
//    Result  finalResult;
//    for (auto it = last_intersection.begin(); it != last_intersection.end(); it++) {
//        switch (isLeaf) {
//            case ActivationLeaf:
//                finalResult.emplace_back(it->first, std::make_pair(it->second, MarkedEventsVector{marked_event::activation(it->first.second)}));
//            case TargetLeaf:
//                finalResult.emplace_back(it->first, std::make_pair(it->second, MarkedEventsVector{marked_event::target(it->first.second)}));
//                break;
//            default:
//                finalResult.emplace_back(it->first, std::make_pair(it->second, MarkedEventsVector{}));
//                break;
//        }
//    }
//
//    return finalResult;
//}

//

static inline void multi_merge(const std::vector<size_t> &range_vecs,
                               const std::vector<size_t> &no_range_vecs,
                              const std::vector<PartialResult>& range_cache,
                               const std::vector<std::pair<DataQuery, PartialResult>>& non_range_tables,
                              Result& result,
                              LeafType isLeaf,
                              bool isUnion) {
    if (range_vecs.empty() && no_range_vecs.empty()) {
        result.clear();
        return;
    }
    auto it = range_vecs.begin();
    auto last_range_intersection = range_cache.at(*it);
    PartialResult curr_range_intersection;
    for (std::size_t i = 1; i < range_vecs.size(); ++i) {
        it++;
        auto ref = range_cache.at(*it);
        if (isUnion)
            partialResultUnion(last_range_intersection,
                               ref,
                               std::back_inserter(curr_range_intersection));
        else
            partialResultIntersection(last_range_intersection,
                                      ref,
                                      std::back_inserter(curr_range_intersection));

        std::swap(last_range_intersection, curr_range_intersection);
        curr_range_intersection.clear();
    }

    // TODO!

    ResultRecord rcx;
    bool isActivation = false;
    bool isTarget = false;
    if (isLeaf == ActivationLeaf) {
        rcx.second.second.emplace_back(marked_event::activation(0));
        isActivation = true;
    } else if (isLeaf == TargetLeaf) {
        rcx.second.second.emplace_back(marked_event::target(0));
        isTarget = true;
    }

    // TODO: better done through views!
    for (auto it = last_range_intersection.begin(); it != last_range_intersection.end(); it++) {
        rcx.first = it->first;
        rcx.second.first = it->second;
        if (isActivation)
            rcx.second.second.at(0).id.parts.left = it->first.second;
        else if (isTarget)
            rcx.second.second.at(0).id.parts.right = it->first.second;
        result.emplace_back(rcx);
    }
}

static inline void data_merge(const std::vector<size_t> &vecs,
                              const std::vector<PartialResult>& results,
                              Result& result,
                              LeafType isLeaf,
                              bool isUnion) {
    if (vecs.empty()) {
        result.clear();
        return;
    }
    auto it = vecs.begin();
    auto last_intersection = results.at(*it);
    PartialResult curr_intersection;
    for (std::size_t i = 1; i < vecs.size(); ++i) {
        it++;
        const auto& ref = results.at(*it);
        if (isUnion)
            partialResultUnion(last_intersection,
                               ref,
                               std::back_inserter(curr_intersection));
        else
            partialResultIntersection(last_intersection,
                                      ref,
                                      std::back_inserter(curr_intersection));

        std::swap(last_intersection, curr_intersection);
        curr_intersection.clear();
    }

    ResultRecord rcx;
    bool isActivation = false;
    bool isTarget = false;
    if (isLeaf == ActivationLeaf) {
        rcx.second.second.emplace_back(marked_event::activation(0));
        isActivation = true;
    } else if (isLeaf == TargetLeaf) {
        rcx.second.second.emplace_back(marked_event::target(0));
        isTarget = true;
    }

    // TODO: better done through views!
    for (auto it = last_intersection.begin(); it != last_intersection.end(); it++) {
        rcx.first = it->first;
        rcx.second.first = it->second;
        if (isActivation)
            rcx.second.second.at(0).id.parts.left = it->first.second;
        else if (isTarget)
            rcx.second.second.at(0).id.parts.right = it->first.second;
        result.emplace_back(rcx);
    }
}


static inline void holistic_merge(LTLfQuery* formula,
                              const std::vector<PartialResult>& range_results,
                              const std::vector<std::pair<DataQuery, PartialResult>>& non_range_results) {

    bool isUnion = true;//formula->fields.id.parts.is_negated;
    if (formula->table_query.empty() && formula->range_query.empty()) {
        formula->result.clear();
        return;
    }

    PartialResult final_result;
    {
        PartialResult last_data_intersection;
        if (!formula->range_query.empty()) {
            auto it = formula->range_query.begin();
            last_data_intersection = range_results.at(*it);
            PartialResult curr_intersection;
            for (std::size_t i = 1; i < formula->range_query.size(); ++i) {
                it++;
                const auto &ref = range_results.at(*it);
                if (isUnion)
                    partialResultUnion(last_data_intersection,
                                       ref,
                                       std::back_inserter(curr_intersection));
                else
                    partialResultIntersection(last_data_intersection,
                                              ref,
                                              std::back_inserter(curr_intersection));

                std::swap(last_data_intersection, curr_intersection);
                curr_intersection.clear();
            }
        }

        PartialResult last_nodata_intersection;
        if (!formula->table_query.empty()) {
            auto it = formula->table_query.begin();
            last_nodata_intersection = non_range_results.at(*it).second;
            PartialResult curr_intersection;
            for (std::size_t i = 1; i < formula->table_query.size(); ++i) {
                it++;
                const auto &ref = non_range_results.at(*it).second;
                if (isUnion)
                    partialResultUnion(last_nodata_intersection,
                                       ref,
                                       std::back_inserter(curr_intersection));
                else
                    partialResultIntersection(last_nodata_intersection,
                                              ref,
                                              std::back_inserter(curr_intersection));

                std::swap(last_nodata_intersection, curr_intersection);
                curr_intersection.clear();
            }
        }

        if (formula->range_query.empty()) {
            std::swap(final_result, last_nodata_intersection);
        } else if (formula->table_query.empty()) {
            std::swap(final_result, last_data_intersection);
        } else {
            if (isUnion)
                partialResultUnion(last_data_intersection,
                                   last_nodata_intersection,
                                   std::back_inserter(final_result));
            else
                partialResultIntersection(last_data_intersection,
                                          last_nodata_intersection,
                                          std::back_inserter(final_result));
        }
        last_data_intersection.clear();
        last_nodata_intersection.clear();
    }

    ResultRecord rcx;
    bool isActivation = false;
    bool isTarget = false;
    if (formula->isLeaf == ActivationLeaf) {
        rcx.second.second.emplace_back(marked_event::activation(0));
        isActivation = true;
    } else if (formula->isLeaf == TargetLeaf) {
        rcx.second.second.emplace_back(marked_event::target(0));
        isTarget = true;
    }

    // TODO: better done through views!
    for (auto it = final_result.begin(); it != final_result.end(); it++) {
        rcx.first = it->first;
        rcx.second.first = it->second;
        if (isActivation)
            rcx.second.second.at(0).id.parts.left = it->first.second;
        else if (isTarget)
            rcx.second.second.at(0).id.parts.right = it->first.second;
        formula->result.emplace_back(rcx);
    }
}


static inline void local_logic_union(const LTLfQuery* q, Result& last_union, bool isTimed = true) {
    size_t N = q->args.size();
    if ((!q) || (N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = q->args.at(1)->result;
    } else if (N == 2) {
        last_union.clear();
        auto& arg1 = q->args.at(0);
        auto& arg2 = q->args.at(1);
        if (isTimed)
            or_logic_timed(arg1->result,
                            arg2->result,
                           last_union,
                     q->joinCondition);
        else
            or_logic_untimed(arg1->result,
                             arg2->result,
                             last_union,
                             q->joinCondition);
    } else {
        auto it = q->args.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < q->args.size(); ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                or_logic_timed(last_union,
                         ref,
                         curr_union);
            else
                or_logic_untimed(last_union,
                               ref,
                               curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}

static inline void local_fast_union(const LTLfQuery* q, Result& last_union, bool isTimed = true) {
    size_t N = q->args.size();
    if ((!q) || (N == 0)) {
        last_union.clear();
        return;
    } else if (N == 1) {
        last_union = q->args.at(1)->result;
    } else if (N == 2) {
        last_union.clear();
        auto& arg1 = q->args.at(0);
        auto& arg2 = q->args.at(1);
        if (isTimed)
            or_fast_timed(arg1->result,
                           arg2->result,
                           last_union,
                           q->joinCondition);
        else
            or_fast_untimed(arg1->result,
                             arg2->result,
                             last_union,
                             q->joinCondition);
    } else {
        auto it = q->args.begin();
        last_union = (*it)->result;
        Result curr_union;
        for (std::size_t i = 1; i < q->args.size(); ++i) {
            it++;
            auto ref = (*it)->result;
            if (isTimed)
                or_fast_timed(last_union,
                               ref,
                               curr_union);
            else
                or_fast_untimed(last_union,
                                 ref,
                                 curr_union);
            std::swap(last_union, curr_union);
            curr_union.clear();
        }
    }
}

static inline void untimed_exists_for_data_queries(LTLfQuery* formula,
                                     const std::vector<PartialResult>& results_cache) {
//    DEBUG_ASSERT(false);
    ResultRecord rcx;
    rcx.second.first = 1.0;
    rcx.first.second = 0;
    bool isActivation = false;
    bool isTarget = false;
    if (formula->isLeaf == ActivationLeaf) {
//        rcx.second.second.emplace_back(marked_event::activation(0));
        isActivation = true;
    } else if (formula->isLeaf == TargetLeaf) {
//        rcx.second.second.emplace_back(marked_event::target(0));
        isTarget = true;
    }

    bool isFirstIteration = true;
    uint32_t traceId = 0;
    uint16_t eventCount = 0;
    Result tmp_result;
    data_merge(formula->range_query, results_cache, tmp_result, formula->isLeaf, true);
//    ResultRecord cp{{0,0}, {1.0, {}}};
    for (auto ref = tmp_result.begin(); ref != tmp_result.end(); ref++) {
        if (isFirstIteration) {
            traceId = ref->first.first;
            eventCount = 1;
            if (isActivation) {
                rcx.second.second.emplace_back(marked_event::activation(ref->first.second));
            } else if (isTarget) {
                rcx.second.second.emplace_back(marked_event::target(ref->first.second));
            }
            isFirstIteration = false;
        } else {
            if ((traceId != ref->first.first)) {
                if ((eventCount >= formula->n)) {
                    rcx.first.first = traceId;
                    formula->result.emplace_back(rcx);
                    rcx.second.second.clear();
                }
                traceId = ref->first.first;
                eventCount = 1;
                if (isActivation) {
                    rcx.second.second.emplace_back(marked_event::activation(ref->first.second));
                } else if (isTarget) {
                    rcx.second.second.emplace_back(marked_event::target(ref->first.second));
                }
            } else {
                eventCount++;
                if (isActivation) {
                    rcx.second.second.emplace_back(marked_event::activation(ref->first.second));
                } else if (isTarget) {
                    rcx.second.second.emplace_back(marked_event::target(ref->first.second));
                }
            }
        }
    }
    tmp_result.clear();
    if ((eventCount >= formula->n)) {
        rcx.first.first = traceId;
        formula->result.emplace_back(rcx);
    }
}


static inline void import_from_partial_results(LTLfQuery* formula, size_t offset, const std::vector<std::pair<DataQuery, PartialResult>>& data_accessing) {
    ResultRecordSemantics R{1.0, {}};
    bool isActivation = false;
    bool isTarget = false;
    if (formula->isLeaf == ActivationLeaf) {
        R.second.emplace_back(marked_event::left(0));
        isActivation = true;
    } else if (formula->isLeaf == TargetLeaf) {
        R.second.emplace_back(marked_event::right(0));
        isTarget = true;
    }
    for (const auto& ref : data_accessing.at(formula->table_query.at(offset)).second) {
        if (isActivation)
            R.second.at(0).id.parts.left = ref.first.second;
        else if (isTarget)
            R.second.at(0).id.parts.right = ref.first.second;
        formula->result.emplace_back(ref.first, R);
    }
}

size_t estimate_operator_time_cost(LTLfQuery* formula) {
    size_t prod = 1;
    for (const auto& x : formula->args)
        prod *= x->result.size();
    return prod;
}

void MAXSatPipeline::abidinglogic_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb) {
    /// Scanning the query plan starting from the leaves (rbegin) towards the actual declare formulae (rend)
    auto it = qm.Q.rbegin(), en = qm.Q.rend();
    size_t Depthidx = qm.Q.size()-1;
    for (; it != en; it++) {
        Result tmp_result;

        PARALLELIZE_LOOP_BEGIN(pool,schedulingType  ,blocks,it->second,estimate_operator_time_cost)
//        PARALLELIZE_LOOP_BEGIN(pool, 0, it->second.size(), lb, ub)
//            for (size_t j = lb; j < ub; j++) {
                LTLfQuery* formula = it->second.at(i); // TODO: run this query
                if (!formula) continue;


//                if (formula->fields.id.parts.directly_from_cache) {
//// import_from_partial_results
//                } else
                {
                    // Combine the results from the results_cache
                    switch (formula->t) {
                        case LTLfQuery::INIT_QP:
                            /// CORRECTNESS CHECK: The INIT shall either contain one single table atom or a set
                            /// of predicates in conjunction
                            DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                            if (formula->table_query.size() == 1) {
                                // In this situation, I'm directly taking the pre-computated data
                                import_from_partial_results(formula, 0, data_accessing);
                            } else {
                                data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                                formula->result.erase(std::remove_if(formula->result.begin(),
                                                                 formula->result.end(),
                                                                 [](const auto&  x){return x.first.second > 0;}),
                                                  formula->result.end());
                            } break;

                        case LTLfQuery::END_QP:
                            DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                            if (formula->table_query.size() == 1) {
                                // In this situation, I'm directly taking the pre-computated data
                                import_from_partial_results(formula, 0, data_accessing);
                            } else {
                                data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                                formula->result.erase(std::remove_if(formula->result.begin(),
                                                                     formula->result.end(),
                                                                     [kb](const auto&  x){return x.first.second < kb.act_table_by_act_id.trace_length.at(x.first.first)-1;}),
                                                      formula->result.end());
                            } break;

                        case LTLfQuery::FIRST_QP:
                        case LTLfQuery::LAST_QP: {
                            DEBUG_ASSERT((formula->table_query.size() == 1));
                            import_from_partial_results(formula, 0, data_accessing);
                        } break;

                        case LTLfQuery::EXISTS_QP: {
                            if (formula->fields.id.parts.is_timed) {
                                DEBUG_ASSERT((formula->n == 1));
                                holistic_merge(formula, results_cache, data_accessing);
                            } else {
                                DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                                if (formula->table_query.size() == 1) {
                                    // In this situation, I'm directly taking the pre-computated data
                                    import_from_partial_results(formula, 0, data_accessing);
                                } else {
                                    untimed_exists_for_data_queries(formula, results_cache);
                                } break;
                            }
                        } break;

                        case LTLfQuery::ABSENCE_QP: {
                            DEBUG_ASSERT(formula->table_query.size() == 1);
                            DEBUG_ASSERT(!formula->fields.id.parts.is_timed);
                            import_from_partial_results(formula, 0, data_accessing);

                        } break;

// TODO: replace with the novel atomic semantics
//                        case LTLfQuery::ABSENCE_QP:
//                            // The difference with absence is that, if it is absent, then it shall not be there with the same number
//                            if (formula->fields.id.parts.is_timed)
//                                data_merge(formula->partial_results, results_cache, formula->result, formula->isLeaf);
//                            else
//                                absence_or_exists(formula, results_cache);
//                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
//                                formula->result = negateUntimed(formula->result, kb.act_table_by_act_id.trace_length, true);
//                            } else if (formula->fields.id.parts.is_timed) {
//                                if (formula->fields.id.parts.preserve)
//                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
//                                negated_logic_timed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            } else {
//                                negated_logic_untimed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            }
//                            break;

                        case LTLfQuery::NEXT_QP:
                            formula->result = rnext(formula->args.at(0)->result);
                            break;

                        case LTLfQuery::OR_QP:
                            local_fast_union(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::AND_QP:
                            local_logic_intersection(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::IMPL_QP:
                            if (formula->fields.id.parts.is_timed) {
                                negated_logic_timed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_logic_timed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                    tmp_result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_logic_untimed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_logic_untimed(formula->args.at(0)->result,
                                                      formula->args.at(1)->result,
                                                      tmp_result,
                                                      formula->result,
                                                      formula->joinCondition,
                                                      kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::IFTE_QP:
                            if (formula->fields.id.parts.is_timed)
                                implies_logic_timed(formula->args.at(0)->result,
                                                  formula->args.at(1)->result,
                                                    formula->args.at(2)->result,
                                                  formula->result,
                                                  formula->joinCondition,
                                                  kb.act_table_by_act_id.trace_length);
                            else
                                implies_logic_untimed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                      formula->args.at(3)->result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::U_QP:
                            if (formula->fields.id.parts.is_timed)
                                until_logic_timed(formula->args.at(0)->result,
                                                  formula->args.at(1)->result,
                                                  formula->result,
                                                  formula->joinCondition,
                                                  kb.act_table_by_act_id.trace_length);
                            else
                                until_logic_untimed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::G_QP:
                            if (formula->fields.id.parts.is_timed)
                                global_logic_timed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                global_logic_untimed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::F_QP:
                            if (formula->fields.id.parts.is_timed)
                                future_logic_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                future_logic_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::NOT_QP:
                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
                                formula->result = negateUntimed(formula->args[0]->result, kb.act_table_by_act_id.trace_length, true);
                            } else if (formula->fields.id.parts.is_timed) {
                                if (formula->fields.id.parts.preserve)
                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
                                negated_logic_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_logic_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::AF_QPT:
                            if (formula->fields.id.parts.is_timed) {
                                future_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                and_logic_timed(formula->args.at(0)->result, tmp_result, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            } else {
                                future_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                and_logic_untimed(formula->args.at(0)->result, tmp_result, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            }
                            break;

                        case LTLfQuery::AXG_QPT:
                            if (formula->fields.id.parts.is_timed) {
                                Result local;
                                global_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                next_logical(tmp_result, local);
                                and_logic_timed(formula->args.at(0)->result, local, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            } else {
                                Result local;
                                global_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                next_logical(tmp_result, local);
                                and_logic_untimed(formula->args.at(0)->result, local, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            }
                            break;

                        case LTLfQuery::AG_QPT:
                            if (formula->fields.id.parts.is_timed) {
                                global_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                and_logic_timed(formula->args.at(0)->result, tmp_result, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            } else {
                                global_logic_timed(formula->args.at(1)->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                and_logic_untimed(formula->args.at(0)->result, tmp_result, formula->result, formula->joinCondition, kb.act_table_by_act_id.trace_length);
                                tmp_result.clear();
                            }
                            break;

                        case LTLfQuery::FALSEHOOD_QP:
                            formula->result.clear();
                            break;

                        default:
                            return; // TODO: Now, skipping the computation! Later on, do something!
                    }
                }
//            }
        PARALLELIZE_LOOP_END

//        // Clearing the caches, so to free potentially unrequired memory for the next computational steps
//        // This might help save some memory in big-data scenarios
#if 0
        PARALLELIZE_LOOP_BEGIN(pool, schedulingType ,blocks,it->second,[](auto& x ){return 1;})
//        PARALLELIZE_LOOP_BEGIN(pool, 0, it->second.size(), lb, ub)
//        for (size_t j = lb; j < ub; j++) {
            auto formula = it->second.at(i);
            for (auto ptr : formula->args) {
                // Preserving the cache only if I need it for computing the Support
                if (ptr->parentMin == Depthidx && (((final_ensemble != PerDeclareSupport) || (ptr->isLeaf != ActivationLeaf)))) {
                    ptr->result.clear(); ptr->result.shrink_to_fit();
                }
            }
//        }
                PARALLELIZE_LOOP_END
#endif
//        PARALLELIZE_LOOP_END
                            Depthidx--;
    }
}



void MAXSatPipeline::fast_v1_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb) {
/// Scanning the query plan starting from the leaves (rbegin) towards the actual declare formulae (rend)
    auto it = qm.Q.rbegin(), en = qm.Q.rend();
    size_t Depthidx = qm.Q.size()-1;
    for (; it != en; it++) {
        Result tmp_result;
        PARALLELIZE_LOOP_BEGIN(pool,schedulingType  ,blocks,it->second,estimate_operator_time_cost)
//            for (size_t j = lb; j < ub; j++) {
                auto formula = it->second.at(i); // TODO: run this query
                if (!formula) continue;

//                if (formula->fields.id.parts.directly_from_cache) {
//// import_from_partial_results
//                } else
                {
                    // Combine the results from the results_cache
                    switch (formula->t) {
                        case LTLfQuery::INIT_QP:
                        /// CORRECTNESS CHECK: The INIT shall either contain one single table atom or a set
                        /// of predicates in conjunction
                        DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                        if (formula->table_query.size() == 1) {
                            // In this situation, I'm directly taking the pre-computated data
                            import_from_partial_results(formula, 0, data_accessing);
                        } else {
                            data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                            formula->result.erase(std::remove_if(formula->result.begin(),
                                                                 formula->result.end(),
                                                                 [](const auto&  x){return x.first.second > 0;}),
                                                  formula->result.end());
                        } break;

                        case LTLfQuery::END_QP:
                            DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                            if (formula->table_query.size() == 1) {
                                // In this situation, I'm directly taking the pre-computated data
                                import_from_partial_results(formula, 0, data_accessing);
                            } else {
                                data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                                formula->result.erase(std::remove_if(formula->result.begin(),
                                                                     formula->result.end(),
                                                                     [kb](const auto&  x){return x.first.second < kb.act_table_by_act_id.trace_length.at(x.first.first)-1;}),
                                                      formula->result.end());
                            } break;

                        case LTLfQuery::FIRST_QP:
                        case LTLfQuery::LAST_QP: {
                            DEBUG_ASSERT((formula->table_query.size() == 1));
                            import_from_partial_results(formula, 0, data_accessing);
                        } break;

                        case LTLfQuery::EXISTS_QP: {
                            if (formula->fields.id.parts.is_timed) {
                                DEBUG_ASSERT((formula->n == 1));
                                holistic_merge(formula, results_cache, data_accessing);
                            } else {
                                DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                                if (formula->table_query.size() == 1) {
                                    // In this situation, I'm directly taking the pre-computated data
                                    import_from_partial_results(formula, 0, data_accessing);
                                } else {
                                    untimed_exists_for_data_queries(formula, results_cache);
                                } break;
                            }
                        } break;

                        case LTLfQuery::ABSENCE_QP: {
                            DEBUG_ASSERT(formula->table_query.size() == 1);
                            DEBUG_ASSERT(!formula->fields.id.parts.is_timed);
                            import_from_partial_results(formula, 0, data_accessing);

                        } break;

// TODO: replace with novel node semantics
//                        case LTLfQuery::ABSENCE_QP:
//                            // The difference with absence is that, if it is absent, then it shall not be there with the same number
//                            if (formula->fields.id.parts.is_timed)
//                                data_merge(formula->partial_results, results_cache, formula->result, formula->isLeaf);
//                            else
//                                absence_or_exists(formula, results_cache);
//                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
//                                formula->result = negateUntimed(formula->result, kb.act_table_by_act_id.trace_length, true);
//                            } else if (formula->fields.id.parts.is_timed) {
//                                if (formula->fields.id.parts.preserve)
//                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
//                                negated_fast_timed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            } else {
//                                negated_fast_untimed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            }
//                            break;

                        case LTLfQuery::NEXT_QP:
                            formula->result = rnext(formula->args.at(0)->result);
                            break;

                        case LTLfQuery::OR_QP:
                            local_fast_union(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::AND_QP:
                            local_fast_intersection(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::IMPL_QP:
                            if (formula->fields.id.parts.is_timed) {
                                negated_fast_timed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_fast_timed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                    tmp_result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_fast_untimed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_fast_untimed(formula->args.at(0)->result,
                                                      formula->args.at(1)->result,
                                                      tmp_result,
                                                      formula->result,
                                                      formula->joinCondition,
                                                      kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::IFTE_QP:
                            if (formula->fields.id.parts.is_timed)
                                implies_fast_timed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                    formula->args.at(2)->result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            else
                                implies_fast_untimed(formula->args.at(0)->result,
                                                      formula->args.at(1)->result,
                                                      formula->args.at(3)->result,
                                                      formula->result,
                                                      formula->joinCondition,
                                                      kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::U_QP:
                            if (formula->fields.id.parts.is_timed)
                                until_fast_timed(formula->args.at(0)->result,
                                                  formula->args.at(1)->result,
                                                  formula->result,
                                                  formula->joinCondition,
                                                  kb.act_table_by_act_id.trace_length);
                            else
                                until_fast_untimed(formula->args.at(0)->result,
                                                    formula->args.at(1)->result,
                                                    formula->result,
                                                    formula->joinCondition,
                                                    kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::G_QP:
                            if (formula->fields.id.parts.is_timed)
                                global_fast_timed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                global_fast_untimed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::F_QP:
                            if (formula->fields.id.parts.is_timed)
                                future_fast_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                future_fast_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::NOT_QP:
                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
                                formula->result = negateUntimed(formula->args[0]->result, kb.act_table_by_act_id.trace_length, true);
                            } else if (formula->fields.id.parts.is_timed) {
                                if (formula->fields.id.parts.preserve)
                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
                                negated_fast_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_fast_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::AF_QPT:
                            if (formula->fields.id.parts.is_timed)
                                aAndFutureB_timed_variant_1(formula->args.at(0)->result,
                                                  formula->args.at(1)->result,
                                                  formula->result,
                                                  formula->joinCondition,
                                                  kb.act_table_by_act_id.trace_length);
                            else
                                throw std::runtime_error("AndFuture is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::AXG_QPT:
                            if (formula->fields.id.parts.is_timed)
                                aAndNextGloballyB_timed(formula->args.at(0)->result,
                                                        formula->args.at(1)->result,
                                                        formula->result,
                                                        formula->joinCondition,
                                                        kb.act_table_by_act_id.trace_length);
                            else
                                throw std::runtime_error("AndNextGlobally is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::AG_QPT:
                            if (formula->fields.id.parts.is_timed)
                                aAndGloballyB_timed_variant_1(formula->args.at(0)->result,
                                                        formula->args.at(1)->result,
                                                        formula->result,
                                                        formula->joinCondition,
                                                        kb.act_table_by_act_id.trace_length);
                            else
                                throw std::runtime_error("AndNextGlobally is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::FALSEHOOD_QP:
                            formula->result.clear();
                            break;

                        default:
                            return; // TODO: Now, skipping the computation! Later on, do something!
                    }
                }
//            }
        PARALLELIZE_LOOP_END

        // Clearing the caches, so to free potentially unrequired memory for the next computational steps
        // This might help save some memory in big-data scenarios
#if 0
        PARALLELIZE_LOOP_BEGIN(pool, schedulingType  ,blocks,it->second,[](auto& x ){return 1;})
//        PARALLELIZE_LOOP_BEGIN(pool, 0, it->second.size(), lb, ub)
//        for (size_t j = lb; j < ub; j++) {
                            auto formula = it->second.at(i);
                            for (auto ptr : formula->args) {
                                // Preserving the cache only if I need it for computing the Support
                                if (ptr->parentMin == Depthidx && (((final_ensemble != PerDeclareSupport) || (ptr->isLeaf != ActivationLeaf)))) {
                                    ptr->result.clear(); ptr->result.shrink_to_fit();
                                }
                            }
//        }
                PARALLELIZE_LOOP_END
#endif
        Depthidx--;
    }
}

void MAXSatPipeline::hybrid_query_running(const std::vector<PartialResult>& results_cache, const KnowledgeBase& kb) {
/// Scanning the query plan starting from the leaves (rbegin) towards the actual declare formulae (rend)
    auto it = qm.Q.rbegin(), en = qm.Q.rend();
    size_t idx = qm.Q.size()-1;
    for (; it != en; it++) {
        Result tmp_result;
        PARALLELIZE_LOOP_BEGIN(pool, schedulingType ,blocks,it->second,estimate_operator_time_cost)
//            for (size_t j = lb; j < ub; j++) {
            auto formula = it->second.at(i); // TODO: run this query
            if (!formula) continue;

//                if (formula->fields.id.parts.directly_from_cache) {
//// import_from_partial_results
//                } else
            {
                    // Combine the results from the results_cache
                    switch (formula->t) {
                        case LTLfQuery::INIT_QP:
                            /// CORRECTNESS CHECK: The INIT shall either contain one single table atom or a set
                            /// of predicates in conjunction
                            DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                            if (formula->table_query.size() == 1) {
                                // In this situation, I'm directly taking the pre-computated data
                                import_from_partial_results(formula, 0, data_accessing);
                            } else {
                                data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                                formula->result.erase(std::remove_if(formula->result.begin(),
                                                                     formula->result.end(),
                                                                     [](const auto&  x){return x.first.second > 0;}),
                                                      formula->result.end());
                            } break;

                        case LTLfQuery::END_QP:
                            DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                            if (formula->table_query.size() == 1) {
                                // In this situation, I'm directly taking the pre-computated data
                                import_from_partial_results(formula, 0, data_accessing);
                            } else {
                                data_merge(formula->range_query, results_cache, formula->result, formula->isLeaf, true);
                                formula->result.erase(std::remove_if(formula->result.begin(),
                                                                     formula->result.end(),
                                                                     [kb](const auto&  x){return x.first.second < kb.act_table_by_act_id.trace_length.at(x.first.first)-1;}),
                                                      formula->result.end());
                            } break;

                        case LTLfQuery::FIRST_QP:
                        case LTLfQuery::LAST_QP: {
                            DEBUG_ASSERT((formula->table_query.size() == 1));
                            import_from_partial_results(formula, 0, data_accessing);
                        } break;

                        case LTLfQuery::EXISTS_QP: {
                            if (formula->fields.id.parts.is_timed) {
                                DEBUG_ASSERT((formula->n == 1));
                                holistic_merge(formula, results_cache, data_accessing);
                            } else {
                                DEBUG_ASSERT((formula->table_query.size() == 1) != (!formula->range_query.empty()));
                                if (formula->table_query.size() == 1) {
                                    // In this situation, I'm directly taking the pre-computated data
                                    import_from_partial_results(formula, 0, data_accessing);
                                } else {
                                    untimed_exists_for_data_queries(formula, results_cache);
                                } break;
                            }
                        } break;

                        case LTLfQuery::ABSENCE_QP: {
                            DEBUG_ASSERT(formula->table_query.size() == 1);
                            DEBUG_ASSERT(!formula->fields.id.parts.is_timed);
                            import_from_partial_results(formula, 0, data_accessing);

                        } break;

// TODO: replace with novel node semantics
//                        case LTLfQuery::ABSENCE_QP:
//                            // The difference with absence is that, if it is absent, then it shall not be there with the same number
//                            if (formula->fields.id.parts.is_timed)
//                                data_merge(formula->partial_results, results_cache, formula->result, formula->isLeaf);
//                            else
//                                absence_or_exists(formula, results_cache);
//                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
//                                formula->result = negateUntimed(formula->result, kb.act_table_by_act_id.trace_length, true);
//                            } else if (formula->fields.id.parts.is_timed) {
//                                if (formula->fields.id.parts.preserve)
//                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
//                                negated_fast_timed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            } else {
//                                negated_fast_untimed(formula->result, tmp_result, kb.act_table_by_act_id.trace_length);
//                                std::swap(formula->result, tmp_result);
//                            }
//                            break;

                        case LTLfQuery::NEXT_QP:
                            formula->result = rnext(formula->args.at(0)->result);
                            break;

                        case LTLfQuery::OR_QP:
                            local_fast_union(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::AND_QP:
                            local_fast_intersection(formula, formula->result, formula->fields.id.parts.is_timed);
                            break;

                        case LTLfQuery::IMPL_QP:
                            if (formula->fields.id.parts.is_timed) {
                                negated_fast_timed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_fast_timed(formula->args.at(0)->result,
                                                   formula->args.at(1)->result,
                                                   tmp_result,
                                                   formula->result,
                                                   formula->joinCondition,
                                                   kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_fast_untimed(formula->args[0]->result, tmp_result, kb.act_table_by_act_id.trace_length);
                                implies_fast_untimed(formula->args.at(0)->result,
                                                     formula->args.at(1)->result,
                                                     tmp_result,
                                                     formula->result,
                                                     formula->joinCondition,
                                                     kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::IFTE_QP:
                            if (formula->fields.id.parts.is_timed)
                                implies_fast_timed(formula->args.at(0)->result,
                                                   formula->args.at(1)->result,
                                                   formula->args.at(2)->result,
                                                   formula->result,
                                                   formula->joinCondition,
                                                   kb.act_table_by_act_id.trace_length);
                            else
                                implies_fast_untimed(formula->args.at(0)->result,
                                                     formula->args.at(1)->result,
                                                     formula->args.at(3)->result,
                                                     formula->result,
                                                     formula->joinCondition,
                                                     kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::U_QP:
                            if (formula->fields.id.parts.is_timed)
                                until_fast_timed(formula->args.at(0)->result,
                                                 formula->args.at(1)->result,
                                                 formula->result,
                                                 formula->joinCondition,
                                                 kb.act_table_by_act_id.trace_length);
                            else if (kb.noTraces >= HYBRID_LOG_QUERY_THRESHOLD)
                                until_fast_untimed(formula->args.at(0)->result,
                                                   formula->args.at(1)->result,
                                                   formula->result,
                                                   formula->joinCondition,
                                                   kb.act_table_by_act_id.trace_length);
                            else 
                              until_logic_untimed(formula->args.at(0)->result,
                                                   formula->args.at(1)->result,
                                                   formula->result,
                                                   formula->joinCondition,
                                                   kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::G_QP:
                            if (formula->fields.id.parts.is_timed)
                                global_fast_timed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                global_fast_untimed(formula->args.at(0)->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::F_QP:
                            if (formula->fields.id.parts.is_timed)
                                future_fast_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            else
                                future_fast_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            break;

                        case LTLfQuery::NOT_QP:
                            if (formula->fields.id.parts.preserve && (!formula->fields.id.parts.is_timed)) {
                                formula->result = negateUntimed(formula->args[0]->result, kb.act_table_by_act_id.trace_length, true);
                            } else if (formula->fields.id.parts.is_timed) {
                                if (formula->fields.id.parts.preserve)
                                    throw std::runtime_error("At this stage, cannot preserve data for timed");
                                negated_fast_timed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            } else {
                                negated_fast_untimed(formula->args[0]->result, formula->result, kb.act_table_by_act_id.trace_length);
                            }
                            break;

                        case LTLfQuery::AF_QPT:
                            if (formula->fields.id.parts.is_timed){
                                if(kb.average_trace_length > HYBRID_QUERY_THRESHOLD)
                                    aAndFutureB_timed_variant_2(formula->args.at(0)->result,
                                                      formula->args.at(1)->result,
                                                      formula->result,
                                                      formula->joinCondition,
                                                      kb.act_table_by_act_id.trace_length);
                                else
                                    aAndFutureB_timed_variant_1(formula->args.at(0)->result,
                                                      formula->args.at(1)->result,
                                                      formula->result,
                                                      formula->joinCondition,
                                                      kb.act_table_by_act_id.trace_length);
                            }
                            else
                                throw std::runtime_error("AndFuture is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::AXG_QPT:
                            if (formula->fields.id.parts.is_timed)
                                aAndNextGloballyB_timed(formula->args.at(0)->result,
                                                        formula->args.at(1)->result,
                                                        formula->result,
                                                        formula->joinCondition,
                                                        kb.act_table_by_act_id.trace_length);
                            else
                                throw std::runtime_error("AndNextGlobally is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::AG_QPT:
                            if (formula->fields.id.parts.is_timed) {
                                if (kb.average_trace_length > HYBRID_QUERY_THRESHOLD)
                                    aAndGloballyB_timed_variant_2(formula->args.at(0)->result,
                                                        formula->args.at(1)->result,
                                                        formula->result,
                                                        formula->joinCondition,
                                                        kb.act_table_by_act_id.trace_length);
                                else
                                    aAndGloballyB_timed_variant_1(formula->args.at(0)->result,
                                                                formula->args.at(1)->result,
                                                                formula->result,
                                                                formula->joinCondition,
                                                                kb.act_table_by_act_id.trace_length);
                            }
                            else
                                throw std::runtime_error("AndNextGlobally is untimed: unexpected implementation!");
                            break;

                        case LTLfQuery::FALSEHOOD_QP:
                            formula->result.clear();
                            break;

                        default:
                            return; // TODO: Now, skipping the computation! Later on, do something!
                    }
                }
//            }

            PARALLELIZE_LOOP_END

        // Clearing the caches, so to free potentially unrequired memory for the next computational steps
        // This might help save some memory in big-data scenarios
#if 0
        PARALLELIZE_LOOP_BEGIN(pool, schedulingType ,blocks,it->second,[](auto& x ){return 1;})
//        PARALLELIZE_LOOP_BEGIN(pool, 0, it->second.size(), lb, ub)
//        for (size_t j = lb; j < ub; j++) {
                            auto formula = it->second.at(i);
                            for (auto ptr : formula->args) {
                                // Preserving the cache only if I need it for computing the Support
                                if (ptr->parentMin == Depthidx && (((final_ensemble != PerDeclareSupport) || (ptr->isLeaf != ActivationLeaf)))) {
                                    ptr->result.clear(); ptr->result.shrink_to_fit();
                                }
                            }
//        }
                PARALLELIZE_LOOP_END
#endif
        idx--;
    }
}

void MAXSatPipeline::pipeline(CNFDeclareDataAware* model,
                              const AtomizingPipeline& atomization,
                              const KnowledgeBase& kb) {
    /// Clearing the previous spurious computation values
    clear();
    qm.atomization = &atomization;

    /// Extracting the predicates from both the LTLf semantics and the data extracted from it
    {
        auto start = std::chrono::system_clock::now();
        data_chunk(model, atomization, kb);
        auto end = std::chrono::system_clock::now();
        auto elapsed =
                std::chrono::duration<double, std::milli>(end - start);
        declare_to_ltlf_time = elapsed.count();
    }

#if 1
    {
        auto start = std::chrono::system_clock::now();
        std::vector<PartialResult> pr = subqueriesRunning(kb);
        switch (operators) {
            case AbidingLogic:
                abidinglogic_query_running(pr, kb);
                break;

            case FastOperator_v1:
                fast_v1_query_running(pr, kb);
                break;

            case Hybrid:
                hybrid_query_running(pr, kb);
                break;

            default:
                break;
        }

        if (!qm.Q.empty()) {
            switch (final_ensemble) {
                case PerDeclareSupport: {
                    std::unordered_map<LTLfQuery*, double> visited;
                    for (size_t i = 0, N = declare_to_query.size(); i<N; i++) { // each declare i
                        const auto &declare = declare_to_query.at(i);
                        Result localActivations;
                        local_logic_union(qm.activations.at(i), localActivations, false);
                        if (localActivations.empty()) {
                            DEBUG_ASSERT(declare->result.empty());
                            support_per_declare.emplace_back(0);
                        }
//                        else if ((!declare->fields.id.parts.is_timed) && ((declare->t == LTLfQuery::INIT_QP) || (declare->t == LTLfQuery::END_QP))) {
//                            auto it2 = visited.emplace(declare, declare->result.size() / (((double)kb.noTraces)));
//                            support_per_declare.emplace_back(it2.first->second);
//                        }
                        else {
                            auto it2 = visited.emplace(declare, 0);
                            if (it2.second) {
                                double numerator = 0.0;
                                for (const auto& trace : declare->result) {
                                    if (!trace.second.second.empty()) {
                                        if (IS_MARKED_EVENT_ACTIVATION(*trace.second.second.begin()) || IS_MARKED_EVENT_MATCH(*trace.second.second.rbegin()))
                                            numerator++;
                                    }
                                }
                                it2.first->second = numerator / ((double)kb.noTraces);
                            }
                            support_per_declare.emplace_back(it2.first->second);
                        }
                    }
                } break;


                case PerDeclareConfidence: {
                    std::unordered_map<LTLfQuery*, double> visited;
                    for (size_t i = 0, N = declare_to_query.size(); i<N; i++) { // each declare i
                        const auto &declare = declare_to_query.at(i);
                        auto& aptr = qm.activations.at(i);
                        if (aptr.empty()) {
                            support_per_declare.emplace_back(declare->result.empty() ? 0 : 1);
                        } else {
                            Result localActivations;
                            local_logic_union(qm.activations.at(i), localActivations, false);
                            if (localActivations.empty()) {
                                DEBUG_ASSERT(declare->result.empty());
                                support_per_declare.emplace_back(0);
                            } else {
                                auto it2 = visited.emplace(declare, 0);
                                if (it2.second) {
                                    auto it = localActivations.begin(), en = localActivations.end();
                                    auto val = it->first.first;
                                    double numerator = 0.0;
                                    double denominator = 1.0; // count == 1
                                    it++;
                                    for (; it != en; it++) {
                                        if (it->first.first != val) {
                                            val = it->first.first;
                                            denominator += 1.0;
                                        }
                                    }
                                    for (const auto& trace : declare->result) {
                                        if (!trace.second.second.empty()) {
                                            if (IS_MARKED_EVENT_ACTIVATION(*trace.second.second.begin()) || IS_MARKED_EVENT_MATCH(*trace.second.second.rbegin()))
                                                numerator++;
                                        }
                                    }
                                    it2.first->second = numerator / denominator;
                                }
                                support_per_declare.emplace_back(it2.first->second);
                            }
                        }
                    }
                } break;

                case TraceMaximumSatisfiability: {
                    // Working under the assumption that all of the final Declare clauses are, at the root level, untimed operations
                    max_sat_per_trace.resize(kb.noTraces, 0.0);
                    for (size_t i = 0, N = declare_to_query.size(); i<N; i++) {
                        const auto &q = declare_to_query.at(i);
                        for (const auto& rx: q->result) {
                            // Counting the trace if and only if it has a near-1 value
                            if (std::abs(rx.second.first - 1.0) <= std::numeric_limits<double>::epsilon())
                                max_sat_per_trace[rx.first.first]++;
                        }
                    }
                    for (auto& ref : max_sat_per_trace) {
                        assert(ref <= declare_to_query.size());
                        ref = ref / ((double)declare_to_query.size());
                    }
                } break;

                case TraceIntersection: {
                    LTLfQuery conjunction;
                    conjunction.args = qm.Q.begin()->second;
                    local_fast_intersection(&conjunction, this->result, false);
                } break;

                default: break;
            }
        }

        auto end = std::chrono::system_clock::now();
//        pr.clear();
        auto elapsed =
                std::chrono::duration<double, std::milli>(end - start);
        ltlf_query_time = elapsed.count();
    }
#endif
}

std::string MAXSatPipeline::generateGraph() const { return qm.generateGraph(); }
