//
// Created by giacomo on 16/07/21.
//
#include <yaucl/strings/serializers.h>
#include "yaucl/graphs/graph_join_pm.h"
#include <yaucl/functional/assert.h>

graph_join_pm::graph_join_pm() : V_size{0}, E_size{0}, doesAcceptEmptyString{false} {}

std::ostream &operator<<(std::ostream &os, const graph_join_pm &pm) {
    os << "Graph(|V|=" << pm.V_size << ")" << std::endl;
    for (const auto& cp : pm.vertex_hash_primary_index) {
        os << " - Bucket: " << cp.first << std::endl;
        for (const auto& entry: cp.second) {
            os << entry << std::endl;
        }
    }
    return os;
}

size_t graph_join_pm::size() const {
    return V_size+E_size;
}

#include <cassert>

size_t add_node(graph_join_pm &graph, const std::string &node_label, bool is_starting, bool is_final) {
    size_t init_id = (graph).V_size;
    auto& elem = (graph).vertex_hash_primary_index[(node_label)];
    graph.vertex_id_secondary_index.emplace_back(node_label, elem.size());
    auto& result =  elem.emplace_back(init_id, is_starting, is_final);
    if (is_starting) {
        graph.starting_point.emplace(init_id);
    }
    (graph).V_size++;
    ///assert((graph).V_size == result.id+1);
    DEBUG_ASSERT(init_id == result.id);
    return result.id;
}

adjacency_entry &resolve_node(graph_join_pm &g, size_t id) {
    auto& ref = g.vertex_id_secondary_index.at(id);
    DEBUG_ASSERT(g.vertex_hash_primary_index.at(ref.first).at(ref.second).id == id);
    return g.vertex_hash_primary_index.at(ref.first).at(ref.second);
}

void add_edge(graph_join_pm &g, adjacency_entry &ref_src, size_t dst, double edge_weight) {
    auto& ref_dst = g.vertex_id_secondary_index.at(dst);
    (ref_src).outgoing_hash_to_outgoingNodeIdWithWeight[(ref_dst.first)].emplace_back((dst), (edge_weight));
    g.E_size++;
}

void add_edge(graph_join_pm &g, size_t src, size_t dst, double edge_weight) {
    add_edge(g, resolve_node(g, src), dst, edge_weight);
}

void dot(graph_join_pm &graph, std::ostream &os, bool ignoreEdgeLabels)  {
    os << "digraph {\nrankdir=LR;\n";
    for (size_t node_id : graph.starting_point) {
        os << "\tfake" << node_id << " [style=invisible]" << std::endl;
    }
    for (int node_id /*: getNodeIds()*/ = 0; node_id<graph.V_size; node_id++) {
        os << '\t' << node_id;
        auto& node = resolve_node(graph, node_id);
        bool hasFinal = node.is_final;
        bool hasInitial = node.is_starting;
        os << " [";
        if (hasFinal || hasInitial) {
            if (hasInitial)
                os << "root=true ";
            if (hasFinal) {
                os << "shape=doublecircle ";
            }

        }
        os << "label=\"" << graph.vertex_id_secondary_index.at(node_id).first << ":" << node_id <<"\"";
        os << "]" << std::endl;
    }
    for (size_t node_id : graph.starting_point) {
        os << "\tfake" << node_id << " -> " << node_id << " [style=bold]" << std::endl;
    }
    for (int node_id = 0; node_id<graph.V_size; node_id++) {

        auto& node = resolve_node(graph, node_id);
        for (const auto& label_and_subadj :
                node.outgoing_hash_to_outgoingNodeIdWithWeight) {
            for (const auto v : label_and_subadj.second) {
                os << '\t' << node_id << " -> " << v.first;
                if (!ignoreEdgeLabels)
                    os << " [label=" << v.first << "]";
                os << std::endl;
            }
        }
    }
    os << "}";
}

const adjacency_entry &resolve_node_const(const graph_join_pm &g, size_t id) {
    auto& ref = g.vertex_id_secondary_index.at(id);
    return g.vertex_hash_primary_index.at(ref.first).at(ref.second);
}

#include <iostream>

void do_heavy_debug(graph_join_pm &graph, size_t id) {
    static size_t jjjj = 0;
    bool test = (graph.vertex_id_secondary_index.size() > id);
    if (!test) {
        std::cerr << "ERROR! (1) the Id is greater than the maximum id for a vertex in the current graph!" << std::endl;
        test = false;
    }
    std::string label;
    size_t offset = 0;
    bool found = false;
    jjjj++;
    if ((jjjj) == 68638)
        std::cerr << "BREAK" << std::endl;
    for (const auto& ref : graph.vertex_hash_primary_index) {
        if (found) break;
        for (size_t i = 0, N = ref.second.size(); i<N; i++) {
            const auto& v_out = ref.second.at(i);
            if (v_out.id == id) {
                ///std::cout << id << " Found @" << ref.first << " element #" << i << std::endl;
                label = ref.first;
                offset = i;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        std::cerr << " ERROR: id " << id << " was not found! " << std::endl;
        DEBUG_ASSERT(found);
    }
    if (test) {
        auto& ref = graph.vertex_id_secondary_index.at(id);
        if (ref.first != label) {
            std::cerr << "ERROR (2): node at position " << id << " has not the expected label of '" << label << "', but rather the following: " << ref.first << std::endl;
            test = false;
        }
        if (!graph.vertex_hash_primary_index.contains(ref.first)) {
            std::cerr << "ERROR (3): label addressed by node with id " << id << ", i.e.," << ref.first << ", is not in the graph!" << std::endl;
            test = false;
        } else {
            auto&    ref2 = graph.vertex_hash_primary_index.at(label);
            if (ref.first != label) {
                ref2 = graph.vertex_hash_primary_index.at(ref.first);
                if (ref2.size() <= offset) {
                    std::cerr << "ERROR (4bis): the offset " << offset << " for retrieved label " << ref.first << " is not there, as there are less elements!" << std::endl;
                    test = false;
                } else if (ref2.at(offset).id != id) {
                    std::cerr << "ERROR (5bis): the offset " << offset << " for retrieved label " << ref.first << " is there, but pointing to a different id, " << ref2.at(offset).id << std::endl;
                    test = false;
                }
            }
            if (ref2.size() <= offset) {
                std::cerr << "ERROR (4): the offset " << offset << " for expected label " << label << " is not there, as there are less elements!" << std::endl;
                test = false;
            } else if (ref2.at(offset).id != id) {
                std::cerr << "ERROR (5): the offset " << offset << " for expected label " << label << " is there, but pointing to a different id, " << ref2.at(offset).id << std::endl;
                test = false;
            }
        }
    }
    DEBUG_ASSERT(test);
}

void do_heavy_debug(graph_join_pm &graph) {
    DEBUG_ASSERT(graph.V_size == graph.vertex_id_secondary_index.size());
    ///std::cout << "{[";
    for (size_t i = 0, N = graph.V_size; i<N; i++) {
        ///std::cout << i << ", ";
        do_heavy_debug(graph, i);
    }
    ///std::cout << "]}" << std::endl;
}

//
// Created by giacomo on 16/07/21.
//

#include <cassert>
#include <yaucl/functional/iterators.h>
#include <yaucl/hashing/pair_hash.h>
#include <unordered_set>
#include <stack>

///size_t iii = 0;

size_t node_resolve(size_t src, size_t dst,
                    const std::string& label,
                    bool starting,
                    bool final,
        //size_t &max_id,
                    yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t> &map,
                    graph_join_pm &graph) {
    std::pair<size_t, size_t> cp = {src, dst};
    size_t id;
    auto opt = map.hasKey(cp);
    if (opt) {
        id = opt.value()->second;
#ifdef DEBUG
        std::cout << "<" << src << ", " << dst << "> => " << id << std::endl;
#endif
    } else {
        id = add_node(graph, label, starting, final);
#ifdef DEBUG
        std::cout << "<" << src << ", " << dst << "> := " << id << std::endl;
#endif
        map.put(cp, id);
    }
    return id;
}

/**
 * Performs the graph join at the adjacency list level!
 */
size_t
outgoing_join(const std::string &label,
              const adjacency_entry &left,
              const adjacency_entry &right,
              size_t &max_id,
              yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t> &map,
              graph_join_pm &graph) {
    size_t node_id;
    std::map<std::string, std::vector<std::pair<size_t, double>>> outgoing_hash_to_outgoingNodeIdWithWeight;

    {
        bool starting_local = left.is_starting && right.is_starting;
        bool final_local = left.is_final && right.is_final;
        node_id = node_resolve(left.id, right.id, label, starting_local, final_local, map, graph);
#ifdef DEBUG
        do_heavy_debug(graph);
#endif
        // By doing so, I can extend the primary index for the result with the element that has been computed!
        auto& result = resolve_node(graph, node_id);
#ifdef DEBUG
        do_heavy_debug(graph);
#endif
        DEBUG_ASSERT(result.id == node_id);
        if (result.is_starting)
            DEBUG_ASSERT(left.is_starting && right.is_starting);
        else {
            result.is_starting = left.is_starting && right.is_starting;
            if (result.is_starting)
                graph.starting_point.emplace(node_id);
        }
        result.is_final = left.is_final && right.is_final;
#ifdef DEBUG
        std::cout << '<' << left.id << ", " << right.id << "> = " << node_id << " ~~ " << result.id << std::endl;
#endif

        auto it_left = left.outgoing_hash_to_outgoingNodeIdWithWeight.begin(), e_left = left.outgoing_hash_to_outgoingNodeIdWithWeight.end();
        auto it_right = right.outgoing_hash_to_outgoingNodeIdWithWeight.begin(), e_right = right.outgoing_hash_to_outgoingNodeIdWithWeight.end();

        yaucl::iterators::map_iterate_key_intersection(it_left, e_left,
                                                       it_right, e_right,
                                                       [&](auto it_left, auto it_right) {
                                                           auto& ref = outgoing_hash_to_outgoingNodeIdWithWeight[it_left->first];
                                                           for (const auto& u : it_left->second) {
                                                               for (const auto& v: it_right->second) {
                                                                   size_t frst_node = node_resolve(u.first, v.first, it_left->first, false, false, map, graph);
#ifdef DEBUG
                                                                   std::cout << "--[" << it_left->first << " = ]-> <" << u.first << ", " << v.first << "> = " << frst_node << " ~~??[To Determine] " << std::endl;
#endif
                                                                   graph.E_size++;
                                                                   ref.emplace_back(frst_node, u.second * v.second);
                                                               }
                                                           }
                                                       });
    }


    auto& result = resolve_node(graph, node_id);
    DEBUG_ASSERT(result.outgoing_hash_to_outgoingNodeIdWithWeight.empty());
    std::swap(result.outgoing_hash_to_outgoingNodeIdWithWeight, outgoing_hash_to_outgoingNodeIdWithWeight);
    return result.id;
}

void graph_join_intermediate(graph_join_pm &left, graph_join_pm &right,
                             size_t& id,
                             graph_join_pm& result,
                             yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t>& vertex_map) {

    std::unordered_set<std::pair<size_t, size_t>> visited;
    std::map<std::string, std::vector<size_t>> left_starting;
    std::map<std::string, std::vector<size_t>> right_starting;
    for (size_t starting : left.starting_point) {
        auto& ref = left.vertex_id_secondary_index.at(starting);
        left_starting[ref.first].emplace_back(starting);
    }
    for (size_t starting : right.starting_point) {
        auto& ref = right.vertex_id_secondary_index.at(starting);
        right_starting[ref.first].emplace_back(starting);
    }
#ifdef DEBUG
    for (const auto& cp : left_starting) {
        std::cout << cp.first << " --(1)-> {";
        for (const auto& elem : cp.second)
            std::cout << elem<< ", ";
        std::cout <<  "}" << std::endl;
    }
    for (const auto& cp : right_starting) {
        std::cout << cp.first << " --(2)-> {";
        for (const auto& elem : cp.second)
            std::cout << elem << ", ";
        std::cout <<  "}" << std::endl;
    }
#endif
    auto it_left = left_starting.begin(), e_left = left_starting.end();
    auto it_right = right_starting.begin(), e_right = right_starting.end();
    std::unordered_set<size_t> starting_nodes;
    std::stack<std::pair<size_t, size_t>> stack;
#ifdef DEBUG
    std::cout << "Entry Point  ~~~~~~~~~~~~~~~~" << std::endl;
#endif
    yaucl::iterators::map_iterate_key_intersection(left_starting.begin(), left_starting.end(),
                                                   right_starting.begin(), right_starting.end(),
                                                   [&](auto itL, auto itR) {
                                                       const std::string& label = itL->first;
#ifdef DEBUG
                                                       std::cout << label << std::endl;
                                                       if (label == "leucocytes" )
                                                           std::cerr<< "HERE: "  <<std::endl;
#endif
                                                       for (size_t L : itL->second) {
                                                           const auto Lnode = resolve_node(left, L);
                                                           for (size_t R : itR->second) {
#ifdef DEBUG
                                                               std::cout << "LR = " << L << " & " << R << std::endl;
#endif
                                                               // Setting the node id
                                                               size_t ref2_id = (outgoing_join(label, Lnode, resolve_node(right, R), id, vertex_map,result));
                                                               auto& ref2 = resolve_node(result, ref2_id);
                                                               // Mark the starting nodes as already visited!
                                                               visited.emplace(L,R);

                                                               // Still, I will need to continue the visit from the outgoing nodes!
                                                               for (const auto& cp : ref2.outgoing_hash_to_outgoingNodeIdWithWeight) {
                                                                   for (size_t i = 0, N = cp.second.size(); i<N; i++) {
                                                                       auto opt = vertex_map.hasValue(cp.second.at(i).first);
                                                                       DEBUG_ASSERT(opt);
                                                                       std::pair<size_t, size_t> cp2 = opt.value()->first;
                                                                       stack.push(cp2);
                                                                   }
                                                               }
                                                           }
                                                       }
                                                   });

    // After the node insertion from the initial nodes, the nodes from which we need to start the visit from are the
    // outgoing nodes
#ifdef DEBUG
    std::cout << "Stack Going ~~~~~~~~~~~~~~~~" << std::endl;
#endif
    while (!stack.empty()) {
        // Pop a vertex from stack and print it
        auto s = stack.top();
        stack.pop();

        // Stack may contain same vertex twice. So
        // we need to print the popped item only
        // if it is not visited.
        if (!visited.contains(s)) {
            visited.insert(s);
            // Inferring the lable from the left operand information
            const std::string& label = left.vertex_id_secondary_index.at(s.first).first;
            size_t ref2_id = outgoing_join(label,
                                           resolve_node(left,  s.first),
                                           resolve_node(right, s.second),
                                           id,
                                           vertex_map,
                                           result);
#ifdef DEBUG
            do_heavy_debug(result);
#endif
            auto& ref2 = resolve_node(result, ref2_id);
            // Mark the starting nodes as already visited!
            visited.insert(s);
            // Still, I will need to continue the visit from the outgoing nodes!
            for (const auto& cp : ref2.outgoing_hash_to_outgoingNodeIdWithWeight) {
                for (size_t i = 0, N = cp.second.size(); i<N; i++) {
                    auto opt = vertex_map.hasValue(cp.second.at(i).first);
                    DEBUG_ASSERT(opt);
                    std::pair<size_t, size_t> cp2 = opt.value()->first;
#ifdef DEBUG
                    std::cout << " In Stack: " << cp.second.at(i).first << " as <"<< cp2.first << ", " << cp2.second << ">" << std::endl;
#endif
                    stack.push(cp2);
                }
            }
        }
    }


}

graph_join_pm graph_join(graph_join_pm &left, graph_join_pm &right) {
    size_t id = 0;
    graph_join_pm result;
    yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t> vertex_map;

    graph_join_intermediate(left, right, id, result, vertex_map);

    result.doesAcceptEmptyString = left.doesAcceptEmptyString && right.doesAcceptEmptyString;
    return result;
}

graph_join_pm replace_labels(const graph_join_pm &g, const std::unordered_map<std::string, std::string> &VM) {
    graph_join_pm elementi;
    for (const auto&cp :VM) {
        std::cout << cp.first << std::endl;
        elementi.vertex_hash_primary_index[cp.second] = g.vertex_hash_primary_index.at(cp.first);
    }
    std::cout << std::endl << std::endl;
    for (const auto& cp : g.vertex_id_secondary_index) {
        std::cout << cp.first << std::endl;
        elementi.vertex_id_secondary_index.emplace_back(VM.at(cp.first), cp.second);
    }
    elementi.starting_point = g.starting_point;
    elementi.V_size = g.V_size;
    elementi.E_size = g.E_size;
}
