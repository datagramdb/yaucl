/*
 * ltlf.cpp
 * This file is part of bpm21
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 18/02/21.
//

#include <yaucl/bpm/structures/ltlf/ltlf.h>
#include <yaucl/functional/assert.h>
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

ltlf::ltlf() : casusu{TRUE}, is_negated{false}, is_compound_predicate{false}, is_exclusive{false} { }

ltlf::ltlf(const std::string &act) : casusu{ACT}, act{act}, is_negated{false}, is_compound_predicate{false}, is_exclusive{false}  {}

ltlf::ltlf(formula_t citki) : casusu{citki}, is_negated{false}, is_compound_predicate{false}, is_exclusive{false}   {}

struct ltlf ltlf::True() { return {}; }

struct ltlf ltlf::Act(const std::string &act) { return {act}; }

struct ltlf ltlf::Neg(const ltlf &sub) {
    ltlf formula{NEG_OF};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Next(const ltlf &sub) {
    ltlf formula{NEXT};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Or(const ltlf &left, const ltlf &right) {
    struct ltlf formula{OR};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    formula.is_exclusive = false;
    return formula;
}

struct ltlf ltlf::And(const ltlf &left, const ltlf &right) {
    struct ltlf formula{AND};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Diamond(const ltlf &sub, bool isForGraphs) {
    if (isForGraphs) return Until(ltlf::True(), sub);
    ltlf formula{DIAMOND};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Box(const ltlf &sub, bool isForGraphs) {
    if (isForGraphs) return  Release({FALSE}, sub);
    ltlf formula{BOX};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Last() {
    return Neg(Next(True()));
}

struct ltlf ltlf::Until(const ltlf &left, const ltlf &right) {
    struct ltlf formula{UNTIL};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Release(const ltlf &left, const ltlf &right) {
    struct ltlf formula{RELEASE};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Implies(const ltlf &left, const ltlf &right, bool isGraphGeneration) {
    if (isGraphGeneration) {
        return Or(Neg(left), right);
    } else {
        auto res = Or(Neg(left), And(left, right));
        res.is_exclusive = true;
        return res;
    }
}

struct ltlf ltlf::Equivalent(const ltlf &left, const ltlf &right, bool isForGraphs) {
    return And(Implies(left, right, isForGraphs), Implies(right, left, isForGraphs));
}

struct ltlf ltlf::WeakUntil(const ltlf &left, const ltlf &right, bool isForGraphs) {
    return Or(Until(left, right), Box(left, isForGraphs));
}


std::ostream &operator<<(std::ostream &os, const ltlf &syntax) {
    std::string reset = "";
    if (syntax.is_negated)
        os << "!";
    switch (syntax.casusu) {
        case ACT:
            return os << syntax.act << reset;
        case NUMERIC_ATOM:
            return os << syntax.numeric_atom<< reset;
        case NEG_OF:
            return os << "(!(" << syntax.args[0] << "))"<< reset;
        case OR:
            return os << "(" << syntax.args[0] << ") | (" << syntax.args[1] << ')'<< reset;
        case AND:
            return os << "(" << syntax.args[0] << ") & (" << syntax.args[1] << ')'<< reset;
        case NEXT:
            return os << "X(" << syntax.args[0] << ")"<< reset;
        case UNTIL:
            return os << "(" << syntax.args[0] << ") U (" << syntax.args[1] << ')'<< reset;
        case RELEASE:
            return os << "(" << syntax.args[0] << ") R (" << syntax.args[1] << ')'<< reset;
        case TRUE:
            return os << "true"<< reset;
        case BOX:
            return os << "G(" << syntax.args[0] << ")"<< reset;
        case DIAMOND:
            return os << "F(" << syntax.args[0] << ")"<< reset;
        default:
            return os << "false"<< reset;
    }
}

void ltlf::collectElements(std::unordered_map<std::string, std::unordered_set<bool>> &negation) const {
    DEBUG_ASSERT(casusu != NUMERIC_ATOM);
    if (casusu == ACT) {
        negation[act].insert(is_negated);
    } else {
        for (const auto& ref : args)
            ref.collectElements(negation);
    }
}

std::unordered_set<std::string> ltlf::mark_join_condition(const std::string& left, const std::string& right) {
    switch (casusu) {
        case ACT: {
            if ((!is_negated) && ((act == left) || (act == right))) {
                if (act == left) {
                    leafType = ActivationLeaf;
                    return {act};
                } else if (act == right) {
                    leafType = TargetLeaf;
                    return {act};
                }
            }
            return {};
        }
        case NEG_OF:
            DEBUG_ASSERT(false);
        case NEXT:
        case DIAMOND:
        case BOX:
            return args.at(0).mark_join_condition(left, right);
        case OR:
        case AND:
        case UNTIL:
        case RELEASE:
        {
            auto L = args.at(0).mark_join_condition(left, right);
            auto R = args.at(1).mark_join_condition(left, right);
            L.insert(R.begin(), R.end());
            if (L.contains(left) && R.contains(right))
                is_join_condition_place = true;
            return L;
        }
        default:
            return {};
    }
}

ltlf ltlf::replace_with(const std::unordered_map<std::pair<bool, std::string>, std::unordered_set<std::string>> &map,
                        bool isForGraph) {
    if (map.empty()) return *this;
    switch (casusu) {
        case ACT:
        {
            auto it = map.find({is_negated, act});
            DEBUG_ASSERT(it != map.end());
            auto a = *this;
            a.leafType = leafType;
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(a.rewritten_act));
            return a;
        }

        case NEG_OF:
            return ltlf::Neg(args.at(0).replace_with(map, isForGraph));
        case NEXT:
            return ltlf::Next(args.at(0).replace_with(map, isForGraph));
        case DIAMOND:
            return ltlf::Diamond(args.at(0).replace_with(map, isForGraph), isForGraph);
        case BOX:
            return ltlf::Box(args.at(0).replace_with(map, isForGraph), isForGraph);
        case OR:
        {
            auto tmp =              ltlf::Or(args.at(0).replace_with(map, isForGraph),
                                                   args.at(1).replace_with(map, isForGraph));
            tmp.is_exclusive = is_exclusive;
            tmp.is_join_condition_place = is_join_condition_place && (!tmp.is_exclusive);
            return tmp;
        }
        case AND: {
            auto tmp =ltlf::And(args.at(0).replace_with(map, isForGraph),
                                args.at(1).replace_with(map, isForGraph));
            tmp.is_join_condition_place = is_join_condition_place;
            return tmp;
        }
        case UNTIL:{
            auto tmp = ltlf::Until(args.at(0).replace_with(map, isForGraph),
                               args.at(1).replace_with(map, isForGraph));
            tmp.is_join_condition_place = is_join_condition_place;
            return tmp;
        }
        case RELEASE: {
            auto tmp = ltlf::Release(args.at(0).replace_with(map, isForGraph),
                                     args.at(1).replace_with(map, isForGraph));
            tmp.is_join_condition_place = is_join_condition_place;
            return tmp;
        }
        case TRUE:
        case FALSE:
            return *this;
        default:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}

struct ltlf ltlf::replace_with(const std::unordered_map<std::string, ltlf> &map) const {
    if (map.empty()) return *this;
    switch (casusu) {
        case ACT:
            if (map.contains(act)) {
                auto f = map.at(act);
                if (is_negated)
                    f = f.negate();
                return f.nnf();
            } else {
                DEBUG_ASSERT(false);
            }

        case NEG_OF:
            return ltlf::Neg(args.at(0).replace_with(map));
        case NEXT:
            return ltlf::Next(args.at(0).replace_with(map));
        case DIAMOND:
            return ltlf::Diamond(args.at(0).replace_with(map));
        case BOX:
            return ltlf::Box(args.at(0).replace_with(map));
        case OR:
            return ltlf::Or(args.at(0).replace_with(map),
                            args.at(1).replace_with(map));
        case AND:
            return ltlf::And(args.at(0).replace_with(map),
                             args.at(1).replace_with(map));

        case UNTIL:
            return ltlf::Until(args.at(0).replace_with(map),
                               args.at(1).replace_with(map));
        case RELEASE:
            return ltlf::Release(args.at(0).replace_with(map),
                                 args.at(1).replace_with(map));
        case TRUE:
        case FALSE:
            return *this;
        default:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}


struct ltlf ltlf::replace_with_unique_name(const std::unordered_map<std::string, std::string> &map) const {
    if (map.empty()) return *this;
    switch (casusu) {
        case ACT:
            if (map.contains(act)) {
                auto f = ltlf::Act(map.at(act));
                if (is_negated)
                    f = f.negate();
                return f;
            } else {
                DEBUG_ASSERT(false);
            }

        case NEG_OF:
            return ltlf::Neg(args.at(0).replace_with_unique_name(map));
        case NEXT:
            return ltlf::Next(args.at(0).replace_with_unique_name(map));
        case DIAMOND:
            return ltlf::Diamond(args.at(0).replace_with_unique_name(map));
        case BOX:
            return ltlf::Box(args.at(0).replace_with_unique_name(map));
        case OR:
            return ltlf::Or(args.at(0).replace_with_unique_name(map),
                            args.at(1).replace_with_unique_name(map));
        case AND:
            return ltlf::And(args.at(0).replace_with_unique_name(map),
                             args.at(1).replace_with_unique_name(map));

        case UNTIL:
            return ltlf::Until(args.at(0).replace_with_unique_name(map),
                               args.at(1).replace_with_unique_name(map));
        case RELEASE:
            return ltlf::Release(args.at(0).replace_with_unique_name(map),
                                 args.at(1).replace_with_unique_name(map));
        case TRUE:
        case FALSE:
            return *this;
        default:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}

bool ltlf::easy_interpret(const std::string &map) const {
    switch (casusu) {
        case TRUE:
            return true;
        case FALSE:
            return false;
        case ACT:
            if (!is_negated) {
                bool res = (map == (act));
                return res;
            } else {
                bool res = !(map == (act));
                return res;
            }
        case NEG_OF:
            return !args.at(0).easy_interpret(map);
        case OR: {
            return args.at(0).easy_interpret(map) || args.at(1).easy_interpret( map);
        }
        case AND: {
            return args.at(0).easy_interpret(map) && args.at(1).easy_interpret( map);
        }
        case NEXT:
            return args.at(0).easy_interpret(map); // Stops the interpretation before the current element
        default:
            DEBUG_ASSERT(false);
            return false;
    }
}

struct ltlf ltlf::oversimplify() const {
    return *this;
}

void ltlf::collectStructuralElements(formula_t type, std::vector<ltlf> &set, bool simplificationType) const {
    if (type != casusu) return;
    else for (const auto& arg : args) {
            if (arg.casusu == type) arg.collectStructuralElements(type, set, simplificationType);
            else set.emplace_back(simplificationType ? arg.oversimplify() : arg.simplify());
        }
}

bool ltlf::containsElement(formula_t type, const ltlf& item, bool simplificationType) const {
    if (type != casusu) return false;
    else {
        for (const auto& arg : args) {
            if (arg.casusu == type) if (arg.containsElement(type, item, simplificationType)) return true;
                else {
                    auto v = simplificationType ? arg.oversimplify() : arg.simplify();
                    if (v == item) return true;
                }
        }
        return false;
    }
}

/*struct ltlf ltlf::_interpret2(const std::unordered_set<struct ltlf> &map) const {
    switch (casusu) {
        case ACT: {
            if (map.contains(*this))
                return True();
        }
            return map.contains(*this) ? True() : ltlf{FALSE};
        case NEG_OF:
        case UNTIL:
        case RELEASE:
            DEBUG_ASSERT(false);
        case OR: {
            return Or(args.at(0)._interpret2(map), args.at(1)._interpret2(map)).simplify();
        }
        case AND: {
            return And(args.at(0)._interpret2(map), args.at(1)._interpret2(map)).simplify();
        }
        case NEXT:
            return args.at(0); // Stops the interpretation before the current element
        default:
            return {*this};
    }
}*/

struct ltlf ltlf::_isPotentialFinalState() const {
    switch (casusu) {
        case ACT:
        case TRUE:
        case FALSE:
        case NEG_OF:
        case NEXT:
        case UNTIL:
        case NUMERIC_ATOM:
            return {FALSE};

        case OR:
            return Or(args.at(0)._isPotentialFinalState(), args.at(1)._isPotentialFinalState()).nnf().simplify();

        case AND:
            return And(args.at(0)._isPotentialFinalState(), args.at(1)._isPotentialFinalState()).nnf().simplify();

        case RELEASE:
            return True();

        default:
            throw std::runtime_error("Unexpected case");
    }
}

#include <cassert>


void ltlf::_actionsUpToNext(PropositionalizedAtomsSet &atoms, bool isTerminal) const {
    switch (casusu) {
        case NUMERIC_ATOM:
            break;
        case ACT: {}
            atoms.insert(act, isTerminal);
            break;
        case NEG_OF:
            std::cerr << "Error: this should be always called after negation normal form, so I should not be able to call this" << std::endl;
            DEBUG_ASSERT(false);
        case NEXT:
            return;
            //args.at(0)._actionsUpToNext(atoms, false);
        case TRUE:
        case FALSE:
            break;
        case OR:
        case AND:
            args.at(0)._actionsUpToNext(atoms, isTerminal);
            args.at(1)._actionsUpToNext(atoms, isTerminal);
            break;
        case UNTIL:
        case RELEASE:
            std::cerr << "Error: this should be always called after next normal form, so I should not be able to call this" << std::endl;
            DEBUG_ASSERT(false);
    }
}

struct ltlf ltlf::_interpret(const std::unordered_set<std::string>& map) const {
    switch (casusu) {
        case ACT:
            if (!is_negated) {
                return map.contains(act) ? True() : ltlf{FALSE};
            } else {
                return map.contains(act) ? ltlf{FALSE}: True();
            }
        case NEG_OF:
        case UNTIL:
        case RELEASE:
            DEBUG_ASSERT(false);
        case OR: {
            return Or(args.at(0)._interpret(map), args.at(1)._interpret(map)).simplify();
        }
        case AND: {
            return And(args.at(0)._interpret(map), args.at(1)._interpret(map)).simplify();
        }
        case NEXT:
            return args.at(0); // Stops the interpretation before the current element
        default:
            return {*this};
    }
}

PropositionalizedAtomsSet ltlf::possibleActionsUpToNext() const {
    PropositionalizedAtomsSet result;
    nnf().simplify().oversimplify().stepwise_expand()._actionsUpToNext(result, true);
    return result;
}

std::unordered_set<std::string> ltlf::propositionalize() const {
    std::unordered_set<std::string> result;
    simplify()._propositionalize(result);
    return result;
}


void ltlf::_propositionalize(std::unordered_set<std::string > &atoms, bool blockNext) const {
    switch (casusu) {
        case ACT: {}
            atoms.insert({act});
            break;
        case NEG_OF:
        case NEXT:
            if (!blockNext)
                args.at(0)._propositionalize(atoms, blockNext);
            break;
        case OR:
        case AND:

        case UNTIL:  //TODO
        case RELEASE://TODO
            args.at(0)._propositionalize(atoms, blockNext);
            args.at(1)._propositionalize(atoms, blockNext);
            break;
            /*Original
             *
             * case UNTIL:
            case RELEASE:
                if (!atoms.contains(*this)) {
                    atoms.insert(*this);
                    stepwise_expand()._propositionalize(atoms, blockNext);
                }
                break;*/
        case TRUE:
        case FALSE:
            break;
        case NUMERIC_ATOM:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}

struct ltlf ltlf::stepwise_expand() const {
    switch (casusu) {
        case NEXT: {
            return ltlf::Next(args.at(0).simplify());
        }
        case OR: {
            auto left = args.at(0).simplify()
                    .stepwise_expand();
            auto right = args.at(1).simplify().stepwise_expand();
            return ltlf::Or(left, right).simplify();
        }
        case AND: {
            auto left = args.at(0).simplify().stepwise_expand();
            auto right = args.at(1).simplify().stepwise_expand();
            return ltlf::And(left, right).simplify();
        }
        case NEG_OF:
            return args.at(0).negate();
        case UNTIL: {
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            return ltlf::Or(right, ltlf::And(left, ltlf::Next(*this))).simplify().stepwise_expand();
        }
        case RELEASE:{
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            return ltlf::And(right, ltlf::Or(left, ltlf::Next(*this))).simplify().stepwise_expand();
        }

        default:
            return {*this};
    }
}

bool ltlf::operator==(const ltlf &rhs) const {
    bool ca = casusu == rhs.casusu;
    if (!ca) return false;

    bool preliminary = (act == rhs.act) &&(is_negated == rhs.is_negated) && (rewritten_act == rhs.rewritten_act) && (joinCondition == rhs.joinCondition) && (leafType == rhs.leafType);
    if (!preliminary) return false;

    switch (casusu) {
        case LAST:
        case TRUE:
        case FALSE:
        case ACT:
        case NEXT:
        case UNTIL:
        case RELEASE:
        case NEG_OF:
        case BOX:
        case DIAMOND:
            return (args == rhs.args);

        case NUMERIC_ATOM:
            return (numeric_atom == rhs.numeric_atom);

        case AND:
        case OR: {
            std::vector<ltlf> leftS, rightS;
            collectStructuralElements(casusu, leftS, true);
            rhs.collectStructuralElements(casusu, rightS, true);
            for (const auto& arg : leftS)
                if (std::find(rightS.begin(), rightS.end(), arg) == rightS.end()) return false;
            for (const auto & arg : rightS)
                if (std::find(leftS.begin(), leftS.end(), arg) == leftS.end()) return false;
            return true;
        }

        default:
            throw std::runtime_error("Unexpected case");
    }

}

bool ltlf::operator!=(const ltlf &rhs) const {
    return !(rhs == *this);
}

struct ltlf ltlf::simplify() const {
    return *this;
}



#include <tuple>




struct ltlf ltlf::negate(bool isGraph) const {
    switch (casusu) {
        case ACT: {
            struct ltlf curr = *this;
            curr.is_negated = !curr.is_negated;
            return curr.setBeingCompound(is_compound_predicate);
        }
        case NUMERIC_ATOM: {
            DataPredicate local = numeric_atom;
            switch (numeric_atom.casusu) {
                case LT:
                    local.casusu = GEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case GT:
                    local.casusu = LEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case GEQ:
                    local.casusu = LT;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case LEQ:
                    local.casusu = GT;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case EQ:
                    local.casusu = NEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case NEQ:
                    local.casusu = EQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case INTERVAL: {
                    ltlf formula = ltlf::True().negate();
                    bool isFormulaSet = false;

                    bool isString = std::holds_alternative<std::string>(numeric_atom.value);
                    union_minimal l_min, l_max;
                    if (isString) {
                        l_min = DataPredicate::MIN_STRING;
                        l_max = DataPredicate::MAX_STRING;
                    } else {
                        l_min = DataPredicate::MIN_DOUBLE;
                        l_max = DataPredicate::MAX_DOUBLE;
                    }

                    if (numeric_atom.value != l_min) {
                        isFormulaSet = true;
                        DataPredicate min;
                        min.value = l_min;
                        min.value_upper_bound = DataPredicate::prev_of(numeric_atom.value);
                        min.var = numeric_atom.var;
                        min.label = numeric_atom.label;
                        formula = ltlf::Interval(min);
                    }
                    if (numeric_atom.value_upper_bound != l_max) {
                        DataPredicate max;
                        max.value = DataPredicate::next_of(numeric_atom.value_upper_bound);
                        max.value_upper_bound = l_max;
                        max.var = numeric_atom.var;
                        max.label = numeric_atom.label;
                        if (isFormulaSet) {
                            formula = ltlf::Or(ltlf::Interval(max), formula);
                        } else {
                            isFormulaSet = true;
                            formula = ltlf::Interval(max);
                        }
                    }

                    for (const auto& elem : numeric_atom.exceptions) {
                        DataPredicate element;
                        element.value = elem;
                        element.value_upper_bound = elem;
                        element.var = numeric_atom.var;
                        element.var = numeric_atom.var;
                        element.label = numeric_atom.label;
                        if (isFormulaSet) {
                            formula = ltlf::Or(ltlf::Interval(element), formula);
                        } else {
                            isFormulaSet = true;
                            formula = ltlf::Interval(element);
                        }
                    }

                    return formula.setBeingCompound(is_compound_predicate);
                }
            }
        }
        case NEG_OF:
            if (args.at(0).casusu == NEG_OF)
                return args.at(0).args.at(0).nnf(isGraph);
            else
                return args.at(0).nnf(isGraph) ;
        case OR:
            return And(args.at(0).negate(isGraph), args.at(1).negate(isGraph)).setBeingCompound(is_compound_predicate);
        case AND:
            return Or(args.at(0).negate(isGraph), args.at(1).negate(isGraph)).setBeingCompound(is_compound_predicate);
        case BOX:
            return Diamond(args.at(0).negate(isGraph), isGraph);
        case DIAMOND:
            return Box(args.at(0).negate(isGraph), isGraph);
        case NEXT:
            return Or({LAST}, (args.at(0).negate(isGraph)));
        case UNTIL:
            return Release(args.at(0).negate(isGraph), args.at(1).negate(isGraph)).setBeingCompound(is_compound_predicate);
        case RELEASE:
            return Until(args.at(0).negate(isGraph), args.at(1).negate(isGraph)).setBeingCompound(is_compound_predicate);
        case TRUE:
            return {FALSE};
        case FALSE:
            return True();
        case LAST:
            return Next(True());
        default:
            throw std::runtime_error("Unexpected case");
    }
}

struct ltlf ltlf::nnf(bool isGraph) const {
    switch (casusu) {
        case NEG_OF:
            if (args.at(0).casusu == NEG_OF)
                return args.at(0).args.at(0).nnf(isGraph);
            else
                return args.at(0).negate(isGraph);
        case OR:
            return Or(args.at(0).nnf(isGraph),
                      args.at(1).nnf(isGraph)).setBeingCompound(is_compound_predicate).setExclusiveness(is_exclusive);
        case AND:
            return And(args.at(0).nnf(isGraph),
                       args.at(1).nnf(isGraph)).setBeingCompound(is_compound_predicate);
        case NEXT:
            return Next(args.at(0).nnf(isGraph)).setBeingCompound(is_compound_predicate);
        case DIAMOND:
            return Diamond(args.at(0).nnf(isGraph), isGraph);
        case BOX:
            return Box(args.at(0).nnf(isGraph), isGraph);
        case UNTIL:
            return Until(args.at(0).nnf(isGraph), args.at(1).nnf(isGraph)).setBeingCompound(is_compound_predicate);
        case RELEASE:
            return Release(args.at(0).nnf(isGraph), args.at(1).nnf(isGraph)).setBeingCompound(is_compound_predicate);
        default:
            return {*this};
    }
}


struct ltlf ltlf::Interval(const std::string &var, numeric_atom_cases case_, const std::string& value) {
    ltlf formula;
    formula.numeric_atom = {var, case_, value};
    formula.casusu = NUMERIC_ATOM;
    return formula;
}

struct ltlf ltlf::Interval(const std::string &var, numeric_atom_cases case_, double value) {
    ltlf formula;
    formula.numeric_atom = {var, case_, value};
    formula.casusu = NUMERIC_ATOM;
    return formula;
}

struct ltlf ltlf::Interval(const DataPredicate &value) {
    ltlf formula;
    formula.numeric_atom = value;
    formula.casusu = NUMERIC_ATOM;
    return formula;
}

ltlf map_disj(const std::unordered_set<std::string> &atoms) {
    std::vector<std::string> A{atoms.begin(), atoms.end()};
    std::sort(A.begin(), A.end());
    DEBUG_ASSERT(!atoms.empty());
    bool isFirst = true;
    ltlf result;
    for (const auto& ref : A) {
        if (isFirst) {
            isFirst = false;
            result = ltlf::Act(ref);
        } else {
            result = ltlf::Or(ltlf::Act(ref), result);
        }
    }
    return result;
}

std::ostream & human_readable_ltlf_printing(std::ostream &os, const ltlf& syntax) {
    std::string reset = "";
    if (syntax.is_negated)
        os << "¬";
    switch (syntax.casusu) {
        case ACT:
            return os << syntax.act << reset;
        case NUMERIC_ATOM:
            return os << syntax.numeric_atom<< reset;
        case NEG_OF:
            os << "(¬(";
            return human_readable_ltlf_printing(os, syntax.args[0]) << "))" << reset;
        case OR:
            os << "(";
            human_readable_ltlf_printing(os, syntax.args[0]) << (syntax.is_exclusive ? ") ⊻ (" : ") ∨ (");
            return human_readable_ltlf_printing(os, syntax.args[1]) << ')' << reset;
        case AND:
            os << "(";
            human_readable_ltlf_printing(os, syntax.args[0]) << ") ∧ (";
            return human_readable_ltlf_printing(os, syntax.args[1]) << ')' << reset;
        case NEXT:
            os << "○(";
            return human_readable_ltlf_printing(os, syntax.args[0]) << ")" << reset;
        case UNTIL:
            os << "(";
            human_readable_ltlf_printing(os, syntax.args[0]) << ") U (";
            return human_readable_ltlf_printing(os, syntax.args[1]) << ')' << reset;
        case RELEASE:
            os << "(";
            human_readable_ltlf_printing(os, syntax.args[0]) << ") R (";
            return human_readable_ltlf_printing(os, syntax.args[1]) << ')' << reset;
        case TRUE:
            return os << "true"<< reset;
        case BOX:
            os << "▢(";
            return human_readable_ltlf_printing(os, syntax.args[0]) << ")" << reset;
        case DIAMOND:
            os << "◇(";
            return human_readable_ltlf_printing(os, syntax.args[0]) << ")" << reset;
        case LAST:
            return os << "LAST" << reset;
        default:
            return os << "false"<< reset;
    }
}

/*
#include <cassert>

aalta::aalta_formula* to_aaltaf(const ltlf& formula) {
    ltl_formula* formula1 = to_aaltaf_rec(formula) ;
    std::cout << to_string(formula1) << std::endl;
    aalta::aalta_formula* af = aalta::aalta_formula::TAIL ();
    af = aalta::aalta_formula(formula1, false, true).unique();
    return af;
}

ltl_formula* to_aaltaf_rec(const ltlf& syntax) {
    std::string reset = "";
    bool isNegated = syntax.is_negated;
    ltl_formula* tmp;
    switch (syntax.casusu) {
        case ACT:
            tmp = create_var(syntax.act.c_str()); break;
        case NUMERIC_ATOM:
            DEBUG_ASSERT(false);
        case NEG_OF:
            tmp = AALTAF_NOT(to_aaltaf_rec(syntax.args[0])); break;
        case OR:
            tmp = AALTAF_OR(to_aaltaf_rec(syntax.args[0]), to_aaltaf_rec(syntax.args[1]) ); break;
        case AND:
            tmp = AALTAF_AND(to_aaltaf_rec(syntax.args[0]), to_aaltaf_rec(syntax.args[1]) ); break;
        case NEXT:
            tmp = AALTAF_NEXT(to_aaltaf_rec(syntax.args[0])); break;
        case UNTIL:
            tmp = AALTAF_UNTIL(to_aaltaf_rec(syntax.args[0]), to_aaltaf_rec(syntax.args[1]) ); break;
        case RELEASE:
            tmp = AALTAF_RELEASE(to_aaltaf_rec(syntax.args[0]), to_aaltaf_rec(syntax.args[1]) ); break;
        case TRUE:
            tmp = AALTAF_TRUE(); break;
        case BOX:
            tmp = AALTAF_GLOBALLY(to_aaltaf_rec(syntax.args[0])); break;
        case DIAMOND:
            tmp = AALTAF_FUTURE(to_aaltaf_rec(syntax.args[0])); break;
        default:
            tmp = AALTAF_FALSE(); break;
    }
    if (syntax.is_negated)
        return nnf(AALTAF_NOT(tmp));
    else
        return nnf(tmp);
}*/
