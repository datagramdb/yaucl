//
// Created by giacomo on 03/01/2022.
//

#include <knobab/server/algorithms/grounding/grounding.h>
#include <yaucl/bpm/structures/commons/testing_predicates.h>
#include <iostream>
#include <yaucl/functional/assert.h>


yaucl::functional::optional_binary<DataPredicate>
instantiateWithValues(const DataPredicate &toGround,
                      const std::unordered_map<std::string, union_minimal> &mapL,
                      const std::unordered_map<std::string, union_minimal> &mapR,
                      std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelLeft);

inline yaucl::functional::optional_binary<DataPredicate> recursively_run_instantiation(const DataPredicate &toGround,
                                          const std::unordered_map<std::string, union_minimal> &mapL,
                                          const std::unordered_map<std::string, union_minimal> &mapR,
                                          //std::vector<DataPredicate> &allPredicates,
                                          std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelLeft//,
                                          //std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelRight,
                                          //bool isAlwaysFromTheSameEventAndIntersectMap

) {
    std::vector<DataPredicate> allPredicates;
    DataPredicate truth{};
    for (const auto &ref: toGround.BiVariableConditions) {
        auto tmp = instantiateWithValues(ref, mapL, mapR, toIntersectAtAnUpperLevelLeft/*, toIntersectAtAnUpperLevelRight,
                                         isAlwaysFromTheSameEventAndIntersectMap*/);
        if (tmp.isInvalid()) {
            return yaucl::functional::optional_binary<DataPredicate>::invalid(); // if an internal instance is unsatisfied, everything becomes falsified, as in conjunction.
        } else if (tmp.isValidButPresent()) {
            auto tmp2 = tmp.get();
            if (tmp2.isBiVariableCondition()) {
                allPredicates.emplace_back(tmp.get());
            } else {
                if (!truth.intersect_with(tmp2))
                    return yaucl::functional::optional_binary<DataPredicate>::invalid();
            }
            // if the predicate is valid but not present, it would very probably already part in toIntersectAtAnUpperLevel
        }
    }
    std::swap(truth.BiVariableConditions, allPredicates);
    return {truth};
}

inline yaucl::functional::optional_binary<DataPredicate>
eqGrounding(const DataPredicate &toGround,
            const std::unordered_map<std::string, union_minimal> &mapL,
            const std::unordered_map<std::string, union_minimal> &mapR,
            const std::unordered_map<std::string, union_minimal>::const_iterator &it,
            std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelLeft//,
            //std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelRight,
            //bool isAlwaysFromTheSameEventAndIntersectMap
) {// If the test is failed, then this predicate reduces to false, which invalidates all
// of the possibile binary predicates in it: so, everything reduces to an empty predicate
    if (!test_minimal_data_predicate(toGround, it->second))
        return yaucl::functional::optional_binary<DataPredicate>::invalid();
    // otherwise, this predicate after grounding_conf reduces to being equal to the given value
// But, before allocating memory for nothing at all, I must first test all of the remaining
// ones if any
    //std::vector<DataPredicate> allPredicates;
    auto l = (recursively_run_instantiation(toGround, mapL, mapR, /*allPredicates,*/ toIntersectAtAnUpperLevelLeft/*,
                                       toIntersectAtAnUpperLevelRight, isAlwaysFromTheSameEventAndIntersectMap*/));
    if (l.isInvalid())
        return yaucl::functional::optional_binary<DataPredicate>::invalid();

    DataPredicate novel{toGround.var, EQ, it->second};
    auto ll = l.get();
    if (!ll.intersect_with(novel))
        return yaucl::functional::optional_binary<DataPredicate>::invalid();

    ll.label = ll.label; // Inheriting the label information, if any
    //std::swap(allPredicates, novel.BiVariableConditions); // moving the memory, instead of copying it!
    return {ll}; // valid, not missing value
}

/**
 * This function provides the grounding_conf of a single data predicate associated to a variable, if possible
 *
 * @param toGround                          Predicate to be grounded
 * @param map                               Substitution map, mapping each variable in the predicate to the value
 *                                          to be replaced
 * @param toIntersectAtAnUpperLevel         If the resulting predicate has a change of variable after grounding_conf, in this
 *                                          return map you will have the result of it
 * @return There are two cases of missing value: either we had a failure, thus implying that the value
 *         did not satisfy the predicate, and therefore the whole part shall be discarded, or an empty
 *         value, meaning that the interval was successfully rewritten, but that, in the meantime, has
 *         changed the variable towards which it belongs. If the value is present, then this is the substitute
 *         candidate (the grounded one) for the one that was originally provided.
 */
/*inline*/ yaucl::functional::optional_binary<DataPredicate>
instantiateWithValues(const DataPredicate &toGround,
                      const std::unordered_map<std::string, union_minimal> &mapL,
                      const std::unordered_map<std::string, union_minimal> &mapR,
                      std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelLeft) {
    // Sanity check!
    // If there is a variable name on the right handside, I shall not have binary predicates assocated as siblings
    DEBUG_ASSERT(toGround.varRHS.empty() || toGround.BiVariableConditions.empty());

    switch (toGround.casusu) {
        case INTERVAL: {
            // If this is an interval, there should be no right variable!
            // Consistency checking for syntax
            DEBUG_ASSERT(toGround.varRHS.empty());
            // If the map contains the value to be grounded as part of it, then
            // we will need to test the predicate
            auto it = mapL.find(toGround.var);
            if (it != mapL.end()) {
                return eqGrounding(toGround, mapL, mapR, it, toIntersectAtAnUpperLevelLeft/*,
                                   toIntersectAtAnUpperLevelRight, isAlwaysFromTheSameEventAndIntersectMap*/);
            } else {
                // Otherwise, the evaluation is unknown, and we continue with the internal predicates, if any
                //std::vector<DataPredicate> allPredicates;
                auto l = (recursively_run_instantiation(toGround, mapL, mapR, /*allPredicates,*/ toIntersectAtAnUpperLevelLeft/*,
                                                   toIntersectAtAnUpperLevelRight,
                                                   isAlwaysFromTheSameEventAndIntersectMap*/));
                if (l.isInvalid())
                    return yaucl::functional::optional_binary<DataPredicate>::invalid();

                auto ll = l.get();
                DataPredicate novel{toGround.label, toGround.var, toGround.value, toGround.value_upper_bound};
                if (!ll.intersect_with(novel))
                    return yaucl::functional::optional_binary<DataPredicate>::invalid();
                ///std::swap(allPredicates, novel.BiVariableConditions); // moving the memory, instead of copying it!
                return ll;
            }
        }

        case TTRUE: {
            ///std::vector<DataPredicate> allPredicates;
            auto l = (recursively_run_instantiation(toGround, mapL, mapR, toIntersectAtAnUpperLevelLeft/*,
                                               toIntersectAtAnUpperLevelRight, isAlwaysFromTheSameEventAndIntersectMap*/));
            if (l.isInvalid())
                return yaucl::functional::optional_binary<DataPredicate>::invalid();

            ///DataPredicate novel;
            ///std::swap(allPredicates, novel.BiVariableConditions); // moving the memory, instead of copying it!
            return l.get();
        }

        case LT:
        case GT:
        case LEQ:
        case GEQ:
        case EQ:
        case NEQ: {
            if (!toGround.varRHS.empty()) {
                // Due to the previous assertion, I am guaranteed that I do not need to check
                // the binary predicates, as they should never be there as children, but as siblings

                // if the right variable appears in the map, then replace itR with a value, first
                auto itR = mapR.find(toGround.varRHS);
                if (itR == mapR.end()) {
                    // Checking for the left variable
                    auto itL = mapL.find(toGround.var);
                    if (itL == mapL.end()) {
                        // If the left variable is not in the map either, then return the predicate as it is
                        return {toGround};
                    } else {
                        // Otherwise, I will need to revert the predicate...
                        DataPredicate cpy = toGround.reverseBiVariablePredicate()
                                        // and then instantiate it: as I know for sure that there is no right value, I'm
                                        // pretty confident that I can return the interval as it is
                                .instantiateRHSWith(itL->second);
                        // Now, I need to check whether cpy has the same variable as the old one
                        if (cpy.var == toGround.var) {
                            ////if (isAlwaysFromTheSameEventAndIntersectMap) {
                                // If yes, I can immediately return it as a proper value (e.g., x == x)
                                // even if this eventuality is rather improbable
                                std::cerr << "WARNING: an odd eventuality was reached" << std::endl;
                                return {cpy};
                            ///} else {
                            ///    // Otherwise, if they have a different label, they refer to a different predicate
                            ///    toIntersectAtAnUpperLevelRight[cpy.var].emplace_back(cpy);
                            ///}
                        } else {
                            // Otherwise, this interval shall be intersected with the other one, at the parent level
                            ////if (isAlwaysFromTheSameEventAndIntersectMap) {
                            ////    toIntersectAtAnUpperLevelLeft[cpy.var].emplace_back(cpy);
                            ////} else {
                            ////    toIntersectAtAnUpperLevelRight[cpy.var].emplace_back(cpy);
                                ////}
                            toIntersectAtAnUpperLevelLeft[cpy.var].emplace_back(cpy);
                            return yaucl::functional::optional_binary<DataPredicate>::validButMissing();
                        }
                    }
                } else {
                    // Instantiating the interval appropriately
                    DataPredicate cpy = toGround.instantiateRHSWith(itR->second);
                    // After reducint the predicate to that, use recursion, so to minimize the code writing and
                    // the case analysis. This won't diverge, as I'm not adding data, but simplifying/rewriting
                    return instantiateWithValues(cpy, mapL, mapR, toIntersectAtAnUpperLevelLeft/*,
                                                 toIntersectAtAnUpperLevelRight,
                                                 isAlwaysFromTheSameEventAndIntersectMap*/);
                }
            } else {
                // if the RHS is empty, then all I need to do is to test if the left element is part of the map
                auto itL = mapL.find(toGround.var);
                if (itL == mapL.end()) {
                    // If it isn't, I'm not grounding_conf it, and therefore I'm returning the interval untouched
                    return {toGround};
                } else {
                    // Otherwise, I just need to test if it is valid and, if yes, I replace the interval
                    // with the exact value that I extracted
                    return eqGrounding(toGround, mapL, mapR, itL, toIntersectAtAnUpperLevelLeft/*,
                                       toIntersectAtAnUpperLevelRight, isAlwaysFromTheSameEventAndIntersectMap*/);
                }
            }
        }

    }
}

inline std::optional<std::unordered_map<std::string, DataPredicate>>
instantiateWithValues(const std::unordered_map<std::string, DataPredicate> &toGround,
                      const std::unordered_map<std::string, union_minimal> &mapL,
                      const std::unordered_map<std::string, union_minimal> &mapR,
                      std::unordered_map<std::string, std::vector<DataPredicate>>* isJoiningPass //,
                      //,
        /*std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelRight,
        bool isAlwaysFromTheSameEventAndIntersectMap*/) {
    DEBUG_ASSERT(!toGround.empty());
    std::unordered_map<std::string, DataPredicate> result;
    /**if (isAlwaysFromTheSameEventAndIntersectMap) {
        toIntersectAtAnUpperLevelLeft.clear();
        toIntersectAtAnUpperLevelRight.clear();
    }*/ std::unordered_map<std::string, std::vector<DataPredicate>> toIntersectAtAnUpperLevelLeft;
    for (const auto &cp: toGround) {
        auto tmp = instantiateWithValues(cp.second, mapL, mapR, toIntersectAtAnUpperLevelLeft/*,
                                         toIntersectAtAnUpperLevelRight, isAlwaysFromTheSameEventAndIntersectMap*/);
        if (tmp.isInvalid())
            return {};
        else if (tmp.isValid()) {
            DEBUG_ASSERT(!result.contains(cp.first));
            if (tmp.isValidButMissing())
                result.emplace(cp.first, DataPredicate{}); // inserting a true predicate
            else
                result.emplace(cp.first, tmp.get());       // inserting the digested predicate
        }
    }
    ///if (isAlwaysFromTheSameEventAndIntersectMap) {
    ///    DEBUG_ASSERT(toIntersectAtAnUpperLevelRight.empty());
    if (isJoiningPass)
        *isJoiningPass = toIntersectAtAnUpperLevelLeft;
        ///DEBUG_ASSERT(toIntersectAtAnUpperLevelLeft.empty());
    for (const auto &cp: toIntersectAtAnUpperLevelLeft) {
        if (!isJoiningPass) DEBUG_ASSERT(result.contains(cp.first));
        auto &ref = result[cp.first];
        for (const auto &item: cp.second) {
            if (!ref.intersect_with(item)) {
                return {};
            }
        }
    }
    ///}
    return {result};
}

inline std::optional<std::vector<std::unordered_map<std::string, DataPredicate>>>
instantiateWithValues(const std::vector<std::unordered_map<std::string, DataPredicate>> &toGround,
                      const std::unordered_map<std::string, union_minimal> &mapL,
                      const std::unordered_map<std::string, union_minimal> &mapR,
                      std::unordered_map<std::string, std::vector<DataPredicate>>* isJoiningPass/*,
                      std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelLeft,
                      std::unordered_map<std::string, std::vector<DataPredicate>> &toIntersectAtAnUpperLevelRight,
                      bool isAlwaysFromTheSameEventAndIntersectMap*/) {
    if (toGround.empty())
        return {toGround};
    std::vector<std::unordered_map<std::string, DataPredicate>> result;
    for (const auto &map: toGround) {
        auto tmp = instantiateWithValues(map, mapL, mapR, isJoiningPass/*, toIntersectAtAnUpperLevelLeft, toIntersectAtAnUpperLevelRight,
                                         isAlwaysFromTheSameEventAndIntersectMap*/);
        if (tmp)
            result.emplace_back(tmp.value());
    }
    if (result.empty())
        return {}; // Returning false, as all of the elemnts are computed to false
    else
        return {result};
}


void updateMapsFromWhereConjunction(
        const std::vector<std::unordered_map<std::string, DataPredicate>> &elementsExtractedFromTheWhere,
        std::vector<std::unordered_map<std::string, DataPredicate>> &resultMapToBeUpdated) {
    std::vector<std::unordered_map<std::string, DataPredicate>> novelElements;
    if (resultMapToBeUpdated.empty()) {
        resultMapToBeUpdated = elementsExtractedFromTheWhere;
    } else {
        if (!elementsExtractedFromTheWhere.empty()) {
            for (const auto& P : elementsExtractedFromTheWhere) {                // from (P || Q || ....), obtaining P
                for (const auto& R : resultMapToBeUpdated) { // from (R || S || ....), obtaining R
                    auto R_and_P = R;
                    bool hasFailed = false;
                    for (const auto& P1 : P) {              // each atom in P, from the first list, (P1 /\ P2 /\ P3 ....)
                        if ((P1.second.casusu == TTRUE) && (P1.second.BiVariableConditions.empty())) continue; // ignore
                        if (R_and_P.contains(P1.first)) {
                            if (!R_and_P[P1.first].intersect_with(P1.second)) {
                                hasFailed = true;
                                break;
                            } // else:
                        } else {
                            R_and_P[P1.first] = P1.second;
                        }
                    }
                    if (!hasFailed) {
                        novelElements.emplace_back(R_and_P);
                    }
                }
            }
            resultMapToBeUpdated = novelElements;
        }
    }
}

std::optional<DeclareDataAware> instantiateWithValues(
        const DeclareDataAware &toGround,
        const std::unordered_map<std::string, union_minimal> &mapLeft,
        const std::unordered_map<std::string, union_minimal> &mapRight
) {
    DeclareDataAware result{toGround};
    if ((!result.dnf_left_map.empty()) && (!mapLeft.empty())) {
        //std::unordered_map<std::string, std::vector<DataPredicate>> toIntersectAtAnUpperLevelLeft, toIntersectAtAnUpperLevelRight;
        auto tmp = instantiateWithValues(result.dnf_left_map, mapLeft, mapLeft, nullptr/*, toIntersectAtAnUpperLevelLeft,
                                         toIntersectAtAnUpperLevelRight, true*/);
        if (!tmp) return {};
        result.dnf_left_map = tmp.value();
    }

    if ((!result.dnf_right_map.empty()) && (!mapRight.empty())) {
        auto tmp = instantiateWithValues(result.dnf_right_map, mapRight, mapRight, nullptr/*, toIntersectAtAnUpperLevelLeft,
                                         toIntersectAtAnUpperLevelRight, true*/);
        if (!tmp) return {};
        result.dnf_right_map = tmp.value();
    }

    if (!result.conjunctive_map.empty()) {
        std::optional<std::vector<std::unordered_map<std::string, DataPredicate>>> tmp;
        {
            std::unordered_map<std::string, std::vector<DataPredicate>> toIntersectAtAnUpperLevelLeft;
            std::unordered_map<std::string, DataPredicate> v, w;
            tmp = instantiateWithValues(result.conjunctive_map, mapLeft, mapRight, &toIntersectAtAnUpperLevelLeft/*, toIntersectAtAnUpperLevelLeft,
                                         toIntersectAtAnUpperLevelRight, false*/);
            if (!tmp) return {};
            for (const auto &cp: toIntersectAtAnUpperLevelLeft) {
                for (const auto &baseCase: cp.second) {
                    std::unordered_map<std::string, DataPredicate>& u = baseCase.wasReversed ? w : v;
                    auto it = u.find(baseCase.var);
                    if (it != u.end()) {
                        if (!it->second.intersect_with(baseCase)) {
                            // if the evaluation reduces to false, the whole conjunction is false! therefore, returning it
                            return {};
                        } // otherwise, the intersection is already effective!
                    } else {
                        if (baseCase.isBiVariableCondition()) {
                            u[baseCase.var].var = baseCase.var;
                            u[baseCase.var].BiVariableConditions.emplace_back(baseCase);
                        } else {
                            u[baseCase.var] = baseCase;
                        }
                    }
                }
            }

            if (!v.empty())
            updateMapsFromWhereConjunction({v}, result.dnf_left_map);
            if (!w.empty())
            updateMapsFromWhereConjunction({w}, result.dnf_right_map);
        }

        ///result.conjunctive_map = tmp.value();
        result.conjunctive_map.clear();
        std::vector<std::unordered_map<std::string, DataPredicate>> onlyLeft, onlyRight;
        std::vector<std::unordered_map<std::string, DataPredicate>> neuLeft, neuRight;
        for (const auto& map : tmp.value()) {
            bool isForLeft = false;
            bool isForRight = false;
            DEBUG_ASSERT(!map.empty());
            for (const auto& varToPred : map) {
                if (varToPred.second.casusu == TTRUE)
                    DEBUG_ASSERT(varToPred.second.BiVariableConditions.empty());
                if (varToPred.second.casusu != TTRUE) {
                    if ((!varToPred.second.varRHS.empty()) && ((!varToPred.second.var.empty()))) {
                        DEBUG_ASSERT(!varToPred.second.wasReversed);
                        isForLeft = true;
                        isForRight = true;
                    } else if (varToPred.second.varRHS.empty()) {
                        isForRight = isForRight || varToPred.second.wasReversed;
                        isForLeft = isForLeft || !isForRight;
                    } else
                        DEBUG_ASSERT(false); // unexpected case
                }
            }
            if (isForLeft && isForRight)
                result.conjunctive_map.emplace_back(map);
            else if (isForLeft) {
                onlyLeft.emplace_back(map);
            } else if (isForRight) {
                onlyRight.emplace_back(map);
            } else
                DEBUG_ASSERT(false); // I shall not have true or false predicates at this stage!
        }
        updateMapsFromWhereConjunction(onlyLeft, result.dnf_left_map);
        updateMapsFromWhereConjunction(onlyRight, result.dnf_right_map);
    }

    for (auto& map : result.dnf_left_map) {
        for (auto& cp : map) {
            if (!cp.second.var.empty())
                cp.second.label = result.left_act;
            if (!cp.second.varRHS.empty())
                cp.second.labelRHS = result.left_act;
            for (auto& child : cp.second.BiVariableConditions) {
                if (!child.var.empty())
                    child.label = result.left_act;
                if (!child.varRHS.empty())
                    child.labelRHS = result.left_act;
            }
        }
    }

    for (auto& map : result.dnf_right_map) {
        for (auto& cp : map) {
            if (!cp.second.var.empty())
                cp.second.label = result.right_act;
            if (!cp.second.varRHS.empty())
                cp.second.labelRHS = result.right_act;
            for (auto& child : cp.second.BiVariableConditions) {
                if (!child.var.empty())
                    child.label = result.right_act;
                if (!child.varRHS.empty())
                    child.labelRHS = result.right_act;
            }
        }
    }

    DEBUG_ASSERT(result.conjunctive_map.empty());

    return result;
}