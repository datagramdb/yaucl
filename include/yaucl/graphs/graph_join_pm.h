//
// Created by giacomo on 16/07/21.
//

#ifndef AUTOMATA_JOIN_GRAPH_JOIN_PM_H
#define AUTOMATA_JOIN_GRAPH_JOIN_PM_H

#include <yaucl/numeric/ssize_t.h>
#include <map>
#include <vector>
#include <string>
#include <yaucl/graphs/adjacency_entry.h>
#include <yaucl/hashing/uset_hash.h>
#include <unordered_set>

/**
 * Definition of a graph automaton as an efficient representation to be combined with other graphs, too!
 */
struct graph_join_pm {
    ssize_t V_size, E_size;
    std::map<std::string, std::vector<adjacency_entry>> vertex_hash_primary_index;
    std::vector<std::pair<std::string, size_t>> vertex_id_secondary_index;
    std::unordered_set<size_t> starting_point;
    bool doesAcceptEmptyString;
    size_t size() const;

    graph_join_pm();
    virtual ~graph_join_pm() {
        vertex_hash_primary_index.clear();
        vertex_id_secondary_index.clear();
        starting_point.clear();
    };
    graph_join_pm(const graph_join_pm&) = default;
    graph_join_pm& operator=(const graph_join_pm&) = default;
    graph_join_pm(graph_join_pm&&) = default;
    graph_join_pm& operator=(graph_join_pm&&) = default;

    friend std::ostream &operator<<(std::ostream &os, const graph_join_pm &pm);
};

/**
 * Adds a node inside a graph
 */
size_t add_node(graph_join_pm& graph, const std::string& node_label, bool is_starting = false, bool is_final = false);

/**
 * Resolves the node inside a grpah by its id
 */
adjacency_entry& resolve_node(graph_join_pm& g, size_t id);
const adjacency_entry &resolve_node_const(const graph_join_pm &g, size_t id);
void add_edge(graph_join_pm& g, adjacency_entry& ref_src, size_t dst, double edge_weight);
void add_edge(graph_join_pm& g, size_t src, size_t dst, double edge_weight);
void dot(struct graph_join_pm& graph, std::ostream& os, bool ignoreEdgeLabels = false);

void do_heavy_debug(struct graph_join_pm& graph, size_t id);
void do_heavy_debug(struct graph_join_pm& graph);

#include <unordered_map>

graph_join_pm replace_labels(const graph_join_pm &g, const std::unordered_map<std::string, std::string>& VM);

#include <utility>
//#include <yaucl/strings/serializer.h>
#include <yaucl/structures/fixed_bimap.h>
#include <unordered_map>
#include <yaucl/graphs/graph_join_pm.h>



/**
 * Adds the graph join node
 * @param src
 * @param dst
 * @param max_id
 * @param map
 * @return
 */
size_t node_resolve(size_t src, size_t dst, size_t& max_id, yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t>& map,
                    graph_join_pm& graph);


void graph_join_intermediate(graph_join_pm &left, graph_join_pm &right,
                             size_t& id,
                             graph_join_pm& result,
                             yaucl::structures::fixed_bimap<std::pair<size_t, size_t>, size_t>& vertex_map);

graph_join_pm graph_join(graph_join_pm& left, graph_join_pm& right);


/**
 * Providing an approximated topological sort, where the loops are ignored
 *
 * @param g                 Graph to be approximately visited
 * @param initial_node      The only graph node having no ingoing edges
 * @return vec               output: The vector containing the topologically sorted vertices
 */
void approximated_topo_sort(graph_join_pm& g, std::vector<size_t>& order);


/**
 * Given a graph, it clusters the node by label and by similar actions. This computes the preliminary step for DAG
 * minimization.
 *
 * @param graph                         Input: graph to be clustered over nodes
 * @param clustersPerClusteredNode      Ouput: node clusters per clustered node
 */
void getNodeClustersFromLabel(graph_join_pm &graph,
                              std::unordered_map<size_t, std::unordered_set<size_t>> &clustersPerClusteredNode);

/**
 * Given a node-labelled DFA, it provides its minimization
 * @param input
 * @param output
 */
void minimize(graph_join_pm& input, graph_join_pm& output);

/**
 * Reverts an automaton, by inverting the edges' direction and starting the visit from the final nodes and ending
 * it to the final ones
 *
 * @param input
 * @param output        Resulting inverted graph
 */
void invert_graph(graph_join_pm& input, graph_join_pm& output);

/**
 * This algorithm provides the preliminary computation required to remove the nodes that are never leading to an acceptance
 *
 * @param input                 Input: graph to test
 * @param forward_visited       Output: whether a vertex is reachable from an initial state
 * @param backward_visited      Output: whether a vertex can reach an acceptance state
 */

void getUnreachableNodesForAcceptance(graph_join_pm &input,
                                      std::vector<bool> &forward_visited,
                                      std::vector<bool> &backward_visited);
/**
 * This function removes all the nodes that are both unreachable from the initial nodes, and that cannot possibly lead
 * to accepting states. This is the ideal graph for determining the inclusion over the observable states, as it removes
 * the behaviour that cannot be possibly captured by any possible edge
 *
 * @param input         Input graph
 * @param output        Pruned graph
 */
void remove_unaccepting_states(graph_join_pm& input, graph_join_pm& output);


/**
 * Generates a deterministic graph
 */
struct make_graph_deterministic {
    std::unordered_map<std::unordered_set<size_t>, size_t> multiNodesToNewNode;

    void generate_out_graph(graph_join_pm &input,
                            graph_join_pm &output);

private:
    size_t generate_out_graph(graph_join_pm &input,
                              graph_join_pm &output,
                              const std::string& label,
                              const std::unordered_set<size_t>& I);
};


#endif //AUTOMATA_JOIN_AUTOMATA_JOIN_H
