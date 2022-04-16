//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_LTLFQUERY_H
#define KNOBAB_SERVER_LTLFQUERY_H


#include <set>
#include <cstddef>
#include <vector>
#include "semantics.h"

#include <yaucl/structures/bit_tagged_unions.h>
#include <yaucl/structures/default_constructors.h>
#include <knobab/server/declare/DeclareDataAware.h>


#define DECLARE_TYPE_NONE   (0)
#define DECLARE_TYPE_LEFT   (1)
#define DECLARE_TYPE_RIGHT  (2)
#define DECLARE_TYPE_MIDDLE  (3)

using declare_type_t = unsigned char;

TAGGED_UNION_WITH_ENCAPSULATION_BEGIN(unsigned char, bit_fields, 0, 8, bool has_theta : 1, bool preserve : 1, bool is_atom : 1, bool is_timed:1, bool is_negated:1, bool is_numbered:1, bool is_queryplan:1, bool directly_from_cache:1)
    (bool has_theta, bool preserve, bool is_atom, bool is_timed, bool is_negated, bool is_numbered, bool is_queryplan, bool directly_from_cache)  {
        id.parts.has_theta = has_theta;
        id.parts.preserve = preserve;
        id.parts.is_atom = is_atom;
        id.parts.is_timed = is_timed;
        id.parts.is_negated = is_negated;
        id.parts.is_numbered = is_numbered;
        id.parts.is_queryplan = is_queryplan;
        id.parts.directly_from_cache = directly_from_cache;
    }
TAGGED_UNION_ENCAPSULATOR_END

struct LTLfQuery {
    enum __attribute__((__packed__)) type {
        INIT_QP = 0,
        END_QP = 1,
        EXISTS_QP = 2,
        ABSENCE_QP = 3,
        NEXT_QP = 4,
        OR_QP = 5,
        AND_QP = 6,
        IMPL_QP = 7,
        IFTE_QP = 8,
        U_QP = 9,
        G_QP = 10,
        F_QP = 11,
        NOT_QP = 12,
        AF_QPT = 13,
        AXG_QPT = 14,
        AG_QPT = 15,
        FIRST_QP = 16,
        LAST_QP = 17,
        FALSEHOOD_QP = 18
    };
    type t;
    declare_type_t declare_arg = 0; // Representation of a specific argument providing data/label condition
    LeafType       isLeaf;           // Marking whether the query is going to be an activation or a target condition
    bit_fields fields;
    size_t n;                        // numeric_arg associated to the query type (exists, absence)

    bool isDisjunctionOfExistentials() const {
        if (args.empty() && args_from_script.empty()) {
            return t == type::EXISTS_QP;
        } else if (!args.empty()) {
            for (const auto &ref: args) if (!ref->isDisjunctionOfExistentials()) return false;
            return true;
        } else if (!args_from_script.empty()) {
            for (const auto &ref: args_from_script) if (!ref.isDisjunctionOfExistentials()) return false;
            return true;
        }
    }

    /// Arguments used while compiling the declare clauses description from the script
    std::vector<LTLfQuery> args_from_script;

    // AFTER THE COMPILATION OF THE QUERY PLAN. TODO: to be inserted in the definition of hashing and equality
    std::vector<LTLfQuery*> args;
    std::set<std::string> atom;
    std::set<size_t> partial_results;
    size_t result_id = 0;
    const DeclareDataAware* joinCondition;
    size_t parentMin = std::numeric_limits<size_t>::max(), parentMax = 0, dis = 0;
    Result result;
    void associateDataQueryIdsToFormulaByAtom(const std::string &x, size_t l) {
        if (atom.contains(x)) {
            partial_results.emplace(l);
        } else for (auto& child : args)
                child->associateDataQueryIdsToFormulaByAtom(x, l);
    }

    LTLfQuery() : t{FALSEHOOD_QP}, declare_arg{DECLARE_TYPE_NONE}, isLeaf{NoneLeaf}, n{0}, fields{0}, joinCondition{nullptr} {}
    DEFAULT_COPY_ASSGN(LTLfQuery)

    bool operator==(const LTLfQuery &rhs) const;
    bool operator!=(const LTLfQuery &rhs) const;

    static LTLfQuery qFIRST(LeafType marking);
    static LTLfQuery qLAST(LeafType marking);
    static LTLfQuery qINIT(short declare_argument, LeafType marking, bool isTimed);
    static LTLfQuery qEND(short declare_argument, LeafType marking, bool isTimed);
    static LTLfQuery qEXISTS(size_t narg, short declare_argument, LeafType marking, bool isTimed, bool isNegated);
    static LTLfQuery qABSENCE(size_t narg, short declare_argument, LeafType marking, bool isTimed);
    static LTLfQuery qNEXT(const LTLfQuery& arg, bool isTimed);
    static LTLfQuery qNOT(const LTLfQuery& arg, bool isTimed, bool preserve);
    static LTLfQuery qOR(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qIMPLICATION(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qIFTE(const LTLfQuery& lhs, const LTLfQuery& middle, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qAND(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qANDGLOBALLY(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qANDNEXTGLOBALLY(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qANDFUTURE(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qUNTIL(const LTLfQuery& lhs, const LTLfQuery& rhs, bool isTimed, bool hasTheta);
    static LTLfQuery qBOX(const LTLfQuery& lhs, bool isTimed);
    static LTLfQuery qDIAMOND(const LTLfQuery& lhs, bool isTimed);
};


#include <ostream>

std::ostream& operator<<(std::ostream& os, const LTLfQuery& );

#include <yaucl/hashing/vector_hash.h>
#include <yaucl/hashing/hash_combine.h>

namespace std {
    template <>
    struct hash<struct LTLfQuery>
    {
        std::size_t operator()(const LTLfQuery& k) const
        {
            using yaucl::hashing::hash_combine;
            size_t init = 31;
            for (const auto& x : k.args_from_script)
                init = hash_combine<LTLfQuery>(init, x);
            size_t f= hash_combine<size_t>(hash_combine<unsigned char>(hash_combine<short>(hash_combine<size_t>(init, k.t), k.isLeaf), k.fields.id.data), k.n);
            return f;
        }
    };

}

#endif //KNOBAB_SERVER_LTLFQUERY_H
