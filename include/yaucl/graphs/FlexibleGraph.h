/*
 * FlexibleGraph.h
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

#ifndef CLASSIFIERS_FLEXIBLEGRAPH_H
#define CLASSIFIERS_FLEXIBLEGRAPH_H

//
// Created by giacomo on 18/02/21.
//


#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <cassert>
#include "yaucl/graphs/algorithms/connected_components.h"

#include <yaucl/graphs/adjacency_graph.h>
#include <yaucl/hashing/pair_hash.h>
//#include <ranges>
#include <functional>

template <typename NodeElement, typename EdgeLabel>
class FlexibleGraph {
protected:
    std::unordered_map<size_t, NodeElement> nodeLabel;
    std::unordered_map<size_t, EdgeLabel> costMap;

public:
    adjacency_graph g;

    std::vector<roaring::Roaring64Map> getConnectedComponents() {
        std::vector<roaring::Roaring64Map> map;
        connected_components(g, map);
        return map;
    }

    ssize_t getConnectedComponents(const roaring::Roaring64Map& starting_points,
                                   std::vector<roaring::Roaring64Map>& result) {
        std::vector<roaring::Roaring64Map> map;
        return connected_components(g, starting_points, map);
    }

    std::unordered_map<NodeElement, std::vector<size_t>> nodeLabelInv;

    virtual void clear() {
        nodeLabel.clear();
        costMap.clear();
        nodeLabelInv.clear();
        g.clear();
    }

    virtual ~FlexibleGraph() {
        nodeLabel.clear();
        costMap.clear();
        nodeLabelInv.clear();
        g.clear();
    }
    FlexibleGraph() = default;
    FlexibleGraph(const FlexibleGraph& x) = default;
    FlexibleGraph(FlexibleGraph&& x) = default;
    FlexibleGraph& operator=(const FlexibleGraph& x) = default;
    FlexibleGraph& operator=(FlexibleGraph&& x) = default;

    size_t vertexSize() const {
        return g.V_size;
    }

    size_t edgeSize() const {
        return g.E_size;
    }

    virtual const NodeElement& getNodeLabel(size_t id) const {
        return nodeLabel.at(id);
    }

    bool updateNodeLabel(size_t id, const NodeElement& label) {
        auto it = nodeLabel.find(id);
        if (it->second != label) {
            auto& vec = nodeLabelInv[it->second];
            vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
            if (vec.empty()) nodeLabelInv.erase(it->second);
            nodeLabelInv[label].emplace_back(id);
            it->second = label;
            return true;
        } else
            return false;
    }

    virtual const EdgeLabel& getEdgeLabel(size_t id) const {
        return costMap.at(id);
    }

    std::unordered_set<EdgeLabel> getAllActionSet() const {
        std::unordered_set<EdgeLabel> result;
        for (const auto& cp : costMap)
            result.insert(cp.second);
        return result;
    }

    size_t addNewNodeWithLabel(const NodeElement& label) {
        auto graphNode = g.add_node();
        nodeLabel[graphNode] = label;
        nodeLabelInv[label].emplace_back(graphNode);
        return graphNode;
    }


    virtual size_t addNewEdgeFromId(size_t src2, size_t dst2, const EdgeLabel& weight) {
        size_t edgeId = g.add_edge(src2, dst2);
        costMap[edgeId] = weight;
        return edgeId;
    }

    virtual std::vector<std::pair<EdgeLabel, size_t>> outgoingEdges(size_t srcNode) const {
        std::vector<std::pair<EdgeLabel, size_t>> outgoings;
        for (const size_t & edge : g.getOutgoingEdgesId(srcNode)) {
            outgoings.emplace_back(costMap.at(edge), g.edge_from_id(edge).second);
        }
        return outgoings;
    }

    virtual std::vector<std::pair<EdgeLabel, size_t>> ingoingEdges(size_t srcNode) const {
        std::vector<std::pair<EdgeLabel, size_t>> outgoings;
        for (const size_t & edge : g.getIngoingEdgesId(srcNode)) {
            outgoings.emplace_back(costMap.at(edge), g.edge_from_id(edge).first);
        }
        return outgoings;
    }

    bool hasNodeLabel(const NodeElement& node) const {
        return nodeLabelInv.contains(node);
    }

    std::vector<size_t> getIdsFromLabel(const NodeElement& node) const {
        return nodeLabelInv.at(node);
    }

    template<typename NL, typename EL>
    static FlexibleGraph<NL, EL> crossProductWithNodeLabels(const FlexibleGraph<NL, EL> lhs,
                                                            const FlexibleGraph<NL, EL>& rhs, std::function<EL(const EL&, const EL&)> edgeCombiner) {

        // Performing the node label's intersection
        typename std::unordered_map<NL, std::vector<int>>::iterator beg, end;
        typename std::unordered_map<NL, std::vector<int>>& toCheckIn = &lhs;
        if (lhs.nodeLabelInv.size() < rhs.nodeLabelInv.size()) {
            beg = lhs.nodeLabelInv.begin();
            end = lhs.nodeLabelInv.end();
            toCheckIn = &rhs;
        } else {
            beg = rhs.nodeLabelInv.begin();
            end = rhs.nodeLabelInv.end();
            toCheckIn = & lhs;
        }

        FlexibleGraph<NL, EL> result;
        std::unordered_set<NL> commonNodeLabels;

        // Shared nodes' label intersection
        std::pair<int, int> srcNodePair, dstNodePair;
        std::unordered_map<std::pair<int, int>, int> pairNodesToDestGraphId;
        for (; beg != end; beg++) {
            // Nodes have the same label
            auto key = beg->first;
            auto it2 = toCheckIn.find(key);
            if (toCheckIn.contains(key)) {
                commonNodeLabels.emplace(key);
            }
        }

        // Join between vertices and edges!
        for (const auto& key: commonNodeLabels) {
            std::vector<int>& srcLeft = lhs.nodeLabelInv.at(key);
            std::vector<int>& srcRight = rhs.nodeLabelInv.at(key);

            for (size_t i = 0, N = srcLeft.size(); i<N; i++) {
                srcNodePair.first = srcLeft.at(i);
                for (size_t j = 0, M = srcRight.size(); j<M; j++) {
                    srcNodePair.second = srcRight.at(j);

                    int src;
                    auto nodeId = pairNodesToDestGraphId.find(srcNodePair);
                    if (nodeId == pairNodesToDestGraphId.end()) {
                        src = (result.addNewNodeWithLabel(key));
                    } else {
                        src = (nodeId->second);
                    }

                    std::unordered_map<NL, std::vector<std::pair<EL, int>>> rightEdgesMap;
                    for (const std::pair<EL, int>& rightEdge : rhs.outgoingEdges(srcNodePair.second)) {
                        auto& rightNodeLabel = lhs.getNodeLabel(rightEdge.second);
                        if (commonNodeLabels.contains(rightNodeLabel)) {
                            rightEdgesMap[rightNodeLabel].emplace_back(rightEdge);
                        }
                    }
                    for (const std::pair<EL, int>& leftEdge : lhs.outgoingEdges(srcNodePair.first)) {
                        auto& dstNodeLabel = lhs.getNodeLabel(leftEdge.second);
                        auto it = rightEdgesMap.find(dstNodeLabel);
                        if (it != rightEdgesMap.end()) {
                            dstNodePair.first = leftEdge.second;
                            for (const std::pair<EL, int>& rightEdge : it->second) {
                                dstNodePair.second = rightEdge.second;

                                EL resultingEL = edgeCombiner(leftEdge.second, rightEdge.second);
                                int dst;
                                auto nodeDstId = pairNodesToDestGraphId.find(dstNodePair);
                                if (nodeDstId == pairNodesToDestGraphId.end()) {
                                    dst = (result.addNewNodeWithLabel(dstNodeLabel));
                                } else {
                                    dst = (nodeId->second);
                                }

                                result.addNewEdgeFromId(src, dst, resultingEL);
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    void dot(std::ostream& os, bool ignoreEdgeLabels = false, const std::string& shape="circle") const {
        os << "digraph finite_state_machine {\n"
              "    rankdir=LR;\n"
              "    size=\"8,5\"\n";
        for (size_t node_id = 0, N = vertexSize(); node_id < N; node_id++) {
            ///TODO: if (removed_nodes.contains(node_id)) continue;
//            std::string shape = "circle";
            os << "node [shape = " << shape << ", label=\"" << getNodeLabel(node_id) << "\", fontsize=10] q" << node_id << ";\n";
        }
        os << "\n\n";
        for (size_t node_id = 0, N = vertexSize(); node_id < N; node_id++) {
            ///TODO: if (removed_nodes.contains(node_id)) continue;
//            std::string shape = "circle";
            for (const std::pair<EdgeLabel, int>& edge : outgoingEdges(node_id)) {
                os << "q" << node_id << " -> q" << edge.second;
                if (!ignoreEdgeLabels)
                    os << " [ label = \"" << edge.first << "\"]";
                os << ";\n";
            }
        }
        os << "}";
    }
    const std::pair<size_t, size_t> &resolveEdgeById(size_t i) const {
        return g.edge_ids.at(i);
    }
};



#endif //CLASSIFIERS_FLEXIBLEGRAPH_H
