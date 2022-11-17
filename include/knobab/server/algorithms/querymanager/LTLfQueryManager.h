//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_LTLFQUERYMANAGER_H
#define KNOBAB_SERVER_LTLFQUERYMANAGER_H


#include <yaucl/numeric/ssize_t.h>
#include <knobab/server/algorithms/atomization/AtomizingPipeline.h>
#include <knobab/server/operators/base_ltlf.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

struct for_occurrence {
    DEFAULT_CONSTRUCTORS(for_occurrence)
    bool isTimed, isDisjunctiveSoNegated;
    LeafType type;
    size_t n_arg;
    for_occurrence(bool isTimed, LeafType type, size_t nArg);
    bool operator==(const for_occurrence &rhs) const;
    bool operator!=(const for_occurrence &rhs) const;
};

namespace std {
    template <>
    struct hash<struct for_occurrence>
    {
        std::size_t operator()(const for_occurrence& k) const
        {
            using yaucl::hashing::hash_combine;
            return hash_combine<bool>(hash_combine<size_t>(hash_combine<LeafType>(hash_combine<bool>(31, k.isTimed), k.type), k.n_arg), k.isDisjunctiveSoNegated);
        }
    };

}

struct MAXSatPipeline;

#include <iostream>
#include "LTLfQuery.h"

struct LTLfQueryManager {
    std::unordered_map<LTLfQuery, LTLfQuery*> conversion_map_for_subexpressions;
    std::map<size_t, std::vector<LTLfQuery*>> Q;
    std::unordered_map<LTLfQuery*, size_t> counter;
    std::vector<LTLfQuery*> atomsToDecomposeInUnion;
    size_t current_query_id = 0;
    std::vector<std::unordered_set<LTLfQuery*>> activations;
    const AtomizingPipeline* atomization;
    MAXSatPipeline *pipeline;

    void generateGraph(std::map<LTLfQuery*, std::vector<LTLfQuery*>>& ref, LTLfQuery*q) const;
    std::string generateGraph() const;
    nlohmann::json generateJSONGraph() const;

    /**
     * Clears the manager, so to get ready with another query plan to be analysed
     */
    void clear();

    std::unordered_map<for_occurrence, std::vector<std::set<std::string>>> focc_atomsets;
    std::unordered_map<for_occurrence, std::vector<LTLfQuery*>> focc_sub_formulae;

    LTLfQuery* alloc(LTLfQuery& orig, size_t qId) {
        orig.fields.id.parts.is_queryplan = true;
        for_occurrence key;
        key.type = orig.isLeaf;
        key.n_arg = orig.n;
        key.isTimed = orig.fields.id.parts.is_timed;
        key.isDisjunctiveSoNegated = orig.fields.id.parts.is_negated;
//        if (orig.t == LTLfQuery::AF_QPT)
//            std::cout << "DEBUG" << std::endl;
        LTLfQuery* ptr = nullptr;
        if (orig.args_from_script.empty() && (orig.t == LTLfQuery::EXISTS_QP)) {
            auto it = focc_atomsets.find(key);
            DEBUG_ASSERT(it != focc_atomsets.end());
            DEBUG_ASSERT(it->second.size() > 0);
            auto it2 = std::find(it->second.begin(), it->second.end(), orig.atom);
            DEBUG_ASSERT(it2 != it->second.end());
            ptr = focc_sub_formulae[key].at( it2 - it->second.begin());
        } else {
            for (auto& x : orig.args_from_script)
                orig.args.emplace_back(alloc(x, qId));
            orig.args_from_script.clear();
            ptr = simplify(orig);
        }
        if ((orig.isLeaf == ActivationLeaf) || ((!key.isTimed) && (orig.t == LTLfQuery::INIT_QP || orig.t == LTLfQuery::END_QP))) {
            DEBUG_ASSERT(activations.size() > qId);
            activations[qId].emplace(ptr);
//            if (current_query_id == activations.size()) { // Query Id counting from zero, so, if that happens, then it means that I need to add the activation in here!
//                activations.emplace_back().emplace(ptr);
//            } else {
//                activations.back().emplace(ptr);
//                //DEBUG_ASSERT( (activations.back() == tmp)); // By default, the activations should always refer to the same atom! That will make the assumption in the pipeline correct
//            }
        }
        return ptr;
    }

    // Union decomposition
    void finalizeUnions() {
        ////  this should be in some cases a disjunction,
        ////       and in other cases it should be a conjunction.
        ////       * When the atom was a negation, this was a disjunction
        ////       * When this was a positive one, it was a conjunction
//        std::unordered_map<for_occurrence, std::vector<LTLfQuery*>> focc_formula;
        LTLfQuery element_disjunction;
        for (auto& arg : focc_atomsets) {
            std::vector<LTLfQuery*> FF;
            remove_duplicates(arg.second);
            if (arg.second.empty()) continue;
            auto result = partition_sets(arg.second);
//            std::cout << result << std::endl;
//            std::cout << "-----" << std::endl;
            for (const auto& min_set : result.minimal_common_subsets) {
                element_disjunction.n = arg.first.n_arg;
                element_disjunction.isLeaf = arg.first.type;
                element_disjunction.fields.id.parts.is_negated = arg.first.isDisjunctiveSoNegated;
                element_disjunction.fields.id.parts.is_timed = arg.first.isTimed;
                element_disjunction.fields.id.parts.is_queryplan = true;
                element_disjunction.fields.id.parts.is_atom = true;
                element_disjunction.fields.id.parts.directly_from_cache = true;
                element_disjunction.fields.id.parts.has_theta = false;
                element_disjunction.t = LTLfQuery::EXISTS_QP;
                element_disjunction.atom = min_set;
                FF.emplace_back(simplify(element_disjunction));
            }
            for (const auto& min_composition : result.minimal_common_subsets_composition) {
                DEBUG_ASSERT(min_composition.size() > 0);
                element_disjunction.atom.clear();
                if (min_composition.size() == 1) {
                    FF.emplace_back(FF.at(*min_composition.begin()));
                } else if (min_composition.size() == 2) {
                    element_disjunction.isLeaf = NotALeaf;
                    element_disjunction.fields.id.parts.is_negated = false;
                    element_disjunction.fields.id.parts.is_timed = arg.first.isTimed;
                    element_disjunction.fields.id.parts.is_queryplan = true;
                    element_disjunction.fields.id.parts.is_atom = false;
                    element_disjunction.fields.id.parts.directly_from_cache = true;
                    element_disjunction.fields.id.parts.has_theta = false;
                    element_disjunction.t = arg.first.isDisjunctiveSoNegated ? LTLfQuery::OR_QP : LTLfQuery::AND_QP;
                    for (size_t id : min_composition) {
                        element_disjunction.args.emplace_back(FF.at(id));
                    }
                    FF.emplace_back(simplify(element_disjunction));
                    element_disjunction.args.clear();
                } else {
                    element_disjunction.isLeaf = NotALeaf;
                    element_disjunction.fields.id.parts.is_negated = false;
                    element_disjunction.fields.id.parts.is_timed = arg.first.isTimed;
                    element_disjunction.fields.id.parts.is_queryplan = true;
                    element_disjunction.fields.id.parts.is_atom = false;
                    element_disjunction.fields.id.parts.directly_from_cache = true;
                    element_disjunction.fields.id.parts.has_theta = false;
                    element_disjunction.t = arg.first.isDisjunctiveSoNegated ? LTLfQuery::OR_QP : LTLfQuery::AND_QP;
                    LTLfQuery* ptr = nullptr;
                    auto it = min_composition.begin();
                    for (size_t i = 0, N = min_composition.size(); i<N; i++) {
                        if (i<2) {
                            element_disjunction.args.emplace_back(FF.at(*it++));
                        } else {
                            ptr = simplify(element_disjunction);
                            element_disjunction.args.clear();
                            element_disjunction.args.emplace_back(ptr);
                            element_disjunction.args.emplace_back(FF.at(*it++));
                        }
                    }
                    FF.emplace_back(simplify(element_disjunction));
                    element_disjunction.args.clear();
                }
            }
            auto& v = focc_sub_formulae[arg.first];
            v.insert(v.begin(), arg.second.size(), nullptr);
            for (auto& min_composition : result.decomposedIndexedSubsets) {
                element_disjunction.atom.clear();
                if (min_composition.second->size() == 1) {
                    v[min_composition.first] = FF.at(*min_composition.second->begin());
                } else if (min_composition.second->size() == 2) {
                    element_disjunction.isLeaf = NotALeaf;
                    element_disjunction.fields.id.parts.is_negated = false;
                    element_disjunction.fields.id.parts.is_timed = arg.first.isTimed;
                    element_disjunction.fields.id.parts.is_queryplan = true;
                    element_disjunction.fields.id.parts.is_atom = false;
                    element_disjunction.fields.id.parts.directly_from_cache = true;
                    element_disjunction.fields.id.parts.has_theta = false;
                    element_disjunction.t = arg.first.isDisjunctiveSoNegated ? LTLfQuery::OR_QP : LTLfQuery::AND_QP;
                    for (const auto& id : *min_composition.second) {
                        element_disjunction.args.emplace_back(FF.at(id));
                    }
                    v[min_composition.first] = simplify(element_disjunction);
                    element_disjunction.args.clear();
                } else {
                    element_disjunction.isLeaf = NotALeaf;
                    element_disjunction.fields.id.parts.is_negated = false;
                    element_disjunction.fields.id.parts.is_timed = arg.first.isTimed;
                    element_disjunction.fields.id.parts.is_queryplan = true;
                    element_disjunction.fields.id.parts.is_atom = false;
                    element_disjunction.fields.id.parts.directly_from_cache = true;
                    element_disjunction.fields.id.parts.has_theta = false;
                    element_disjunction.t = arg.first.isDisjunctiveSoNegated ? LTLfQuery::OR_QP : LTLfQuery::AND_QP;
                    LTLfQuery* ptr = nullptr;
                    auto it = min_composition.second->begin();
                    for (size_t i = 0, N = min_composition.second->size(); i<N; i++) {
                        if (i<2) {
                            element_disjunction.args.emplace_back(FF.at(*it++));
                        } else {
                            ptr = simplify(element_disjunction);
                            element_disjunction.args.clear();
                            element_disjunction.args.emplace_back(ptr);
                            element_disjunction.args.emplace_back(FF.at(*it++));
                        }
                    }
                    v[min_composition.first] = simplify(element_disjunction);
                    element_disjunction.args.clear();
                }
            }
        }
//        for (const auto& kv : focc_atomsets) {
//            if (kv.second.empty()) continue;
//            auto it = focc_sub_formulae.at(kv.first);
//            for (size_t i = 0, N = kv.second.size(); i<N; i++) {
//                std::cout << kv.second.at(i) << std::endl;
//                std::cout << *it.at(i) << std::endl;
//                std::cout << "~~~~~~~~~" << std::endl;
//            }
//        }

//
//        for (const auto& ref : result.decomposedIndexedSubsets) {
////        auto& f = atomsToDecomposeInUnion.at(ref.first);
//        bool just = true;
//        LTLfQuery element_disjunction;
//        for (size_t i : *ref.second) {
//            if (i < isFromFurtherDecomposition) {
//                auto l = LTLfQuery::qEXISTS(1, DECLARE_TYPE_NONE, NoneLeaf, true, false);
//                l.fields.id.parts.is_atom = false;
//                l.atom.insert(result.minimal_common_subsets.at(i).begin(), result.minimal_common_subsets.at(i).end());
//                if (just) {
//                    element_disjunction = l;
//                    element_disjunction.fields.id.parts.is_timed = isTimed;
//                    element_disjunction.fields.id.parts.is_queryplan = true;
//                    just = false;
//                } else {
//                    element_disjunction = LTLfQuery::qOR(l, element_disjunction, true, false);
//                }
//            } else
//                for (size_t further : result.minimal_common_subsets_composition.at(i-isFromFurtherDecomposition)) {
//                    auto l = LTLfQuery::qEXISTS(1, DECLARE_TYPE_NONE, NoneLeaf, true, false);
//                    l.fields.id.parts.is_atom = false;
//                    l.atom.insert(result.minimal_common_subsets.at(further).begin(), result.minimal_common_subsets.at(further).end());
//                    if (just) {
//                        element_disjunction = l;
//                        element_disjunction.fields.id.parts.is_timed = isTimed;
//                        element_disjunction.fields.id.parts.is_queryplan = true;
//                        just = false;
//                    } else {
//                        element_disjunction = LTLfQuery::qOR(l, element_disjunction, true, false);
//                        element_disjunction.fields.id.parts.is_timed = isTimed;
//                        element_disjunction.fields.id.parts.is_queryplan = true;
//                    }
//                }
//
//        }
//        element_disjunction.fields.id.parts.is_timed = isTimed;
//        element_disjunction.fields.id.parts.is_queryplan = true;
//
////        LTLfQuery *q = simplifyRecursively(element_disjunction);
////        auto tmpValue = atomsToDecomposeInUnion[ref.first]->isLeaf;
////        q->isLeaf = tmpValue;
////
////        if (atomsToDecomposeInUnion[ref.first]->isDisjunctionOfExistentials()) {
////            // All of this code is correct, just because we always had this assumption to work with!
////            *atomsToDecomposeInUnion[ref.first] = *q;
////            //delete q; //this will not delete the other nodes, recursively. TODO: this should be done in clear() and avoid leaks
////            atomsToDecomposeInUnion[ref.first]->isLeaf = tmpValue;
////        } else {
////            LTLfQuery tmp = *atomsToDecomposeInUnion[ref.first];
////            tmp.args.emplace_back(q);
////            tmp.isLeaf = tmpValue;
////            LTLfQuery *q2 = simplifyRecursively(tmp);
////            q2->isLeaf = tmpValue;
////            *atomsToDecomposeInUnion[ref.first] = *q2;
////            //delete q; //this will not delete the other nodes, recursively. TODO: this should be done in clear() and avoid leaks
////            atomsToDecomposeInUnion[ref.first]->isLeaf = tmpValue;
////        }
//    }

    }

    LTLfQuery instantiate(const std::unordered_set<std::string>& atom,
                                          size_t formulaId,
                                          const LTLfQuery &input,
                                          const DeclareDataAware *joinCondition,
                                        const std::set<std::string> &data_atom,
                                          const std::set<std::string> &atom_universe,
                                          const std::set<std::string> &left,
                                          const std::set<std::string> &right) {

        bool firstOrLast = false;
        // Time assumptions on the Init and End operator!
        // Only untimed and non-negated operators are allowed
        if ((((input.t == LTLfQuery::INIT_QP) || (input.t == LTLfQuery::END_QP)))) {
            if (input.fields.id.parts.is_timed)
                throw std::runtime_error("ERROR: an init or end cannot be timed in this implementation!");
            else if (input.fields.id.parts.is_negated)
                throw std::runtime_error("ERROR: an init or end cannot be negated in this implementation!");
        } if ((((input.t == LTLfQuery::FIRST_QP) || (input.t == LTLfQuery::LAST_QP)))) {
            firstOrLast = true;
            if (input.fields.id.parts.is_negated)
                throw std::runtime_error("ERROR: an first or last cannot be negated in this implementation!");
        }

        LTLfQuery q;
        q.t = input.t;
        q.n = input.n;
        q.isLeaf = input.isLeaf;
        q.fields = input.fields;
        q.fields.id.parts.is_negated = true; // input.fields.id.parts.is_negated; /// TODO: BUGFIX, false always // I need to remember whether this was an intersection or a union, depending on the negation of the atom!
        q.isLeaf = input.isLeaf;
        for_occurrence key;
        key.isTimed = firstOrLast || input.fields.id.parts.is_timed;
        key.n_arg = input.n;
        key.type = input.isLeaf;
        key.isDisjunctiveSoNegated =  true; // input.fields.id.parts.is_negated; /// TODO: BUGFIX, false always
        auto& V = focc_atomsets[key];

        bool hasAtLeastOneDataAtom = false;
        if (!((q.t == LTLfQuery::FIRST_QP) || (q.t == LTLfQuery::LAST_QP))) {

        }

        if(!((q.t == LTLfQuery::FIRST_QP) || (q.t == LTLfQuery::LAST_QP))) {
            if ((input.declare_arg == DECLARE_TYPE_LEFT) || (input.declare_arg == DECLARE_TYPE_NONE)) {
                if (input.fields.id.parts.is_negated) {
                    for (const auto& x : atom_universe) {
                        if (!left.contains(x)) {
                            hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                            q.atom.insert(x);
//                        atomToFormulaId[x].emplace_back(formulaId);
                        }
                    }
                } else {
                    for (const auto& x : left) {
                        hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                        q.atom.insert(x);
//                    atomToFormulaId[x].emplace_back(formulaId);
                    }
                }
                if (q.t == LTLfQuery::EXISTS_QP) {
                    V.emplace_back(q.atom);
                }
            } else if (input.declare_arg == DECLARE_TYPE_RIGHT) {
                if (input.fields.id.parts.is_negated) {
                    for (const auto& x : atom_universe) {
                        if (!right.contains(x)) {
                            hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                            q.atom.insert(x);
//                        atomToFormulaId[x].emplace_back(formulaId);
                        }
                    }
                } else {
                    for (const auto& x : right) {
                        hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                        q.atom.insert(x);
//                    atomToFormulaId[x].emplace_back(formulaId);
                    }
                }
                if (q.t == LTLfQuery::EXISTS_QP) {
                    V.emplace_back(q.atom);
                }
            } else {
                //To be done at a future step: supporting three argument clauses
                if (q.fields.id.parts.is_atom) {
                    if(!((q.t == LTLfQuery::FIRST_QP) || (q.t == LTLfQuery::LAST_QP))) {
                        if (input.fields.id.parts.is_negated) {
                            for (const auto& x : atom_universe) {
                                if (!left.contains(x)) {
                                    hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                                    q.atom.insert(x);
//                                atomToFormulaId[x].emplace_back(formulaId);
                                }
                            }
                        } else {
                            for (const auto& x : left) {
                                hasAtLeastOneDataAtom = hasAtLeastOneDataAtom || data_atom.contains(x);
                                q.atom.insert(x);
//                            atomToFormulaId[x].emplace_back(formulaId);
                            }
                        }
                    }
                }
            }
        }

        if (input.fields.id.parts.has_theta) {
            if (input.doInvTheta && (joinCondition)) {
                DEBUG_ASSERT(joinCondition->isFlippedComputed);
                q.joinCondition = joinCondition->flipped_equivalent;
            } else {
                q.joinCondition = joinCondition;
            }
        }
        for (auto& args : input.args_from_script)
            q.args_from_script.emplace_back(instantiate(atom, formulaId, args, joinCondition, data_atom, atom_universe, left, right));
        q.fields.id.parts.is_queryplan = false;
//        q.fields.id.parts.is_negated = false;
        q.declare_arg = input.declare_arg; //DECLARE_TYPE_NONE;
        if ((q.t == LTLfQuery::ABSENCE_QP) && (hasAtLeastOneDataAtom || (q.atom.size()>1) || (input.fields.id.parts.is_timed))) {
            LTLfQuery trueAbsence;
            trueAbsence.t = LTLfQuery::NOT_QP;
            trueAbsence.n = input.n;
            trueAbsence.isLeaf = NotALeaf;
            trueAbsence.fields = input.fields;
            trueAbsence.fields.id.parts.is_atom = false;
            trueAbsence.fields.id.parts.is_negated = false; // after resolution, nothing is negated!
            trueAbsence.isLeaf = input.isLeaf;
            trueAbsence.fields.id.parts.is_queryplan = false;
            q.t = LTLfQuery::EXISTS_QP;
            q.n = 1;
            trueAbsence.args_from_script.emplace_back(q);
            V.emplace_back(q.atom);
            return trueAbsence;
        } else
            return q;
//        auto tmp = simplify(q);
//        if (q.isLeaf == ActivationLeaf) {
//            if (current_query_id == activations.size()) { // Query Id counting from zero, so, if that happens, then it means that I need to add the activation in here!
//                activations.emplace_back().emplace(tmp);
//            } else {
//                activations.back().emplace(tmp);
//                //DEBUG_ASSERT( (activations.back() == tmp)); // By default, the activations should always refer to the same atom! That will make the assumption in the pipeline correct
//            }
//        }
//        return tmp;
    }

    LTLfQuery *simplify(const std::unordered_set<std::string>& atom,
                        size_t formulaId,
                        const LTLfQuery &input,
                        const DeclareDataAware *joinCondition,
                        const std::unordered_set<std::string> &atom_universe,
                        const std::unordered_set<std::string> &left,
                        const std::unordered_set<std::string> &right,
                        std::vector<std::string> &toUseAtoms,
                        std::unordered_map<std::string , std::vector<size_t>>& atomToFormulaId);

    void sort_query_plan_for_scheduling(const AtomizingPipeline& ap, std::vector<LTLfQuery*>& W, KnowledgeBase* ptr);

private:

    LTLfQuery* simplify(const LTLfQuery& q);
//    LTLfQuery* simplifyRecursively(LTLfQuery &element_disjunction);
};



#endif //KNOBAB_SERVER_LTLFQUERYMANAGER_H
