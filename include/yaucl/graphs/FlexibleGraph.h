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
#include "yaucl/graphs/adjacency_graph.h"
//#include <ranges>
#include <functional>

template <typename NodeElement, typename EdgeLabel>
class FlexibleGraph {
protected:
    adjacency_graph g;
    std::unordered_map<size_t, NodeElement> nodeLabel;
    std::unordered_map<size_t, EdgeLabel> costMap;

public:
    std::unordered_map<NodeElement, std::vector<size_t>> nodeLabelInv;

    virtual ~FlexibleGraph() {
        nodeLabel.clear();
        costMap.clear();
        nodeLabelInv.clear();
    }
    FlexibleGraph() = default;
    FlexibleGraph(const FlexibleGraph& x) = default;
    FlexibleGraph(FlexibleGraph&& x) = default;
    FlexibleGraph& operator=(const FlexibleGraph& x) = default;
    FlexibleGraph& operator=(FlexibleGraph&& x) = default;

    size_t maximumNodeId() const {
        return g.V_size;
    }

    size_t maximumEdgeId() const {
        return g.E_size;
    }

    virtual NodeElement getNodeLabel(size_t id) const {
        return nodeLabel.at(id);
    }

    virtual EdgeLabel getEdgeLabel(size_t id) const {
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

    virtual void addNewEdgeFromId(size_t src2, size_t dst2, const EdgeLabel& weight) {
        costMap[g.add_edge(src2, dst2)] = weight;
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

};



#endif //CLASSIFIERS_FLEXIBLEGRAPH_H
