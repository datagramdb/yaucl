//
// Created by giacomo on 26/07/21.
//

#ifndef BPM21_GRAPH_JOIN_PM_ALGORITHMS_H
#define BPM21_GRAPH_JOIN_PM_ALGORITHMS_H

#include <yaucl/graphs/graph_join_pm.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

#endif //BPM21_GRAPH_JOIN_PM_ALGORITHMS_H
