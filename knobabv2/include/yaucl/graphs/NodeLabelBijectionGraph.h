/*
 * NodeLabelBijectionGraph.h
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

#ifndef GRAPHOS_GRAPH_H
#define GRAPHOS_GRAPH_H

#include <yaucl/graphs/adjacency_graph.h>
#include <unordered_map>
#include <optional>
#include <cassert>

template <typename NodeElement, typename EdgeLabel>
class NodeLabelBijectionGraph {
protected:
    std::unordered_map<size_t , NodeElement> nodeLabelInv;
    std::unordered_map<NodeElement, size_t> nodeLabel;
    std::unordered_map<size_t, EdgeLabel> costMap;

public:
    adjacency_graph g;

    NodeLabelBijectionGraph()  {}
    NodeLabelBijectionGraph(const NodeLabelBijectionGraph<NodeElement, EdgeLabel>& x) = default;
    NodeLabelBijectionGraph(NodeLabelBijectionGraph<NodeElement, EdgeLabel>&& x) = default;
    NodeLabelBijectionGraph<NodeElement, EdgeLabel>& operator=(const NodeLabelBijectionGraph<NodeElement, EdgeLabel>& x) = default;
    NodeLabelBijectionGraph<NodeElement, EdgeLabel>& operator=(NodeLabelBijectionGraph<NodeElement, EdgeLabel>&& x) = default;

    size_t maximumNodeId() const {
        return g.V_size;
    }

    size_t maximumEdgeId() const {
        return g.E_size;
    }

    std::unordered_set<EdgeLabel> getAllActionSet() const {
        std::unordered_set<EdgeLabel> result;
        for (const auto& cp : costMap)
            result.insert(cp.second);
        return result;
    }


    NodeElement getUniqueLabel(size_t id) const {
        return nodeLabelInv.at(id);
    }

    EdgeLabel getEdgeLabel(size_t id) const {
        return costMap.at(id);
    }

    size_t addUniqueStateOrGetExisting(const NodeElement& node) {
        auto it = nodeLabel.find(node);
        if (it == nodeLabel.end()) {
            auto graphNode = g.add_node();
            DEBUG_ASSERT(nodeLabelInv.insert(std::make_pair((graphNode), node)).second);
            return nodeLabel.insert(std::make_pair(node, (graphNode))).first->second;
        } else {
            return it->second;
        }
    }

    void addNewEdgeFromId(size_t src2, size_t dst2, const EdgeLabel& weight) {
        costMap[g.add_edge(src2, dst2)] = weight;
    }

    void addNewEdgeR(const NodeElement& src, size_t dst2, const EdgeLabel& weight) {
        addNewEdgeFromId(addUniqueStateOrGetExisting(src), dst2, weight);
    }

    void addNewEdgeL(size_t src2, const NodeElement& dst, const EdgeLabel& weight) {
        addNewEdgeFromId(src2, addUniqueStateOrGetExisting(dst), weight);
    }

    void addNewEdgeB(const NodeElement& src, const NodeElement& dst, const EdgeLabel& weight) {
        addNewEdgeFromId(addUniqueStateOrGetExisting(src), addUniqueStateOrGetExisting(dst), weight);
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

    std::vector<std::pair<EdgeLabel, size_t>> outgoingEdges(const NodeElement& n) const {
        return outgoingEdges(nodeLabel.at(n));
    }

    std::vector<std::pair<EdgeLabel, size_t>> ingoingEdges(const NodeElement& n) const {
        return ingoingEdgesById(nodeLabel.at(n));
    }

    std::optional<size_t> hasNode(const NodeElement& node) {
        auto it = nodeLabel.find(node);
        return (it == nodeLabel.end()) ? std::optional<size_t>() : std::optional{it->second};
    }

    size_t getId(const NodeElement& node) const {
        return nodeLabel.at(node);
    }
};


#endif //GRAPHOS_GRAPH_H
