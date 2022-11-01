//
// Created by giacomo on 08/01/2022.
//

#ifndef KNOBAB_MINIMIZEDFA_H
#define KNOBAB_MINIMIZEDFA_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>

#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/vector_hash.h>
#include <yaucl/hashing/uset_hash.h>

#include <yaucl/graphs/FlexibleFA.h>
#include <iostream>
#include <yaucl/structures/eq_class.h>

void marcaRec(std::pair<size_t,size_t> cp, std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t,size_t>>, bool>>& M);

template<typename NodeElement, typename EdgeLabel>
FlexibleFA<std::vector<NodeElement>, EdgeLabel> minimizeDFA(const FlexibleFA<NodeElement, EdgeLabel>& graph) {
    std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> M;

    std::vector<size_t> v = graph.getNodeIds();
    std::vector<size_t> notFinals;
    std::unordered_set<EdgeLabel> sigma;
    std::unordered_map<size_t, std::unordered_map<EdgeLabel, size_t>> localDelta;
    for (size_t qp : v) {
        if (!graph.final_nodes.contains(qp)) {
            notFinals.emplace_back(qp);
        }
        for (const std::pair<EdgeLabel, size_t> &edge : graph.outgoingEdges(qp)) {
            sigma.insert(edge.first);
            localDelta[qp][edge.first] = edge.second;
        }
    }

    std::sort(v.begin(), v.end());
    {
        std::pair<size_t, size_t> cp;
        {
            for (size_t i = 0, N = v.size(); i < N; i++) {
                size_t pp = v.at(i);
                cp.first = pp;
                for (size_t j = i + 1; j < N; j++) {
                    size_t qp = v.at(j);
                    cp.second = qp;
                    if (qp < pp) {
                        std::swap(cp.first, cp.second);
                    }
                    //std::cout << '{' << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<'}' << std::endl;
                    if ((graph.final_nodes.contains(pp) && (graph.final_nodes.contains(qp))) ||
                        (!graph.final_nodes.contains(qp) && (!graph.final_nodes.contains(pp)))) {
                        M[cp] = {std::unordered_set<std::pair<size_t, size_t>>{}};
                    } else {
                        M[cp] = {false};
                    }
                }
            }
        }

        for (auto &cp2 : M) {
            if (!std::holds_alternative<bool>(cp2.second)) {
                //std::cout << '<' << graph.getNodeLabel(cp2.first.first) <<',' << graph.getNodeLabel(cp2.first.second) <<'>' << std::endl;
                bool Xfound = false;
                std::unordered_set<std::pair<size_t, size_t>> L;
                for (const auto &a : sigma) {
                    std::unordered_set<size_t> S;
                    S.insert(cp2.first.first);
                    S.insert(cp2.first.second);
                    auto S2 = graph.Move2(S, a);
                    auto it = S2.begin();
                    if (S2.size() > 1) {
                        cp.first = *it;
                        cp.second = *(++it);
                        if (cp.first > cp.second)
                            std::swap(cp.first, cp.second);
                        if (std::holds_alternative<bool>(M[cp])) {
                            //std::cout << "\t--> <" << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<"> XXX" << std::endl;
                            marcaRec(/*graph,*/ cp2.first, M);
                            Xfound = true;
                            break;
                        } else {
                            if (cp.first != cp.second) {
                                //std::cout << "\t--> <" << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<">  <<=" << std::endl;
                                L.insert(cp);
                            }
                        }
                    }

                }
                if (!Xfound) {
                    for (const std::pair<size_t, size_t> &cp3 : L) {
                        auto it = M.find(cp3);
                        DEBUG_ASSERT(!std::holds_alternative<bool>(it->second));
                        std::get<std::unordered_set<std::pair<size_t, size_t>>>(it->second).insert(cp2.first);
                    }
                }
            }
        }
    }

    // TODO: this is not going to work if the graph only accepts the empty string!
    size_t vCount = 0;
    std::unordered_set<size_t> equivalentNodes, VS;
    VS.insert(v.begin(), v.end());
    FlexibleFA<std::vector<NodeElement>, EdgeLabel> result;
    std::unordered_map<std::unordered_set<size_t>, size_t> nodeToId;
    std::unordered_map<size_t, std::unordered_set<size_t>> full_eq_class;
    equivalence_class<size_t> cl;
    {
        std::unordered_set<size_t> preserved;
        for (const auto& cp : M) {
            if (!std::holds_alternative<bool>(cp.second)) {
                cl.insert(cp.first.first, cp.first.second);
            } else {
                cl.insert(cp.first.first, cp.first.first);
                cl.insert(cp.first.second, cp.first.second);
            }
        }
        for (const auto& cp : cl.calculateEquivalenceClass()) {
            std::cout <<  graph.getNodeLabel(cp.first) << std::endl;
            for (const auto& x : cp.second) {
                //std::cout << "\t -" <<  graph.getNodeLabel(x) << "[internal id, not displayed label= "<< x << "]" << std::endl;
            }
            equivalentNodes.insert(cp.second.begin(), cp.second.end());
            auto it = nodeToId.emplace(cp.second, vCount);
            if (it.second) {
                vCount++;
                std::vector<NodeElement> nl;
                for (const auto& x : cp.second) {
                    nl.emplace_back(graph.getNodeLabel(x));
                }
                it.second = result.addNewNodeWithLabel(nl);
            }
            full_eq_class[cp.first] = cp.second;
            for (const auto& elements : cp.second) {
                full_eq_class[elements] = cp.second;
            }
        }
    }
    for (size_t fin : graph.fini()) {
        result.addToFinalNodesFromId(nodeToId.at(full_eq_class.at(fin)));
    }
    for (size_t ini : graph.init()) {
        result.addToInitialNodesFromId(nodeToId.at(full_eq_class.at(ini)));
    }
    for (const auto& node : nodeToId) {
        /*std::cout << "{" ;
        for (size_t id : node.first)
            std::cout << id << ",";
        std::cout << "}"  << std::endl;*/

#if 1
        for (const std::pair<EdgeLabel, std::unordered_set<size_t>>& eOut :
                graph.Move3(node.first)) {
#if 1
            std::unordered_set<size_t> S;
            for (const auto& elem : eOut.second) {
                auto f = full_eq_class.at(elem);
                S.insert(f.begin(), f.end());
            }
            auto it = nodeToId.find(S);
            DEBUG_ASSERT(it != nodeToId.end());
#else
            auto it = nodeToId.find(eOut.second);
            // TODO: DEBUG_ASSERT(it != nodeToId.end());
            if (it != nodeToId.end())
#endif
            /*std::cout << "{" ;
            for (size_t id : node.first)
                std::cout << graph.getNodeLabel(id) << ",";
            std::cout << "}"  ;
            std::cout <<  "--[" << eOut.first << "]-->";
            std::cout << "{" ;
            for (size_t id : S)
                std::cout << graph.getNodeLabel(id) << ",";
            std::cout << "}"  << std::endl;*/
            result.addNewEdgeFromId(node.second, it->second, eOut.first);
        }
#endif
    }

    //result.pruneUnreachableNodes();
    return result;
}

#endif //KNOBAB_MINIMIZEDFA_H
