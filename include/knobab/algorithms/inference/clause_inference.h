//
// Created by giacomo on 10/11/22.
//

#ifndef KNOBAB_SERVER_CLAUSE_INFERENCE_H
#define KNOBAB_SERVER_CLAUSE_INFERENCE_H

#include "knobab/server/declare/DeclareDataAware.h"

enum ClauseInferenceOutcome {
    Unknown,
    ImpliesLeft,
    ImpliesRight,
    Inconsistent,
    Annihilate
};

static inline ClauseInferenceOutcome flipOutcome(ClauseInferenceOutcome outcome) {
    switch (outcome) {
        case ImpliesLeft:
            return ImpliesRight;
        case ImpliesRight:
            return ImpliesLeft;
        default:
            return outcome;
    }
}

static inline ClauseInferenceOutcome negateOutcome(ClauseInferenceOutcome outcome) {
    switch (outcome) {
        case ImpliesLeft:
        case ImpliesRight:
            return Inconsistent;
        default:
            return Unknown;
    }
}

#include <functional>
#include "yaucl/structures/set_operations.h"


ClauseInferenceOutcome infer(const DeclareDataAware& atomized_left,
                             const DeclareDataAware& atomized_right,
                             std::vector<DeclareDataAware>& further_inferred_clauses) {
    size_t n_args;
    std::function<size_t(size_t, size_t)> lambdaForImpl = [](size_t a, size_t b){ return std::min(a,b);};
    std::function<void(const DeclareDataAware& premise, const DeclareDataAware& consequence, std::vector<DeclareDataAware>&)> extendModelWith = [](const DeclareDataAware&, const DeclareDataAware&, std::vector<DeclareDataAware>& ){ return ;};

    bool leadsToInconsistency;
    bool swapActivations;
    bool doTest;
    if ((atomized_left.casusu == atomized_right.casusu)) {
        if ((atomized_left.casusu == "ChainPrecedence")
            || (atomized_left.casusu == "ChainResponse")
            || (atomized_left.casusu == "ChainSuccession")) {
            n_args = 2;
            leadsToInconsistency = true;
            swapActivations = true;
            doTest = true;
        } else if ((atomized_left.casusu == "Init") || (atomized_left.casusu == "Ends")) {
            n_args = 1;
            leadsToInconsistency = true;
            swapActivations = false;
            doTest = true;
        } else if (atomized_left.casusu == "Exists") {
            n_args = 1;
            leadsToInconsistency = false;
            swapActivations = false;
            doTest = true;
        } else if (atomized_left.casusu == "Absence") {
            n_args = 1;
            leadsToInconsistency = false;
            lambdaForImpl = [](size_t a, size_t b){ return std::max(a,b);};
            swapActivations = true;
            doTest = true;
        } else {
            doTest = false;
        }
    } else {

        // Exists and absence are mutually exclusive
        if ((atomized_left.casusu == "Exists") && (atomized_right.casusu == "Absence")) {
            auto copy = atomized_right;
            copy.casusu = "Exists";
            return negateOutcome(infer(atomized_left, copy, further_inferred_clauses));
        } else if ((atomized_right.casusu == "Exists") && (atomized_left.casusu == "Absence")) {
            auto copy = atomized_left;
            copy.casusu = "Exists";
            return negateOutcome(infer(atomized_right, copy, further_inferred_clauses));
        }

        if (((atomized_left.casusu == "Exists") || (atomized_left.casusu == "Init") || (atomized_left.casusu == "Ends")) &&
                (atomized_right.casusu == "RespExistence")) {
            doTest = true;
            n_args = 1;
            leadsToInconsistency = false;
            swapActivations = true;
            extendModelWith = [](const DeclareDataAware& ends,
                                 const DeclareDataAware& precedence,
                                 std::vector<DeclareDataAware>& thing){
                DeclareDataAware absence;
                absence.n = 1;
                absence.casusu = "Exists";
                absence.left_act = precedence.right_act;
                absence.dnf_left_map = precedence.dnf_left_map;
                absence.left_decomposed_atoms = precedence.right_decomposed_atoms;
                thing.emplace_back(absence);
            };
        } else if (((atomized_right.casusu == "Exists") || (atomized_right.casusu == "Init") || (atomized_right.casusu == "Ends")) &&
              (atomized_left.casusu == "RespExistence")) {
            return flipOutcome(infer(atomized_right, atomized_left, further_inferred_clauses));
        }

            // If the Response activates the NotSuccession, then the NotSuccession's activation shall
        // never appear.
        else if ((atomized_left.casusu == "Response") && (atomized_right.casusu == "NotSuccession")) {
            auto copy = atomized_right;
            copy.casusu = "Response";
            if (infer(atomized_left, copy, further_inferred_clauses) == ImpliesLeft) {
                DeclareDataAware generation;
                generation.n = 1;
                generation.casusu = "Absence";
                generation.left_act = atomized_right.left_act;
                generation.dnf_left_map = atomized_right.dnf_left_map;
                generation.left_decomposed_atoms = atomized_right.left_decomposed_atoms;
                further_inferred_clauses.emplace_back(generation);
                return Annihilate;
            }
        } else if ((atomized_right.casusu == "NotSuccession") && (atomized_left.casusu == "Response")) {
            return infer(atomized_right, atomized_left, further_inferred_clauses);
        }

        // If I know for sure that the model starts with the activation of precedence,
        // then everything boils down to the Init
        else if ((atomized_left.casusu == "Init") && (atomized_right.casusu == "Precedence")) {
            doTest = true;
            n_args = 1;
            leadsToInconsistency = false;
            swapActivations = true;
        } else if ((atomized_right.casusu == "Init") && (atomized_left.casusu == "Precedence")) {
            return flipOutcome(infer(atomized_right, atomized_left, further_inferred_clauses));
        }

        // If I know that a trace ends with a given activation that is also shared with the precedence,
        // then I know that the target shall be always be absent from the trace and the only ends
        // is sufficient
        else if ((atomized_left.casusu == "Ends") && (atomized_right.casusu == "Precedence")) {
            doTest = true;
            n_args = 1;
            leadsToInconsistency = false;
            swapActivations = true;
            extendModelWith = [](const DeclareDataAware& ends,
                                 const DeclareDataAware& precedence,
                                 std::vector<DeclareDataAware>& thing){
                DeclareDataAware absence;
                absence.n = 1;
                absence.casusu = "Absence";
                absence.left_act = precedence.right_act;
                absence.dnf_left_map = precedence.dnf_left_map;
                absence.left_decomposed_atoms = precedence.right_decomposed_atoms;
                thing.emplace_back(absence);
            };
        } else if ((atomized_right.casusu == "Ends") && (atomized_left.casusu == "Precedence")) {
            return flipOutcome(infer(atomized_right, atomized_left, further_inferred_clauses));
        }

        else {
            doTest = false;
        }
    }
    if (!doTest)
        return Unknown;
    size_t minarg = lambdaForImpl(atomized_left.n, atomized_right.n);
    bool leftImpliesRight_L = isSubsetOf(atomized_left.left_decomposed_atoms,
                                         atomized_right.left_decomposed_atoms);
    bool rightImpliesLeft_L = isSubsetOf(atomized_right.left_decomposed_atoms,
                                         atomized_left.left_decomposed_atoms);
    bool leftImpliesRight_R = (n_args==1) || isSubsetOf(atomized_left.right_decomposed_atoms,
                                                        atomized_right.right_decomposed_atoms);
    bool rightImpliesLeft_R= (n_args==1) || isSubsetOf(atomized_right.right_decomposed_atoms,
                                                       atomized_left.right_decomposed_atoms);
    if (swapActivations) {
        std::swap(leftImpliesRight_L, rightImpliesLeft_L);
    }
    if ((minarg == atomized_left.n) && leftImpliesRight_L && leftImpliesRight_R) {
        extendModelWith(atomized_left, atomized_right, further_inferred_clauses);
        return ImpliesRight;
    } else if ((minarg == atomized_right.n) && rightImpliesLeft_L && rightImpliesLeft_R) {
        extendModelWith(atomized_right, atomized_left, further_inferred_clauses);
        return ImpliesLeft;
    } else if (!leadsToInconsistency)
        return Unknown;
    else if (leadsToInconsistency && (leftImpliesRight_L))
        return Inconsistent;
}

#endif //KNOBAB_SERVER_CLAUSE_INFERENCE_H
