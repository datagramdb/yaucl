/*
 * NodeLabelBijectionFA.h
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

#include <yaucl/graphs/NodeLabelBijectionGraph.h>
#include <unordered_set>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/structures/set_operations.h>
#include <ostream>

#ifndef GRAPHOS_NLBFA_H
#define GRAPHOS_NLBFA_H

/**
 * General definition of the automaton
 *
 * @tparam NodeElement
 * @tparam EdgeLabel
 */
template <typename NodeElement, typename EdgeLabel>
class NodeLabelBijectionFA : public NodeLabelBijectionGraph<NodeElement, EdgeLabel> {

    std::unordered_set<size_t> initial_nodes;
    std::unordered_set<size_t> final_nodes;
public:

    double size() const {
        return NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumNodeId() + NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumEdgeId();
    }

    const std::unordered_set<size_t>& init() const {
        return initial_nodes;
    }

    const std::unordered_set<size_t>& fini() const {
        return final_nodes;
    }

    bool isFinalNodeByID(size_t id) const {
        return final_nodes.contains(id);
    }

    bool isInitialNodeByID(size_t id) const {
        return initial_nodes.contains(id);
    }

    void addToInitialNodes(const NodeElement& node) {
        initial_nodes.insert(getId(node));
    }

    void addToInitialNodesFromId(size_t node) {
        initial_nodes.insert((node));
    }

    void addToFinalNodes(const NodeElement& node) {
        final_nodes.insert(NodeLabelBijectionGraph<NodeElement, EdgeLabel>::getId(node));
    }

    void addToFinalNodesFromId(size_t node) {
        final_nodes.insert(node);
    }

    std::vector<EdgeLabel, size_t> delta(const NodeElement& node) {
        return outgoingEdges(node);
    }

    std::unordered_map<EdgeLabel, std::unordered_set<size_t>> Move(const std::unordered_set<NodeElement>& P) {
        std::unordered_map<EdgeLabel, std::unordered_set<int>> reachable;
        for (const NodeElement& p : P) {
            for (const std::pair<EdgeLabel, int>& cp: outgoingEdges(p)) {
                reachable[cp.first].insert(cp.second);
            }
        }
        return reachable;
    }

    std::unordered_set<size_t> Move(const std::unordered_set<NodeElement>& P, const EdgeLabel& given) {
        std::unordered_set<size_t> reachable;
        for (const NodeElement& p : P) {
            for (const std::pair<EdgeLabel, size_t>& cp: outgoingEdges(p)) {
                if (cp.second == given)
                    reachable.insert(cp.first);
            }
        }
        return reachable;
    }

    std::unordered_set<size_t> Closure(const std::unordered_set<NodeElement>& P, const EdgeLabel& epsilon) {
        std::unordered_set<size_t> t, result;
        for (const NodeElement& p : P) {
            t.insert(getId(p));
            result.insert(getId(p));
        }
        while (!t.empty()) {
            size_t elem = *t.begin();
            t.erase(t.begin());
            const std::unordered_set<size_t> elemSet{elem};
            for (const int s : Move(elemSet, epsilon)) {
                if (result.insert(s).second) {
                    t.insert(s);
                }
            }
        }
        return result;
    }

    template<class T, class U> class FlexibleFA;

    FlexibleFA<EdgeLabel, NodeElement> shiftLabelsToNodes() {
        FlexibleFA<EdgeLabel, NodeElement> result;
        std::unordered_map<size_t , size_t> edgeToNewNodeMap;
        std::unordered_map<size_t, std::vector<size_t>> outgoingEdges;

        for (size_t arcId = 0, N = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumEdgeId(); arcId<N; arcId++) {
            std::pair<size_t,size_t> arc = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::g.edge_from_id(arcId);
            int arcSrc = arc.first;
            outgoingEdges[arcSrc].emplace_back(arcId);
            int arcIdAsNode = result.addNewNodeWithLabel(NodeLabelBijectionGraph<NodeElement, EdgeLabel>::getEdgeLabel(arcId));
            edgeToNewNodeMap.emplace(arcId, arcIdAsNode);

            if (init().contains(arcSrc))
                result.addToInitialNodesFromId(arcIdAsNode);
            if (fini().contains(arc.second))
                result.addToFinalNodesFromId(arcIdAsNode);
        }
        for (size_t arcId = 0, N = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumEdgeId(); arcId<N; arcId++) {
            std::pair<size_t,size_t> arc = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::g.edge_from_id(arcId);
            int src = edgeToNewNodeMap.at(arcId);
            int origArcDst = arc.second;
            auto it = outgoingEdges.find(origArcDst);
            if (it != outgoingEdges.end()) {
                auto dstLabel = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::getUniqueLabel(origArcDst);
                for (int arc2 : it->second) {
                    result.addNewEdgeFromId(src, edgeToNewNodeMap.at(arc2), dstLabel);
                }
            }
        }
        return result;
    }

    friend std::ostream &operator<<(std::ostream &os, const NodeLabelBijectionFA &fa) {
        for (size_t node = 0, N =  fa.maximumNodeId(); node<N; node++) {
            os << node << "=" << fa.getUniqueLabel(node) << std::endl;
        }
        for (const auto& it : fa.initial_nodes)
            os << "Initial = " << it << std::endl;
        for (const auto& it : fa.final_nodes)
            os << "Final = " << it << std::endl;
        for (size_t node = 0, N =  fa.maximumNodeId(); node<N; node++) {
            for (const auto& edge : fa.outgoingEdges(node)) {
                os << node << " ~~{" << edge.first << "}~~> " << edge.second  << std::endl;
            }
        }
        return os;
    }

    void dot(std::ostream& os, bool ignoreEdgeLabels = false) {
        os << "digraph finite_state_machine {\n"
              "    rankdir=LR;\n"
              "    size=\"8,5\"\n";
        for (size_t node_id = 0, N = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumNodeId(); node_id<N; node_id++) {
            ///TODO: if (removed_nodes.contains(node_id)) continue;
            std::string shape = "circle";
            if (final_nodes.contains(node_id)) {
                shape = "doublecircle";
            }
            os << "node [shape = " << shape << ", label=\"" << NodeLabelBijectionGraph<NodeElement, EdgeLabel>::getUniqueLabel(node_id) << "\", fontsize=10] q" << node_id << ";\n";
            if (initial_nodes.contains(node_id)) {
                os << "node [shape = point] s" << node_id << ";\n";
            }
        }
        os << "\n\n";
        for (size_t node_id = 0, N = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumNodeId(); node_id<N; node_id++) {
            ///TODO: if (removed_nodes.contains(node_id)) continue;
            std::string shape = "circle";
            if (initial_nodes.contains(node_id)) {
                os << "s" << node_id << " -> q" << node_id << ";\n";
            }
            for (const std::pair<EdgeLabel, int>& edge : NodeLabelBijectionGraph<NodeElement, EdgeLabel>::outgoingEdges(node_id)) {
                os << "q" << node_id << " -> q" << edge.second;
                if (!ignoreEdgeLabels)
                    os << " [ label = \"" << edge.first << "\"]";
                os << ";\n";
            }
        }
        os << "}";
    }


    NodeLabelBijectionFA<NodeElement, EdgeLabel>& makeDFAAsInTheory(const NodeElement& distinctId) {
        std::unordered_set<EdgeLabel> acts = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::getAllActionSet();
        bool insertBottom = false;
        size_t botNode = -1;
        size_t actsSize = acts.size();
        for (size_t nodeId = 0, N = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::maximumNodeId(); nodeId<N; nodeId++) {
            std::vector<std::pair<EdgeLabel, size_t>> E = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::outgoingEdges(nodeId);
            if (E.size() == actsSize) continue;
            else {
                if (!insertBottom) {
                    botNode = NodeLabelBijectionGraph<NodeElement, EdgeLabel>::addUniqueStateOrGetExisting(distinctId);
                    for (const EdgeLabel& label : acts) {
                        NodeLabelBijectionGraph<NodeElement, EdgeLabel>::addNewEdgeFromId(botNode, botNode, label);
                    }
                    insertBottom = true;
                }
                std::unordered_set<EdgeLabel> curr;
                for (const auto& cp : E)
                    curr.insert(cp.first);
                for (const EdgeLabel& label : unordered_difference(acts, curr)) {
                    NodeLabelBijectionGraph<NodeElement, EdgeLabel>::addNewEdgeFromId(nodeId, botNode, label);
                }
            }
        }
        return *this;
    }


};
#endif //GRAPHOS_FA_H
