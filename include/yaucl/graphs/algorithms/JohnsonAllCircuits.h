//
// Created by giacomo on 14/02/23.
//

#ifndef KNOBAB_SERVER_JOHNSONALLCIRCUITS_H
#define KNOBAB_SERVER_JOHNSONALLCIRCUITS_H

#include <yaucl/graphs/FlexibleFA.h>
#include <yaucl/graphs/algorithms/KosarajuSCC.h>
#include <stack>
#include <deque>

template <typename NodeLabel, typename EdgeLabel>
struct JohnsonAllCircuits {
    FlexibleFA<NodeLabel, EdgeLabel>& g;
    JohnsonAllCircuits(FlexibleFA<NodeLabel, EdgeLabel>& g) : g{g} {};

    std::unordered_map<size_t, std::deque<size_t>> blockedNodes;
    std::vector<std::vector<size_t>> circuits;
    roaring::Roaring64Map blocked;

    void run(size_t sample_max_per_node = std::numeric_limits<size_t>::max()) {
        KosarajuSCC scc_algo{g};
        roaring::Roaring64Map allNodes, originally_removed;
        originally_removed = g.removed_nodes;
        allNodes.addRange(0, g.g.V_size);
        allNodes -= g.removed_nodes;
        std::vector<roaring::Roaring64Map> scc;
        scc_algo.run(scc);
        std::vector<size_t> stack;
        std::sort(scc.begin(), scc.end(), [](const auto& x, const auto& y) {return  x.cardinality()<y.cardinality();});
        size_t i = 1;
        for (roaring::Roaring64Map component : scc) {
            i++;
            roaring::Roaring64Map now_removed = allNodes - component;
            g.removed_nodes = now_removed;
            // Projection graph over the new strongly connected component
            for (size_t s : component) {
                blocked.clear();
                blockedNodes.clear();
                for (size_t u : component) blockedNodes[u] = {};
                size_t curr_smpn = sample_max_per_node;
                circuit(s, s, stack, curr_smpn);
            }
        }
        std::swap(g.removed_nodes, originally_removed);
        g.removed_nodes = originally_removed;
    }



//
    void unblock(size_t u) {
        blocked.remove(u);
        auto it = blockedNodes.find(u);
        if (it == blockedNodes.end()) return;
        while (!it->second.empty()) {
            auto w = it->second.front();
            it->second.pop_front();
            if (blocked.contains(w)) {
                unblock(w);
            }
        }
    }


     bool circuit(size_t v, size_t s, std::vector<size_t>& stack, size_t& sample_max_per_node) {
        if (sample_max_per_node == 0) return true;
                std::deque<size_t> ed;
                if ((g.vertexSize()) == 0) { return false; }
                bool f = false;
                stack.push_back(v);
                blocked.add(v);
                for (const std::pair<EdgeLabel, size_t>& edge_id : g.outgoingEdges(v)) {
                    if (sample_max_per_node == 0) {
                        stack.clear();
                        return true;
                    }
                    size_t w = edge_id.second;
                    if (w == s) {
                        stack.push_back(s);
                        circuits.emplace_back(stack);
                        sample_max_per_node--;
                        if (sample_max_per_node == 0) {
                            stack.clear();
                            return true;
                        }
                        stack.pop_back();
                        f = true;
                    }
                    else {
                        if (! blocked.contains(w)) {
                            if (circuit(w, s, stack, sample_max_per_node)) { f = true; }
                        }
                    }
                }
                if (f) {
                    unblock(v);
                    if (sample_max_per_node == 0) {
                        stack.clear();
                        return true;
                    }
                }
                else {
                    for (const std::pair<EdgeLabel, size_t>& edge_id : g.outgoingEdges(v)) {
                        size_t w = edge_id.second;
                        auto it = blockedNodes.emplace(w, ed);
                        if (std::find(it.first->second.begin(), it.first->second.end(),v) != it.first->second.end()) {
                            it.first->second.push_back(v);
                        }
                    }
                }
                stack.pop_back();
                return f;
    }

};


#endif //KNOBAB_SERVER_JOHNSONALLCIRCUITS_H
