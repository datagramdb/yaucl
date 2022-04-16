//
// Created by giacomo on 16/04/2022.
//


#include <yaucl/functional/iterators.h>
#include <knobab/server/algorithms/grounding/grounding.h>
#include <knobab/server/algorithms/grounding/kb_grounding.h>
#include <yaucl/functional/assert.h>

GroundingStrategyConf::GroundingStrategyConf() : strategy1{ALWAYS_EXPAND_LESS_TOTAL_VALUES},  doPreliminaryFill{true}, trace_ids{}, I_A_x{}, ignoreActForAttributes{false}
{}


inline std::pair<std::unordered_set<std::string>,
        std::unordered_set<std::string>>
collectFromDeclare(GroundingStrategyConf &conf, const DeclareDataAware &d) {
    std::unordered_set<std::string> leftAttrSet, rightAttrSet;
    //auto& refLeft = conf.I_A_x[declare.left_act];
    auto cp =
            std::make_pair(d.collectLeftAttributes([&leftAttrSet](const auto& x) { leftAttrSet.insert(x);}, conf.creamOffSingleValues, true),
                    //auto& refRight = conf.I_A_x[declare.right_act];
                           d.collectRightAttributes([&rightAttrSet](const auto& x) { rightAttrSet.insert(x);}, conf.creamOffSingleValues, true));

    std::set<union_type> leftValues, rightValues;

    // Clearing the element, as I'm filling for each decleare now
    if (conf.ignoreActForAttributes) {
        conf.currentLeftAct.clear();
        conf.currentRightAct.clear();
        conf.OtherAttributes.insert(d.left_act);
        conf.OtherAttributes.insert(d.right_act);
    } else {
        conf.currentLeftAct = d.left_act;
        conf.currentRightAct = d.right_act;
        conf.ActToAttribute[d.left_act] = leftAttrSet;
        conf.ActToAttribute[d.right_act] = rightAttrSet;
    }
    return cp;
}

inline void fillForCartesian(std::vector<std::string> &names, std::vector<std::vector<union_type>> &toCartesianProduct,
                             std::unordered_set<std::string> &refL,
                             std::unordered_map<std::string, std::set<union_type>> &map) {
    for (const auto& name : refL) {
        auto it = map.find(name);
        if (it != map.end()) {
            names.emplace_back(name);
            toCartesianProduct.emplace_back(it->second.begin(), it->second.end());
        }
    }
}




inline DisjunctiveDeclareDataAware GroundWhereStrategy(GroundingStrategyConf& conf,
                                                       const KnowledgeBase& db,
                                                       const DeclareDataAware& d) {

    // Conditions over which I want to disable the grounding for the DeclareDataAware clause.
    // This will imply that we will have no
    if (d.conjunctive_map.empty() || (conf.strategy1 == GroundingStrategyConf::NO_EXPANSION))
        return {d};

    if (!conf.doPreliminaryFill) {
        conf.OtherAttributes.clear();
        conf.ActToAttribute.clear();
        std::tie(conf.LEFT, conf.RIGHT) = collectFromDeclare(conf, d);
        db.collectValuesFrom(conf.I_A_x, conf.I_x, {}, conf.ActToAttribute, conf.OtherAttributes);
    }

    std::string left_act = d.left_act, right_act = d.right_act;
    std::unordered_map<std::string, std::set<union_type>> I_X_left;
    std::unordered_map<std::string, std::set<union_type>> I_Y_right;

    if (conf.ignoreActForAttributes) {
        I_X_left = conf.I_x;
        if (conf.strategy1 == GroundingStrategyConf::ALWAYS_EXPAND_LESS_TOTAL_VALUES)
            I_Y_right = conf.I_x;
    } else {
        DEBUG_ASSERT(conf.I_A_x.contains(left_act));
        DEBUG_ASSERT(conf.I_A_x.contains(right_act));
        I_X_left = conf.I_A_x.at(left_act);
        if (conf.strategy1 == GroundingStrategyConf::ALWAYS_EXPAND_LESS_TOTAL_VALUES)
            I_Y_right = conf.I_A_x.at(right_act);
    }

    if (conf.creamOffSingleValues) {
        // TODO: TESTING IF THIS ACTUALLY WORKS! (begin)
        DEBUG_ASSERT(false);
#if 0
        auto S = d.toFiniteSemantics().nnf().possibleActionsUpToNext();
        if (S.containsInSet(left_act)) {
            filter_some_values_from_disjunctive_map(d.dnf_left_map, I_X_left);
        }
        if (S.containsInSet(right_act)) {
            filter_some_values_from_disjunctive_map(d.dnf_right_map, I_Y_right);
        }
#endif
        // TODO: (end)
    }

    // If there are some incompatibilites with the predicates, none of them will be a good solutuion
    if (I_X_left.empty() || I_Y_right.empty()) return {};

    // Selecting the most advantageous part for performing the expansion, id est, the one leading to less elements
    bool isLeft = true;
    if (conf.strategy1 == GroundingStrategyConf::ALWAYS_EXPAND_LESS_TOTAL_VALUES) {
        size_t leftSize = 1, rightSize = 1;
        bool leftEmpty = true, rightEmpty = true;
        for (const auto& element: I_X_left) {
            if (conf.LEFT.contains(element.first)) {
                leftEmpty = false;
                leftSize *= element.second.size();
            }
        }
        if (leftEmpty) return {};
        for (const auto& element: I_Y_right) {
            if (conf.RIGHT.contains(element.first)) {
                rightEmpty = false;
                rightSize *= element.second.size();
            }
        }
        if (rightEmpty) return {};
        if (rightSize < leftSize)
            isLeft = false;
    }

    std::vector<std::string> names;
    std::vector<std::vector<union_type>> toCartesianProduct;
    if (isLeft) {
        fillForCartesian(names, toCartesianProduct, conf.LEFT, I_X_left);
    } else {
        fillForCartesian(names, toCartesianProduct, conf.RIGHT, I_Y_right);
    }

    DisjunctiveDeclareDataAware result;
    auto cartesianResult = yaucl::iterators::cartesian(toCartesianProduct, [](const auto& x) {return true;});
    for (const auto& toMap : cartesianResult) {
        size_t N = toMap.size();
        DEBUG_ASSERT(N == names.size());
        std::unordered_map<std::string, union_minimal> map;
        for (size_t i = 0; i<N; i++) {
            auto val = toMap[i];
            if (std::holds_alternative<std::string>(val)) {
                map[names.at(i)] = std::get<std::string>(val);
            } else {
                if (std::holds_alternative<double>(val))
                    map[names.at(i)] = std::get<double>(val);
                else if (std::holds_alternative<size_t>(val))
                    map[names.at(i)] = (double)std::get<size_t>(val);
                else if (std::holds_alternative<long long>(val))
                    map[names.at(i)] = (double)std::get<long long>(val);
                else if (std::holds_alternative<bool>(val))
                    map[names.at(i)] = std::get<bool>(val) ? 1.0 : 0.0;
                else
                    throw std::runtime_error(std::to_string(val.index())+" Unexpected index type");
            }
        }

        if (isLeft) {
            auto tmp = instantiateWithValues(d, map, {});
            if (tmp)
                result.elementsInDisjunction.emplace_back(tmp.value());
        } else {
            auto tmp = instantiateWithValues(d, {}, map);
            if (tmp)
                result.elementsInDisjunction.emplace_back(tmp.value());
        }
    }

    return result;
}

CNFDeclareDataAware GroundWhereStrategy(GroundingStrategyConf& conf,
                                        const KnowledgeBase& db,
                                        const std::vector<DeclareDataAware>& declare) {

    /**
     * Scanning the Knowledge Base for attributes via declare, before
     */
    if (conf.doPreliminaryFill) {
        conf.OtherAttributes.clear();
        conf.ActToAttribute.clear();
        for (const auto& d : declare) {
            auto it = collectFromDeclare(conf, d);
            conf.LEFT.insert(it.first.begin(), it.first.end());
            conf.RIGHT.insert(it.second.begin(), it.second.end());
        }
        db.collectValuesFrom(conf.I_A_x, conf.I_x, {}, conf.ActToAttribute, conf.OtherAttributes);
    }

    CNFDeclareDataAware cnf;
    for (const auto& ref : declare) {
        auto V = GroundWhereStrategy(conf, db, ref);
        if (!V.elementsInDisjunction.empty())
            cnf.singleElementOfConjunction.emplace_back(V);
    }
    return cnf;
}
