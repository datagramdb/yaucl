//
// Created by giacomo on 15/02/23.
//

#ifndef KNOBAB_SERVER_KOSARAJUSCC_H
#define KNOBAB_SERVER_KOSARAJUSCC_H

#include <yaucl/graphs/FlexibleFA.h>
#include <yaucl/structures/any_to_uint_bimap.h>
#include <stack>

template <typename NodeLabel, typename EdgeLabel>
struct KosarajuSCC  {
    const FlexibleFA<NodeLabel, EdgeLabel>& g;

    KosarajuSCC(const FlexibleFA<NodeLabel, EdgeLabel>& g) : g{g} {};

    void run(std::vector<roaring::Roaring64Map>& actual_result) {
        std::stack<size_t> stack;
        std::stack<std::pair<size_t,bool>> dfsStackCallStack;
        roaring::Roaring64Map visited;
        yaucl::structures::any_to_uint_bimap<size_t> node_conversion; // For creating transposed graph
        adjacency_graph transposed_graph;
        std::vector<roaring::Roaring64Map> cc_result;
        actual_result.clear();

        // dfsStack
        auto v = g.getNodeIds();
        for (size_t u : v) {
            auto exp = node_conversion.put(u).first;
            auto id = transposed_graph.add_node();
            DEBUG_ASSERT(exp == id);
            dfsStackCallStack.emplace(u, false);
            while (!dfsStackCallStack.empty()) {
                auto cp = dfsStackCallStack.top();
                dfsStackCallStack.pop();
                if (cp.second) {
                    stack.push(cp.first);
                } else {
                    visited.add(cp.first);
                    dfsStackCallStack.emplace(u, true);
                    for (const auto& edgeRef : g.outgoingEdges(cp.first)) {
                        if (!visited.contains(edgeRef.second))
                            dfsStackCallStack.emplace(edgeRef.second, false);
                    }
                }
            }
        }

        for (size_t u : v) {
            for (const auto& edgeRef : g.outgoingEdges(u)) {
                transposed_graph.add_edge(node_conversion.getValue(edgeRef.second),
                                          node_conversion.getValue(u));
            }
        }

        size_t count = 0;
        std::vector<size_t> order(transposed_graph.V_size, 0),
                            id(transposed_graph.V_size, 0);
        std::stack<size_t> ccdfs_stack;
        for (size_t i = 0; i<transposed_graph.V_size; i++) {
            order[i] = stack.top();
            stack.pop();
        }

        // Bertossi's cc (with order!)
        for (int i = 0; i<transposed_graph.V_size; i++) {
            if (id.at(order.at(i)) == 0) {
                count++;
                // do ccdfs
                ccdfs_stack.push(order.at(i));
                while (!ccdfs_stack.empty()) {
                    size_t u = ccdfs_stack.top();
                    ccdfs_stack.pop();
                    id[u] = count;
                    for (size_t e : transposed_graph.nodes.at(u)) {
                        auto v = transposed_graph.edge_ids.at(e).second;
                        if (id.at(v) == 0)
                            ccdfs_stack.push(v);
                    }
                }
            }
        }

        std::unordered_map<size_t, roaring::Roaring64Map> res;
        for (size_t i = 0; i < transposed_graph.V_size; i++) {
            res[id.at(i)].add(node_conversion.getKey(i));
        }

//        connected_components(transposed_graph, cc_result);
        actual_result.reserve(res.size());
        for (const auto& [k,v] : res) {
            actual_result.emplace_back(v);
        }
    }

};

#endif //KNOBAB_SERVER_KOSARAJUSCC_H
