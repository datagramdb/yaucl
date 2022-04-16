/*
 * FlexibleFA.h
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
// Created by giacomo on 21/02/21.
//

#ifndef CLASSIFIERS_FLEXIBLEFA_H
#define CLASSIFIERS_FLEXIBLEFA_H

#include <unordered_set>
#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/graphs/FlexibleGraph.h>
#include <yaucl/structures/set_operations.h>
#include <ostream>
#include <variant>
#include <cassert>
#include <yaucl/graphs/NodeLabelBijectionFA.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>

template <typename NodeElement, typename EdgeLabel>
class FlexibleFA : public FlexibleGraph<NodeElement, EdgeLabel> {

public:
    std::unordered_set<size_t> initial_nodes;
    std::unordered_set<size_t> final_nodes;
    std::unordered_set<size_t> removed_nodes;

    ~FlexibleFA() {
        initial_nodes.clear();
        final_nodes.clear();
        removed_nodes.clear();
    }

    const std::unordered_set<size_t>& init() const {
        return initial_nodes;
    }

    const std::unordered_set<size_t>& fini() const {
        return final_nodes;
    }

    std::set<std::vector<EdgeLabel>> generative(size_t limitSize = 3, size_t max_per_limit = 50) {
        std::vector<size_t> Arg(getNodeIds().size(), 0);
        std::queue<std::pair<size_t, std::vector<EdgeLabel>>> Q;
        std::set<std::vector<EdgeLabel>> EL;
        for (size_t i : init())
            Q.emplace(i, std::vector<EdgeLabel>{});
        while (!Q.empty()) {
            std::pair<size_t, std::vector<EdgeLabel>> cp = Q.front();
            Q.pop();
            if ((cp.second.size() <= limitSize)) {
                if (final_nodes.contains(cp.first) && (cp.second.size() > 0) /*&& (Arg[cp.first] < max_per_limit)*/) {
                    //Arg[cp.first]++;
                    EL.emplace(cp.second);
                }
                for (auto cp2 : outgoingEdges(cp.first)) {
                    std::vector<EdgeLabel> el{cp.second};
                    el.emplace_back(cp2.first);
                    Q.emplace(cp2.second, el);
                }
            }
        }
        return EL;
    }

    std::vector<size_t> getNodeIds() const  {
        std::vector<size_t> result;
        for (size_t id = 0, N = FlexibleGraph<NodeElement, EdgeLabel>::maximumNodeId(); id<N; id++) {
            if (!removed_nodes.contains(id))
                result.emplace_back(id);
        }
        return result;
    }



    std::vector<size_t> getEdgeIds() const  {
        std::vector<size_t> result;
        for (size_t id = 0, N = FlexibleGraph<NodeElement, EdgeLabel>::maximumEdgeId(); id<N; id++) {
            auto cp = FlexibleGraph<NodeElement, EdgeLabel>::g.edge_from_id(id);
            if ((!removed_nodes.contains(cp.first)) && (!removed_nodes.contains(cp.second)))
                result.emplace_back(id);
        }
        return result;
    }

    double size() const {
        return FlexibleGraph<NodeElement, EdgeLabel>::maximumNodeId() - removed_nodes.size() +getEdgeIds().size();
    }

    NodeElement getNodeLabel(size_t id) const override {
        DEBUG_ASSERT(!removed_nodes.contains(id));
        return FlexibleGraph<NodeElement, EdgeLabel>::nodeLabel.at(id);
    }

    virtual void addNewEdgeFromId(size_t src2, size_t dst2, const EdgeLabel& weight) override {
        DEBUG_ASSERT(!removed_nodes.contains(src2));
        DEBUG_ASSERT(!removed_nodes.contains(dst2));
        FlexibleGraph<NodeElement, EdgeLabel>::addNewEdgeFromId(src2, dst2, weight);
    }

    virtual std::vector<std::pair<EdgeLabel, size_t>> outgoingEdges(size_t n) const override {
        if (removed_nodes.contains(n)) return {};
        std::vector<std::pair<EdgeLabel, size_t>> result;
        for (const std::pair<EdgeLabel, size_t>& cp : FlexibleGraph<NodeElement, EdgeLabel>::outgoingEdges(n)) {
            if (!removed_nodes.contains(cp.second)) {
                result.emplace_back(cp);
            }
        }
        return result;
    }

    virtual std::vector<size_t> outgoingEdgesWithMove(size_t n, const EdgeLabel& label) const {
        if (removed_nodes.contains(n)) return {};
        std::vector<size_t> result;
        for (const std::pair<EdgeLabel, size_t>& cp : FlexibleGraph<NodeElement, EdgeLabel>::outgoingEdges(n)) {
            if (!removed_nodes.contains(cp.second)) {
                if (cp.first == label)
                    result.emplace_back(cp.second);
            }
        }
        return result;
    }


    virtual std::vector<std::pair<EdgeLabel, size_t>> ingoingEdges(size_t n) const override {
        if (removed_nodes.contains(n)) return {};
        std::vector<std::pair<EdgeLabel, size_t>> result;
        for (const std::pair<EdgeLabel, size_t>& cp : FlexibleGraph<NodeElement, EdgeLabel>::ingoingEdges(n)) {
            if (!removed_nodes.contains(cp.second)) {
                result.emplace_back(cp);
            }
        }
        return result;
    }

    bool isFinalNodeByID(size_t id) const {
        return final_nodes.contains(id);
    }

    bool isInitialNodeByID(size_t id) const {
        return initial_nodes.contains(id);
    }

    void addToInitialNodesFromId(size_t node) {
        initial_nodes.insert((node));
    }

    void addToFinalNodesFromId(size_t node) {
        final_nodes.insert((node));
    }

    std::vector<EdgeLabel, size_t> delta(const NodeElement& node) const {
        return outgoingEdges(node);
    }

    std::unordered_map<EdgeLabel, std::unordered_set<size_t>> Move(const std::unordered_set<NodeElement>& P) const {
        std::unordered_map<EdgeLabel, std::unordered_set<size_t>> reachable;
        for (const NodeElement& p : P) {
            if (removed_nodes.contains(p)) continue;
            for (const std::pair<EdgeLabel, size_t>& cp: outgoingEdges(p)) {
                reachable[cp.first].insert(cp.second);
            }
        }
        return reachable;
    }

    void test_correctness(const std::vector<EdgeLabel>& verifier) const {
        DEBUG_ASSERT(initial_nodes.size() == 1);
        size_t start = *initial_nodes.begin();
        auto it = verifier.begin();
        std::stringstream ss;
        while (it != verifier.end()) {
            std::vector<size_t> v = outgoingEdgesWithMove(start, *it);
            if (!(v.size() == 1)) {
                std::cerr << ss.str() << std::endl;
                std::cerr << "Failure at " << start << " @" << *it << std::endl;
                exit(1);
            }
            ss << getNodeLabel(start) << "-[" << *it << "]->" << getNodeLabel(v.at(0)) << std::endl;
            start = v.at(0);
            it++;
        }
        if (!(final_nodes.contains(start))) {
            std::cerr << ss.str() << std::endl;
            exit(1);
        } else {
            //std::cout << ss.str() << std::endl;
        }
    }

    std::unordered_set<size_t> Move(const std::unordered_set<NodeElement>& P, const EdgeLabel& given) {
        std::unordered_set<size_t> reachable;
        for (const NodeElement& p : P) {
            if (removed_nodes.contains(p)) continue;
            for (const std::pair<EdgeLabel, size_t>& cp: outgoingEdges(p)) {
                if (cp.first == given)
                    reachable.insert(cp.second);
            }
        }
        return reachable;
    }

    std::unordered_set<size_t> Move2(const std::unordered_set<size_t>& P, const EdgeLabel& given) const {
        std::unordered_set<size_t> reachable;
        for (const size_t& p : P) {
            if (removed_nodes.contains(p)) continue;
            for (const std::pair<EdgeLabel, size_t>& cp: outgoingEdges(p)) {
                if (cp.first == given)
                    reachable.insert(cp.second);
            }
        }
        return reachable;
    }

    std::unordered_map<EdgeLabel, std::unordered_set<size_t>> Move3(const std::unordered_set<size_t>& P) const {
        std::unordered_map<EdgeLabel, std::unordered_set<size_t>> reachable;
        for (const size_t& p : P) {
            if (removed_nodes.contains(p)) continue;
            for (const std::pair<EdgeLabel, size_t>& cp: outgoingEdges(p)) {
                reachable[cp.first].insert(cp.second);
            }
        }
        return reachable;
    }

    std::unordered_set<size_t> Closure(const std::unordered_set<size_t>& P, const EdgeLabel& epsilon) {
        std::unordered_set<size_t> t, result;
        t = P;
        result = P;
        while (!t.empty()) {
            size_t elem = *t.begin();
            t.erase(t.begin());
            const std::unordered_set<size_t> elemSet{elem};
            for (const size_t s : Move(elemSet, epsilon)) {
                if (result.insert(s).second) {
                    t.insert(s);
                }
            }
        }
        return result;
    }

    std::unordered_set<size_t> ClosureId(const std::unordered_set<NodeElement>& P, const EdgeLabel& epsilon) {
        std::unordered_set<size_t> t, result;
        for (const NodeElement& p : P) {
            for (const auto& id : FlexibleGraph<NodeElement, EdgeLabel>::getIdsFromLabel(p)) {
                t.insert(id);
                result.insert(id);
            }
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

    friend std::ostream &operator<<(std::ostream &os, const FlexibleFA &fa) {
        for (const auto& node : fa.getNodeIds()) {
            os << node << "=" << fa.getNodeLabel(node) << std::endl;
        }
        for (const auto& it : fa.initial_nodes)
            os << "Initial = " << it << std::endl;
        for (const auto& it : fa.final_nodes)
            os << "Final = " << it << std::endl;
        for (const auto& node : fa.getNodeIds()) {
            for (const auto& edge : fa.outgoingEdges(node)) {
                os << node << " ~~{" << edge.first << "}~~> " << edge.second  << std::endl;
            }
        }
        return os;
    }

    void dot(std::ostream& os, bool ignoreEdgeLabels = false) const {
        os << "digraph {\nrankdir=LR;\n";
        /*"    rankdir=LR;\n"
        "    size=\"8,5\"\n";*/
        for (size_t node_id : initial_nodes) {
            if (removed_nodes.contains(node_id)) continue;
            os << "\tfake" << node_id << " [style=invisible]" << std::endl;
        }
        for (int node_id : getNodeIds()) {
            if (removed_nodes.contains(node_id)) continue;
            os << '\t' << node_id;
            bool hasFinal = final_nodes.contains(node_id);
            bool hasInitial = initial_nodes.contains(node_id);
            os << " [";
            if (hasFinal || hasInitial) {
                if (hasInitial)
                    os << "root=true ";
                if (hasFinal) {
                    os << "shape=doublecircle ";
                }

            }
            os << "label=\"" << getNodeLabel(node_id) <<"\"";
            os << "]" << std::endl;
            /*std::string shape = "circle";
            if (final_nodes.contains(node_id)) {
                shape = "doublecircle";
            }
            os << "node [shape = " << shape << ", label=\"" << getNodeLabel(node_id) << "\", fontsize=10] q" << node_id << ";\n";
            if (initial_nodes.contains(node_id)) {
                os << "node [shape = point] s" << node_id << ";\n";
            }*/
        }
        for (size_t node_id : initial_nodes) {
            if (removed_nodes.contains(node_id)) continue;
            os << "\tfake" << node_id << " -> " << node_id << " [style=bold]" << std::endl;
        }
        for (int node_id : getNodeIds()) {
            if (removed_nodes.contains(node_id)) continue;
            for (const std::pair<EdgeLabel, int>& edge : outgoingEdges(node_id)) {
                os << '\t' << node_id << " -> " << edge.second;
                if (!ignoreEdgeLabels)
                    os << " [label=" << edge.first << "]";
                os << std::endl;
            }
        }
        os << "}";
    }

    void removeStatesNotLeadingToAcceptance() {
        //return;
#if 1
        std::unordered_set<size_t> reached, all;
        auto allNodeIds = getNodeIds();
        all.insert(allNodeIds.begin(), allNodeIds.end());

        // Preserving only the nodes reachable from the initial nodes potentially leading to final states
        for (size_t initial : getNodeIds()) {
            std::unordered_set<size_t> visited_src_dst;
            FlexibleGraph<NodeElement, EdgeLabel>::g.DFSUtil(initial, visited_src_dst);
            if (!unordered_intersection(visited_src_dst, final_nodes).empty())//Preserving the nodes only if I am able to at least reach one final state
                reached.insert(initial);
        }

        std::unordered_set<size_t> candidatesForRemoval = unordered_difference(all, reached);

        // std::cerr << "Removal candidates: #" << candidatesForRemoval.size() << std::endl;
        for (size_t nodeToBeRemoved : candidatesForRemoval) {
            removed_nodes.insert(nodeToBeRemoved);
            initial_nodes.erase(nodeToBeRemoved);
            final_nodes.erase(nodeToBeRemoved);
        }
#endif
    }

    void pruneUnreachableNodes() {
        //return;
#if 1
        std::unordered_set<size_t> reached, all;
        auto allNodeIds = getNodeIds();
        all.insert(allNodeIds.begin(), allNodeIds.end());

        // Preserving only the nodes reachable from the initial nodes potentially leading to final states
        for (size_t initial : initial_nodes) {
            std::unordered_set<size_t> visited_src_dst;
            FlexibleGraph<NodeElement, EdgeLabel>::g.DFSUtil(initial, visited_src_dst);
            if (!unordered_intersection(visited_src_dst, final_nodes).empty())//Preserving the nodes only if I am able to at least reach one final state
                reached.insert(visited_src_dst.begin(), visited_src_dst.end());
        }

        std::unordered_set<size_t> candidatesForRemoval = unordered_difference(all, reached);

        // std::cerr << "Removal candidates: #" << candidatesForRemoval.size() << std::endl;
        for (size_t nodeToBeRemoved : candidatesForRemoval) {
            removed_nodes.insert(nodeToBeRemoved);
            initial_nodes.erase(nodeToBeRemoved);
            final_nodes.erase(nodeToBeRemoved);
            /*std::vector<size_t>& vec = FlexibleGraph<NodeElement, EdgeLabel>::nodeLabelInv.at(
                    FlexibleGraph<NodeElement, EdgeLabel>::getNodeLabel(nodeToBeRemoved));
            vec.erase(std::remove(vec.begin(), vec.end(), nodeToBeRemoved), vec.end());*/
        }
#endif
    }

    FlexibleFA<size_t, NodeElement> shiftLabelsToEdges() const {

        std::cout << "Shifting" << std::endl;
        FlexibleFA<size_t, NodeElement> result;
        size_t start = result.addNewNodeWithLabel(-1);
        std::unordered_map<size_t, size_t> node_id_conversion;
        result.addToInitialNodesFromId(start);

        for (size_t nodeId : getNodeIds()) {
            auto nodeLabel = getNodeLabel(nodeId);
            size_t neueId = result.addNewNodeWithLabel(nodeId);
            node_id_conversion.emplace(nodeId, neueId);

            if (initial_nodes.contains(nodeId)) {
                result.addNewEdgeFromId(start, neueId, nodeLabel);
            }
            if (final_nodes.contains(nodeId)) {
                result.addToFinalNodesFromId(neueId);
            }
        }
        for (size_t i : getNodeIds()) {
            for (const std::pair<EdgeLabel, size_t>& edge : outgoingEdges(i)) {
                result.addNewEdgeFromId(node_id_conversion.at(i), node_id_conversion.at(edge.second), getNodeLabel(edge.second));
            }
        }
        return result;
    }

    void ignoreNodeLabels2(FlexibleFA<size_t, EdgeLabel> &multigraph) const {

        std::unordered_map<size_t, size_t> inverseMap;
        size_t incr = 0;
        for (size_t element : getNodeIds()) {
            int neueId = multigraph.addNewNodeWithLabel((incr++));
            inverseMap[element] = neueId;
            if (isFinalNodeByID(element))
                multigraph.addToFinalNodesFromId(neueId);
            if (isInitialNodeByID(element))
                multigraph.addToInitialNodesFromId(neueId);
        }

        for (size_t element : getNodeIds()) {
            size_t srcId = inverseMap.at(element);
            for (const std::pair<EdgeLabel, size_t>& edge : outgoingEdges(element)) {
                size_t dstId = inverseMap.at(edge.second);
                multigraph.addNewEdgeFromId(srcId, dstId, edge.first);
            }
        }
    }

    void ignoreNodeLabels(FlexibleFA<std::string, EdgeLabel> &multigraph)  {

        std::unordered_map<size_t, size_t> inverseMap;
        size_t incr = 0;
        for (size_t element : getNodeIds()) {
            int neueId = multigraph.addNewNodeWithLabel(std::to_string(incr++));
            inverseMap[element] = neueId;
            if (isFinalNodeByID(element))
                multigraph.addToFinalNodesFromId(neueId);
            if (isInitialNodeByID(element))
                multigraph.addToInitialNodesFromId(neueId);
        }

        for (size_t element : getNodeIds()) {
            size_t srcId = inverseMap.at(element);
            for (const std::pair<EdgeLabel, size_t>& edge : outgoingEdges(element)) {
                size_t dstId = inverseMap.at(edge.second);
                multigraph.addNewEdgeFromId(srcId, dstId, edge.first);
            }
        }
    }


    FlexibleFA<EdgeLabel, NodeElement> shiftLabelsToNodes() const {
        FlexibleFA<EdgeLabel, NodeElement> result;
        std::unordered_map<int, int> edgeToNewNodeMap;
        std::unordered_map<int, std::vector<int>> outgoingEdges;

        for (size_t arcId : getEdgeIds()) {
            auto arc = FlexibleGraph<NodeElement, EdgeLabel>::g.edge_ids.at(arcId);
            int arcSrc = arc.first;
            outgoingEdges[arcSrc].emplace_back(arcId);
            int arcIdAsNode = result.addNewNodeWithLabel(FlexibleGraph<NodeElement, EdgeLabel>::getEdgeLabel(arcId));
            edgeToNewNodeMap.emplace(arcId, arcIdAsNode);
            if (initial_nodes.contains(arcSrc))
                result.addToInitialNodesFromId(arcIdAsNode);
            if (final_nodes.contains(arc.second))
                result.addToFinalNodesFromId(arcIdAsNode);
        }
        for (size_t arcId : getEdgeIds()) {
            auto arc = FlexibleGraph<NodeElement, EdgeLabel>::g.edge_ids.at(arcId);
            size_t src = edgeToNewNodeMap.at(arcId);
            int origArcDst = arc.second;
            auto it = outgoingEdges.find(origArcDst);
            if (it != outgoingEdges.end()) {
                auto dstLabel = FlexibleGraph<NodeElement, EdgeLabel>::getNodeLabel(origArcDst);
                for (int arc2 : it->second) {
                    result.addNewEdgeFromId(src, edgeToNewNodeMap.at(arc2), dstLabel);
                }
            }
        }
        return result;
    }



    template <typename NL, typename EL>
    static FlexibleFA<NL, EL> crossProductWithNodeLabels(FlexibleFA<NL, EL>& lhs,
                                                         FlexibleFA<NL, EL>& rhs, bool doPruning = true) {


        // Performing the node label's intersection
        typename std::unordered_map<NL, std::vector<size_t>>::iterator beg, end;
        typename std::unordered_map<NL, std::vector<size_t>>* toCheckIn = &lhs.nodeLabelInv;
        if (lhs.nodeLabelInv.size() < rhs.nodeLabelInv.size()) {
            beg = lhs.nodeLabelInv.begin();
            end = lhs.nodeLabelInv.end();
            toCheckIn = &rhs.nodeLabelInv;
        } else {
            beg = rhs.nodeLabelInv.begin();
            end = rhs.nodeLabelInv.end();
            toCheckIn = &lhs.nodeLabelInv;
        }

        FlexibleFA<NL, EL> result;
        std::unordered_set<NL> commonNodeLabels;

        // Shared nodes' label intersection
        std::pair<size_t, size_t> srcNodePair, dstNodePair;
        std::unordered_map<std::pair<size_t, size_t>, size_t> pairNodesToDestGraphId;
        for (; beg != end; beg++) {
            // Nodes have the same label
            auto key = beg->first;
            auto it2 = toCheckIn->find(key);
            if (toCheckIn->contains(key)) {
                commonNodeLabels.emplace(key);
            }
        }

        // Join between vertices and edges!
        for (const auto& key: commonNodeLabels) {
            std::vector<size_t>& srcLeft = lhs.nodeLabelInv.at(key);
            std::vector<size_t>& srcRight = rhs.nodeLabelInv.at(key);

            for (size_t i = 0, N = srcLeft.size(); i<N; i++) {
                srcNodePair.first = srcLeft.at(i);
                for (size_t j = 0, M = srcRight.size(); j<M; j++) {
                    srcNodePair.second = srcRight.at(j);


                    size_t src;
                    auto nodeId = pairNodesToDestGraphId.find(srcNodePair);
                    if (nodeId == pairNodesToDestGraphId.end()) {
                        src = (result.addNewNodeWithLabel(key));
                        pairNodesToDestGraphId[srcNodePair] = src;
                        if ((lhs.isInitialNodeByID(srcNodePair.first)) && (rhs.isInitialNodeByID(srcNodePair.second))) {
                            result.addToInitialNodesFromId(src);
                        }
                        if ((lhs.isFinalNodeByID(srcNodePair.first)) && (rhs.isFinalNodeByID(srcNodePair.second))) {
                            result.addToFinalNodesFromId(src);
                        }
                    } else {
                        src = (nodeId->second);
                    }


                    std::unordered_map<NL, std::vector<std::pair<EL, size_t>>> rightEdgesMap;
                    for (const auto& rightEdge : rhs.outgoingEdges(srcNodePair.second)) {
                        const auto& rightNodeLabel = rhs.getNodeLabel(rightEdge.second);
                        if (commonNodeLabels.contains(rightNodeLabel)) {
                            rightEdgesMap[rightNodeLabel].emplace_back(rightEdge);
                        }
                    }
                    for (const auto& leftEdge : lhs.outgoingEdges(srcNodePair.first)) {
                        const auto& dstNodeLabel = lhs.getNodeLabel(leftEdge.second);
                        auto it = rightEdgesMap.find(dstNodeLabel);
                        if (it != rightEdgesMap.end()) {
                            dstNodePair.first = leftEdge.second;
                            for (const auto& rightEdge : it->second) {
                                dstNodePair.second = rightEdge.second;

                                EL resultingEL = (leftEdge.first + rightEdge.first);
                                size_t dst;
                                auto nodeDstId = pairNodesToDestGraphId.find(dstNodePair);
                                if (nodeDstId == pairNodesToDestGraphId.end()) {
                                    dst = (result.addNewNodeWithLabel(dstNodeLabel));
                                    pairNodesToDestGraphId[dstNodePair] = dst;
                                    if ((lhs.isInitialNodeByID(dstNodePair.first)) && (rhs.isInitialNodeByID(dstNodePair.second))) {
                                        result.addToInitialNodesFromId(dst);
                                    }
                                    if ((lhs.isFinalNodeByID(dstNodePair.first)) && (rhs.isFinalNodeByID(dstNodePair.second))) {
                                        result.addToFinalNodesFromId(dst);
                                    }
                                } else {
                                    dst = (nodeDstId->second);
                                }

                                result.addNewEdgeFromId(src, dst, resultingEL);
                            }
                        }
                    }
                }
            }
        }
        if (doPruning) result.pruneUnreachableNodes();
        return result;
    }

    std::unordered_set<EdgeLabel> getAllActionSet() const {
        std::unordered_set<EdgeLabel> result;
        for (const auto& v : getNodeIds()) {
            for (const auto& u : outgoingEdges(v)) {
                result.emplace(u.first);
            }
        }
        return result;
    }

    FlexibleFA<NodeElement, EdgeLabel>& makeDFAAsInTheory(const std::unordered_set<EdgeLabel>& additional = {}) {
        ///std::cout << "makeDFAAsInTheory" << std::endl;
        std::unordered_set<EdgeLabel> acts = getAllActionSet();
        acts.insert(additional.begin(), additional.end());
        bool insertBottom = false;
        size_t botNode = -1;
        size_t actsSize = acts.size();
        for (size_t nodeId : getNodeIds()) {
            std::vector<std::pair<EdgeLabel, size_t>> E = outgoingEdges(nodeId);
            if (E.size() == actsSize) continue;
            else {
                if (!insertBottom) {
                    botNode = FlexibleGraph<NodeElement, EdgeLabel>::addNewNodeWithLabel(NodeElement());
                    for (const EdgeLabel& label : acts) {
                        addNewEdgeFromId(botNode, botNode, label);
                    }
                    insertBottom = true;
                }
                std::unordered_set<EdgeLabel> curr;
                for (const auto& cp : E)
                    curr.insert(cp.first);
                for (const EdgeLabel& label : unordered_difference(acts, curr)) {
                    addNewEdgeFromId(nodeId, botNode, label);
                }
            }
        }
        return *this;
    }


};

#endif //CLASSIFIERS_FLEXIBLEFA_H
