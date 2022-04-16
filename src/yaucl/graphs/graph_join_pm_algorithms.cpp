//
// Created by giacomo on 26/07/21.
//

#include <yaucl/graphs/graph_join_pm_algorithms.h>

#include <stack>

void approximated_topo_sort(graph_join_pm &g, std::vector<size_t> &order) {
    size_t N = g.V_size;
    order.resize(N, 0);
    std::vector<bool> visited(N, false);
    size_t curr_pos = N;

    std::stack<std::pair<size_t, bool>> visitStack;
    for (size_t initial_node : g.starting_point)
        visitStack.emplace(initial_node, false);

    // DFS Visit
    while (!visitStack.empty()) {
        size_t u;
        bool u_visited;
        std::pair<size_t, bool> cp = visitStack.top();
        u = cp.first;
        u_visited = cp.second;
        visitStack.pop();
        if (u_visited) {
            // If it was already visited before continuing the visit, then put it back to the stack
            order[--curr_pos] = u;
        } else {
            if (!visited.at(u)) {
                // Visit the outoing edges
                visited[u] = true;
                const adjacency_entry& ref = resolve_node_const(g, u);
                if (!ref.outgoing_hash_to_outgoingNodeIdWithWeight.empty()) {
                    // Remember to visit the node back after visiting the adjacent nodes
                    visitStack.emplace(u, true);
                    for (const auto& cp : ref.outgoing_hash_to_outgoingNodeIdWithWeight) {
                        for (size_t i = 0, N = cp.second.size(); i < N; i++) {
                            // Visiting the adjacent nodes
                            visitStack.emplace(cp.second.at(i).first, false);
                        }
                    }
                } else {
                    order[--curr_pos] = u;
                }
            }
        }
    }
}



#include <yaucl/hashing/hash_combine.h>
#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/functional/iterators.h>
#include <yaucl/functional/assert.h>

void rec_mark(std::unordered_map<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>>& table,
              std::unordered_set<std::pair<size_t, size_t>>& marked,
              std::pair<size_t, size_t> cp) {
    if (cp.first > cp.second) std::swap(cp.first, cp.second);
    if (marked.insert(cp).second) {
        auto it = table.find(cp);
        DEBUG_ASSERT(it != table.end());
        std::vector<std::pair<size_t, size_t>> V;
        std::swap(V, it->second);
        for (const auto& cp2 : V)
            rec_mark(table, marked, cp2);
    }

}


void getNodeClustersFromLabel(graph_join_pm &graph,
                              std::unordered_map<size_t, std::unordered_set<size_t>> &clustersPerClusteredNode) {
    DEBUG_ASSERT(graph.V_size == graph.vertex_id_secondary_index.size()); // Guaranteeing that the access is ok

    std::vector<size_t> order;
    std::unordered_map<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>> table;
    std::unordered_set<std::pair<size_t, size_t>> marked;
    std::unordered_map<std::string, std::pair<std::vector<size_t>, std::vector<size_t>>> element_map_finalNotFinal{};

    approximated_topo_sort(graph, order);
    for (size_t node_id = 0, N = graph.V_size; node_id<N; node_id++) {
        const std::pair<std::string, size_t>& cp  = graph.vertex_id_secondary_index.at(node_id);
        const std::string& label = cp.first;
        DEBUG_ASSERT(graph.vertex_hash_primary_index.contains(label));
        auto& it2 = graph.vertex_hash_primary_index.at(label);
        DEBUG_ASSERT(it2.size() > cp.second);
        auto& ref = graph.vertex_hash_primary_index.at(label).at(cp.second);
        auto it = element_map_finalNotFinal.find(label);
        if (it == element_map_finalNotFinal.end()) {
            if (ref.is_final) {
                element_map_finalNotFinal[label] = {{node_id}, {}};
            } else {
                element_map_finalNotFinal[label] = {{}, {node_id}};
            }
        } else {
            if (ref.is_final) {
                it->second.first.emplace_back(node_id);
            } else {
                it->second.second.emplace_back(node_id);
            }
        }
    }

    for (auto& it : element_map_finalNotFinal) {
        size_t N = it.second.first.size();
        size_t left, right;
        if (it.second.first.size() > 1) {
            std::sort(it.second.first.begin(), it.second.first.end(), [order](size_t a, size_t b) {
                return order.at(a) < order.at(b);
            });
            for (size_t i = 0; i<N; i++) {
                size_t M = it.second.first.at(i);
                for (size_t j = 0; j<i; j++) {
                    size_t O = it.second.first.at(j);
                    if (M<O) {
                        left = M;
                        right = O;
                        table[std::make_pair(left, right)] = {};
                    } else {
                        right = M;
                        left = O;
                        table[std::make_pair(left, right)] = {};
                    }
                }
            }
        }

        N = it.second.second.size();
        if (it.second.second.size() > 1) {
            std::sort(it.second.second.begin(), it.second.second.end(), [order](size_t a, size_t b) {
                return order.at(a) < order.at(b);
            });
            for (size_t i = 0; i<N; i++) {
                size_t M = it.second.second.at(i);
                for (size_t j = 0; j<i; j++) {
                    size_t O = it.second.second.at(j);
                    if (M<O) {
                        left = M;
                        right = O;
                        table[std::make_pair(left, right)] = {};
                    } else {
                        right = M;
                        left = O;
                        table[std::make_pair(left, right)] = {};
                    }
                }
            }
        }
    }

    for (auto& m : table) {
        auto& ref = m.first;
        if (!marked.contains(ref)) {
            auto& left = resolve_node_const(graph, ref.first);
            auto& right = resolve_node_const (graph, ref.second);

            auto it_left = left.outgoing_hash_to_outgoingNodeIdWithWeight.begin(), e_left = left.outgoing_hash_to_outgoingNodeIdWithWeight.end();
            auto it_right = right.outgoing_hash_to_outgoingNodeIdWithWeight.begin(), e_right = right.outgoing_hash_to_outgoingNodeIdWithWeight.end();
            bool hasFoundMarked = false;
            bool noIteration = true;

            while ((it_left != e_left) && (it_right != e_right)) {
                if (it_left->first < it_right->first)
                    DEBUG_ASSERT(false && "The two markings do not conicide!");
                else if (it_left->first > it_right->first)
                    DEBUG_ASSERT(false && "The two markings do not conicide!");
                else {
                    DEBUG_ASSERT(it_left->second.size() == 1);
                    DEBUG_ASSERT(it_right->second.size() == 1);
                    size_t ll = it_left->second.at(0).first;
                    size_t rr = it_right->second.at(0).first;
                    if (ll>rr) std::swap(ll, rr);
                    noIteration = false;
                    if (ll != rr) {
                        std::pair<size_t, size_t> cp{ll, rr};
                        hasFoundMarked = hasFoundMarked ||
                                         (resolve_node_const(graph, ll).is_final != resolve_node_const(graph, rr).is_final) ||
                                         ((!table.contains(cp)) || (marked.contains(cp)));
                    }
                    it_left++;
                    it_right++;
                }
            }

            if (noIteration) {
                hasFoundMarked = true;
            }

            if (hasFoundMarked) {
                rec_mark(table, marked, ref);
            }
        }
    }

    for (auto it = table.begin(); it != table.end(); it++) {
        if (!marked.contains(it->first)) /*
            it = table.erase(it);
        else*/ {
            clustersPerClusteredNode[it->first.first].insert(it->first.second);
            clustersPerClusteredNode[it->first.first].insert(it->first.first);
            clustersPerClusteredNode[it->first.second].insert(it->first.first);
            clustersPerClusteredNode[it->first.second].insert(it->first.second);
        }
    }
    table.clear();
}

void minimize(graph_join_pm &g, graph_join_pm &result) {
    result.doesAcceptEmptyString = g.doesAcceptEmptyString;
    std::unordered_map<size_t, std::unordered_set<size_t>> eq_rel;

    ///std::cout << "Starting minimization" << std::endl;
    getNodeClustersFromLabel(g, eq_rel);


    std::unordered_map<std::unordered_set<size_t>, size_t> mapped_id;
    std::unordered_map<size_t, size_t> old_id_to_id;

    DEBUG_ASSERT(g.vertex_id_secondary_index.size() == g.V_size);
    for (size_t node_id = 0, N = g.V_size; node_id<N; node_id++) {
        auto it = eq_rel.find(node_id);
        if (it == eq_rel.end()) {
            auto& ref = g.vertex_id_secondary_index.at(node_id);
            auto& full = g.vertex_hash_primary_index.at(ref.first).at(ref.second);
            old_id_to_id[node_id] = add_node(result, ref.first, full.is_starting, full.is_final);
        } else {
            auto f = mapped_id.find(it->second);
            if (f == mapped_id.end()) {
                size_t tmp_id = *it->second.begin();
                auto& ref = g.vertex_id_secondary_index.at(tmp_id);
                auto& full = g.vertex_hash_primary_index.at(ref.first).at(ref.second);
                size_t new_id = add_node(result, ref.first, full.is_starting, full.is_final);
                for (size_t k : it->second) {
                    old_id_to_id[k] = new_id;
                }
                mapped_id[it->second] = new_id;
            }
        }
    }

    DEBUG_ASSERT(g.vertex_id_secondary_index.size() == g.V_size);
    for (size_t node_id = 0, N = g.V_size; node_id<N; node_id++) {
        DEBUG_ASSERT(old_id_to_id.contains(node_id));
        size_t src_id = old_id_to_id.at(node_id);
        auto it = eq_rel.find(node_id);
        if (it == eq_rel.end()) {
            const auto& ref = resolve_node_const(g, node_id);
            for (const auto& x : ref.outgoing_hash_to_outgoingNodeIdWithWeight) {
                for (const std::pair<size_t, double>& y : x.second) {
                    DEBUG_ASSERT(old_id_to_id.contains(y.first));
                    size_t dst_id = old_id_to_id.at(y.first);
                    add_edge(result, src_id, dst_id, y.second);
                }
            }
        } else {
            auto it2 = mapped_id.find(it->second);
            if (it2 != mapped_id.end()) {
                src_id = it2->second;
                const auto& ref = resolve_node_const(g, node_id);
                for (const auto& x : ref.outgoing_hash_to_outgoingNodeIdWithWeight) {
                    for (const std::pair<size_t, double>& y : x.second) {
                        DEBUG_ASSERT(old_id_to_id.contains(y.first));
                        size_t dst_id = old_id_to_id.at(y.first);
                        add_edge(result, src_id, dst_id, y.second);
                    }
                }
                mapped_id.erase(it2);
            }
        }
    }

    ///std::cout << "Minimization status: completed!" << std::endl;
}

void invert_graph(graph_join_pm &input, graph_join_pm &output) {
    std::cerr<< "WARNING: inverting the graph is dangerous for empty string preserving (TEST)" << std::endl;
    for (size_t i = 0, N = input.V_size; i<N; i++) {
        auto& ref = input.vertex_id_secondary_index.at(i);
        auto& v_i = input.vertex_hash_primary_index.at(ref.first).at(ref.second);
        add_node(output, ref.first, v_i.is_final, v_i.is_starting);
    }
    for (size_t i = 0, N = input.V_size; i<N; i++) {
        auto& v_i = resolve_node_const(input, i);
        for (const auto& cp : v_i.outgoing_hash_to_outgoingNodeIdWithWeight) {
            for (const auto& edge : cp.second) {
                add_edge(output, edge.first, i, edge.second);
            }
        }
    }
}

#include <stack>
#include <vector>



void remove_unaccepting_states(graph_join_pm &input, graph_join_pm &output) {

    std::vector<bool> forward_visited(input.V_size, false);
    std::vector<bool> backward_visited(input.V_size, false);
    output.doesAcceptEmptyString = input.doesAcceptEmptyString;
    getUnreachableNodesForAcceptance(input, forward_visited, backward_visited);


    // Returning the graph containing the nodes that are both reaching final nodes and that can be reach the final node
    std::unordered_map<size_t, size_t> old_to_new_id;
    for (size_t i = 0, N = input.V_size; i<N; i++) {
        if ((forward_visited.at(i)) && (backward_visited.at(i))) {
            auto& ref = input.vertex_id_secondary_index.at(i);
            auto& v_i = input.vertex_hash_primary_index.at(ref.first).at(ref.second);
            old_to_new_id[i] = add_node(output, ref.first, v_i.is_starting, v_i.is_final);
        }
    }
    for (size_t i = 0, N = input.V_size; i<N; i++) {
        if ((forward_visited.at(i)) && (backward_visited.at(i))) {
            auto& v_i = resolve_node_const(input, i);
            size_t v_i_new = old_to_new_id.at(i);
            for (const auto& cp : v_i.outgoing_hash_to_outgoingNodeIdWithWeight) {
                for (const auto& edge : cp.second) {
                    if ((forward_visited.at(edge.first)) && (backward_visited.at(edge.first))) {
                        add_edge(output, v_i_new, old_to_new_id.at(edge.first), edge.second);
                    }
                }
            }
        }
    }

}

void getUnreachableNodesForAcceptance(graph_join_pm &input, std::vector<bool> &forward_visited,
                                      std::vector<bool> &backward_visited) {// Create a stack for DFS
    std::stack<size_t> stack;

    // Push the current source node.
    for (size_t s : input.starting_point)
        stack.push(s);
    while (!stack.empty()) {
        size_t s = stack.top();
        stack.pop();
        if (!forward_visited[s]) {
            forward_visited[s] = true;
            for (const auto& cp : resolve_node(input, s).outgoing_hash_to_outgoingNodeIdWithWeight)
                for (const auto& edge : cp.second)
                    if (!forward_visited[edge.first])
                        stack.push(edge.first);
        }
    }

    graph_join_pm intermediate;
    invert_graph(input, intermediate);

    // Initially mark all verices as not backward_visited


    // Push the current source node.
    for (size_t s : intermediate.starting_point)
        stack.push(s);

    while (!stack.empty())
    {
        // Pop a vertex from stack and print it
        size_t s = stack.top();
        stack.pop();

        // Stack may contain same vertex twice. So
        // we need to print the popped item only
        // if it is not backward_visited.
        if (!backward_visited[s]) {
            backward_visited[s] = true;
            for (const auto& cp : resolve_node(intermediate, s).outgoing_hash_to_outgoingNodeIdWithWeight)
                for (const auto& edge : cp.second)
                    if (!backward_visited[edge.first])
                        stack.push(edge.first);
        }
    }
}



void make_graph_deterministic::generate_out_graph(graph_join_pm &input,
                        graph_join_pm &output) {
    multiNodesToNewNode.clear();
    output.doesAcceptEmptyString = input.doesAcceptEmptyString;
    std::unordered_map<std::string, std::unordered_set<size_t>> M;
    for (size_t startingNodeId : input.starting_point) {
        auto label = input.vertex_id_secondary_index[startingNodeId].first;
        M[label].insert(startingNodeId);
    }
    for (const auto& ref : M)
        generate_out_graph(input, output, ref.first, ref.second);
}

size_t make_graph_deterministic::generate_out_graph(graph_join_pm &input,
                          graph_join_pm &output,
                          const std::string& label,
                          const std::unordered_set<size_t>& I)  {
    if (multiNodesToNewNode.contains(I)) {
        return multiNodesToNewNode.at(I);
    } else {
        std::unordered_map<std::string, std::unordered_set<size_t>> M;
        size_t srcNodeId;
        bool allStarting = true;
        bool atLeastOneEnding = false;
        for (size_t id : I) {
            if ((!atLeastOneEnding) && resolve_node(input, id).is_final) {
                atLeastOneEnding = true;
            }
            if (!input.starting_point.contains(id)) {
                allStarting = false;
            }
            for (const std::pair<std::string, std::vector<std::pair<size_t, double>>>& edgeId :
                    resolve_node(input, id).outgoing_hash_to_outgoingNodeIdWithWeight) {
                auto& refEntry = M[edgeId.first];
                for (const std::pair<size_t, double>& cp : edgeId.second) {
                    refEntry.insert(cp.first);
                }
            }
        }

        srcNodeId = add_node(output, label, allStarting, atLeastOneEnding);
        multiNodesToNewNode[I] = srcNodeId; // set node as visited

        for (const auto& outgoingEdge : M) {
            size_t dstNodeId = generate_out_graph(input,output, outgoingEdge.first, outgoingEdge.second);
            add_edge(output, srcNodeId, dstNodeId, 1.0);
        }

        return srcNodeId;
    }
}