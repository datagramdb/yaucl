//
// Created by giacomo on 16/02/2022.
//

#include "knobab/server/algorithms/querymanager/LTLfQueryManager.h"
#include "knobab/server/algorithms/querymanager/MAXSatPipeline.h"
#include <stack>

static inline void topological_sort(const std::vector<LTLfQuery*>& W,
                                    std::vector<LTLfQuery*>& vertexOreder) {
    std::unordered_set<LTLfQuery*> toVisit{W.begin(), W.end()};
    std::stack<std::pair<bool, LTLfQuery*>> internalStack;
    std::unordered_set<LTLfQuery*> visited;
    //auto isEnd = adjMap.end();
    while (!toVisit.empty()) {
        auto first = toVisit.begin();
        if (first != toVisit.cend()) {
            internalStack.push(std::make_pair(false, *first));
        }
        while (!internalStack.empty()) {
            std::pair<bool, LTLfQuery*>  cp = internalStack.top();
            internalStack.pop();
            toVisit.erase(cp.second);
            if (cp.first) {
                vertexOreder.emplace_back(cp.second);
            } else {
                if (visited.insert(cp.second).second) {
                    internalStack.push((cp));
                    internalStack.push(std::make_pair(true, cp.second));
                    //auto out = adjMap.find(cp.second);
                    for (auto & i : cp.second->args) {
                        internalStack.push(std::make_pair(false, i));
                    }
                }
            }
        }
    }
    std::reverse(vertexOreder.begin(), vertexOreder.end());
}

#include <magic_enum.hpp>
#include <yaucl/functional/assert.h>

void LTLfQueryManager::generateGraph(std::map<LTLfQuery*, std::vector<LTLfQuery*>>& ref, LTLfQuery*q) const {
    auto it = ref.emplace(q, q->args);
    if (it.second) {
        for (const auto& arg : q->args)
            generateGraph(ref, arg);
    }
}

#include <nlohmann/json.hpp>
std::string LTLfQueryManager::generateGraph() const {
    std::map<LTLfQuery*, std::vector<LTLfQuery*>> ref;
    std::map<LTLfQuery*,size_t> layerId;
    if (Q.empty()) return "{nodes: [], edges: []}";
    auto it = Q.begin();
    ref[nullptr] = it->second;
    layerId[nullptr] = 0;
    for (; it != Q.end(); it++) {
        for (const auto& arg : it->second) {
            layerId[arg] = 1+it->first;
            generateGraph(ref, arg);
        }
    }
    nlohmann::json json;
    json["nodes"] = {};
    json["edges"] = {};
    auto& nodes = json["nodes"];
    auto& edges = json["edges"];

    for (const auto& cp : ref) {
        nlohmann::json node;
        node["id"] = cp.first ?(size_t)cp.first : 0;
        node["group"] = layerId[cp.first];
        if ((cp.first) && (!cp.first->atom.empty()) && (cp.first->args.empty())) {
            std::stringstream aa;
            aa << *cp.first;
            node["label"] = aa.str();
        } else {
            node["label"] = cp.first ? ((cp.first->isTop ? "^" : "") + std::string(cp.first->fields.id.parts.is_timed ? "t" : "") + std::string(magic_enum::enum_name(cp.first->t))) : "Ensemble";
        }
        nodes.push_back(node);
        for (const auto& out : cp.second) {
            nlohmann::json edge;
            edge["from"] = cp.first ? (size_t)cp.first : 0;
            edge["to"] = (size_t)out;
            edges.push_back(edge);
        }
    }
    return json.dump(4);
}

void LTLfQueryManager::clear() {
    current_query_id = 0;
    for (auto it = conversion_map_for_subexpressions.begin(); it != conversion_map_for_subexpressions.end(); it++) {
        delete it->second;
        it = conversion_map_for_subexpressions.erase(it);
    }
//    atomsToDecomposeInUnion.clear();
    counter.clear();
}

void LTLfQueryManager::sort_query_plan_for_scheduling(const AtomizingPipeline& ap, std::vector<LTLfQuery*>& W, KnowledgeBase* ptr) {
    // Making ready for the parallelization of the query execution by setting it into layers
    std::vector<LTLfQuery*> topological_order;
    topological_sort(W, topological_order);
    for (const auto& subFormula: topological_order) {
        for (const auto& adj : subFormula->args) {
            adj->dis = std::max(adj->dis, subFormula->dis +1);
            adj->parentMax = std::max(adj->parentMax, subFormula->dis);
            adj->parentMin = std::min(adj->parentMin, subFormula->dis);
        }
    }

    for (const auto& subFormula: topological_order) {
        Q[subFormula->dis].emplace_back(subFormula);
    }
}

#include <iostream>

LTLfQuery *LTLfQueryManager::simplify(const std::unordered_set<std::string>& atom,
                                      size_t formulaId,
                                      const LTLfQuery &input,
                                      const DeclareDataAware *joinCondition,
                                      const std::unordered_set<std::string> &atom_universe,
                                      const std::unordered_set<std::string> &left,
                                      const std::unordered_set<std::string> &right,
                                      std::vector<std::string> &toUseAtoms,
                                      std::unordered_map<std::string , std::vector<size_t>>& atomToFormulaId) {
    LTLfQuery q;
    q.t = input.t;
    q.n = input.n;
    q.isLeaf = input.isLeaf;
    q.fields = input.fields;
    q.fields.id.parts.is_negated = false; // after resolution, nothing is negated!
    q.isLeaf = input.isLeaf;

    if ((input.declare_arg == DECLARE_TYPE_LEFT)) {
        if (input.fields.id.parts.is_negated) {
            for (const auto& x : atom_universe) {
                if (!left.contains(x)) {
                    q.atom.insert(x);
                    atomToFormulaId[x].emplace_back(formulaId);
                }
            }
        } else {
            for (const auto& x : left) {
                q.atom.insert(x);
                atomToFormulaId[x].emplace_back(formulaId);
            }
        }
    } else if (input.declare_arg == DECLARE_TYPE_RIGHT) {
        if (input.fields.id.parts.is_negated) {
            for (const auto& x : atom_universe) {
                if (!right.contains(x)) {
                    q.atom.insert(x);
                    atomToFormulaId[x].emplace_back(formulaId);
                }
            }
        } else {
            for (const auto& x : right) {
                q.atom.insert(x);
                atomToFormulaId[x].emplace_back(formulaId);
            }
        }
    } else {
        //To be done at a future step: supporting three argument clauses
        if(q.fields.id.parts.is_atom) {
            if(!((q.t == LTLfQuery::FIRST_QP) || (q.t == LTLfQuery::LAST_QP))) {
                if (input.fields.id.parts.is_negated) {
                    for (const auto& x : atom_universe) {
                        if (!left.contains(x)) {
                            q.atom.insert(x);
                            atomToFormulaId[x].emplace_back(formulaId);
                        }
                    }
                } else {
                    for (const auto& x : left) {
                        q.atom.insert(x);
                        atomToFormulaId[x].emplace_back(formulaId);
                    }
                }
            }
        }
    }
    if (input.fields.id.parts.has_theta) {
        q.joinCondition = joinCondition;
    }
    for (auto& args : input.args_from_script)
        q.args.emplace_back(simplify(atom, formulaId, args, joinCondition, atom_universe, left, right, toUseAtoms, atomToFormulaId));
    q.fields.id.parts.is_queryplan = true;
    q.fields.id.parts.is_negated = false;
    q.declare_arg = DECLARE_TYPE_NONE;
    auto tmp = simplify(q);
    if (q.isLeaf == ActivationLeaf) {
        if (current_query_id == activations.size()) { // Query Id counting from zero, so, if that happens, then it means that I need to add the activation in here!
            activations.emplace_back().emplace(tmp);
        } else {
            activations.back().emplace(tmp);
            //DEBUG_ASSERT( (activations.back() == tmp)); // By default, the activations should always refer to the same atom! That will make the assumption in the pipeline correct
        }
    }
    return tmp;
}

LTLfQuery *LTLfQueryManager::simplify(const LTLfQuery &q) {
    auto it = conversion_map_for_subexpressions.find(q);
    if (it != conversion_map_for_subexpressions.end()) {
        counter[it->second]++;
        return it->second;
    } else {
        auto* ptr = new LTLfQuery();
        *ptr = q;
        /// Checking whether this is a leaf node
        if (ptr->args.empty()) {
            switch (ptr->t) {
                case LTLfQuery::INIT_QP:{
                    DEBUG_ASSERT(!ptr->fields.id.parts.is_timed);
                    DEBUG_ASSERT(!ptr->atom.empty());
                    for (const auto& it2 : ptr->atom) {
                        if (atomization->data_query_atoms.contains(it2)) {
                            // For the moment, we just register the leaf as associated to the atom
                            // subqueriesRunning, part 2, will then associated the formula with the
                            // intermediate result associated to each atom
                            pipeline->pushDataRangeQuery(ptr, *atomization, it2);
                        } else {
                            // Directly stores the atom into table_query
                            ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::InitQuery(it2)));
                        }
                    }
                } break;

                case LTLfQuery::END_QP: {
                    DEBUG_ASSERT(!ptr->fields.id.parts.is_timed);
                    DEBUG_ASSERT(!ptr->atom.empty());
                    for (const auto& it2 : ptr->atom) {
                        if (atomization->data_query_atoms.contains(it2)) {
                            // For the moment, we just register the leaf as associated to the atom
                            // subqueriesRunning, part 2, will then associated the formula with the
                            // intermediate result associated to each atom
                            pipeline->pushDataRangeQuery(ptr, *atomization, it2);
                        } else {
                            // Directly stores the atom into table_query
                            ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::EndsQuery(it2)));
                        }
                    }
                } break;

                case LTLfQuery::FIRST_QP: {
                    DEBUG_ASSERT(ptr->fields.id.parts.is_timed);
                    DEBUG_ASSERT(ptr->atom.empty());
                    // Directly stores the atom into table_query
                    ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::FirstQuery(ptr->isLeaf)));
                } break;

                case LTLfQuery::LAST_QP: {
                    DEBUG_ASSERT(ptr->fields.id.parts.is_timed);
                    DEBUG_ASSERT(ptr->atom.empty());
                    // Directly stores the atom into table_query
                    ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::LastQuery(ptr->isLeaf)));
                } break;

                case LTLfQuery::EXISTS_QP: {
                    if (ptr->fields.id.parts.is_timed) {
                        if (ptr->n != 1) {
                            throw std::runtime_error("In the current implementation, we only support timed exists with n = 1!");
                        }
                    }
                    auto it2 = ptr->atom.begin();
                    if ((!ptr->fields.id.parts.is_timed) && (ptr->atom.size() == 1) && (!atomization->data_query_atoms.contains(*it2))) {
                        // If we have only one atom, has size of one and it is not a data query, then I can exploit the tables
                        ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::ExistsQuery(*it2, ptr->n, ptr->isLeaf)));
                    } else {
                        // This is computed independently from the element being timed or untimed
                        for (const auto& it3 : ptr->atom) {
                            if (atomization->data_query_atoms.contains(it3)) {
                                // For the moment, we just register the leaf as associated to the atom
                                // subqueriesRunning, part 2, will then associated the formula with the
                                // intermediate result associated to each atom
                                pipeline->pushDataRangeQuery(ptr, *atomization, it3);
                            } else {
                                ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::AtomQuery(it3)));
                            }
                        }
                    }
                } break;

                case LTLfQuery::FALSEHOOD_QP:
                    // Falsehood just returns empty: nothing shall be done in here.
                    break;

                case LTLfQuery::ABSENCE_QP: {
//                    if (ptr->fields.id.parts.is_negated) {
//                        throw std::runtime_error("Why do you want to use a negated absence? Please use a normal existene instead!");
//                    }
                    if (ptr->fields.id.parts.is_timed) {
                        if (ptr->n != 1) {
                            throw std::runtime_error("In the current implementation, we only support timed exists with n = 1!");
                        }
                    }
                    auto it3 = ptr->atom.begin();
                    if ((!ptr->fields.id.parts.is_timed) && (ptr->atom.size() == 1) && (!atomization->data_query_atoms.contains(*it3))) {
                        // If we have only one atom, has size of one and it is not a data query, then I can exploit the tables
                        ptr->table_query.emplace_back(pipeline->pushNonRangeQuery(DataQuery::AbsenceQuery(*it3, ptr->n,ptr->isLeaf)));
                    } else {
                        throw std::runtime_error("ERROR: we are not expecting timed absence queries or with multiple atoms anymore after rewriting! Absence should be now a NOT operator");
                    }
                } break;

                // These other cases should not be covered, as they are non-leaf nodes
                case LTLfQuery::NEXT_QP:
                case LTLfQuery::OR_QP:
                case LTLfQuery::AND_QP:
                case LTLfQuery::IMPL_QP:
                case LTLfQuery::IFTE_QP:
                case LTLfQuery::U_QP:
                case LTLfQuery::G_QP:
                case LTLfQuery::F_QP:
                case LTLfQuery::NOT_QP:
                case LTLfQuery::AF_QPT:
                case LTLfQuery::AXG_QPT:
                case LTLfQuery::AG_QPT:
                    DEBUG_ASSERT(false);
                    break;
            }
//            std::cout << *ptr << std::endl;
        }
        counter.emplace(ptr, 1);
        assert(conversion_map_for_subexpressions.emplace(q, ptr).second);
        return ptr;
    }
}


nlohmann::json LTLfQueryManager::generateJSONGraph() const {
    std::map<LTLfQuery*, std::vector<LTLfQuery*>> ref;
    std::map<LTLfQuery*,size_t> layerId;
    nlohmann::json json;
    json["nodes"] = {};
    json["edges"] = {};
    if (Q.empty()) return json;
    auto it = Q.begin();
    ref[nullptr] = it->second;
    layerId[nullptr] = 0;
    for (; it != Q.end(); it++) {
        for (const auto& arg : it->second) {
            layerId[arg] = 1+it->first;
            generateGraph(ref, arg);
        }
    }
    auto& nodes = json["nodes"];
    auto& edges = json["edges"];
    for (const auto& cp : ref) {
        nlohmann::json node;
        node["id"] = cp.first ?(size_t)cp.first : 0;
        node["group"] = layerId[cp.first];
        if ((cp.first) && (!cp.first->atom.empty())) {
            std::stringstream aa;
            aa << *cp.first;
            node["label"] = aa.str();
        } else {
            node["label"] = cp.first ? ((cp.first->fields.id.parts.is_timed ? "t" : "") + std::string(magic_enum::enum_name(cp.first->t))) : "Ensemble";
        }
        nodes.push_back(node);
        for (const auto& out : cp.second) {
            nlohmann::json edge;
            edge["from"] = cp.first ? (size_t)cp.first : 0;
            edge["to"] = (size_t)out;
            edges.push_back(edge);
        }
    }
    return json;
}

for_occurrence::for_occurrence(bool isTimed, LeafType type, size_t nArg) : isTimed(isTimed), type(type), n_arg(nArg) {}

bool for_occurrence::operator==(const for_occurrence &rhs) const {
    return isTimed == rhs.isTimed &&
            isDisjunctiveSoNegated == rhs.isDisjunctiveSoNegated &&
           type == rhs.type &&
           n_arg == rhs.n_arg;
}

bool for_occurrence::operator!=(const for_occurrence &rhs) const {
    return !(rhs == *this);
}
