//
// Created by giacomo on 06/07/2020.
//

#ifndef PRIMARY_MEMORY_LATTICE_TARJAN_H
#define PRIMARY_MEMORY_LATTICE_TARJAN_H


#include <yaucl/graphs/primary_memory_graph.h>
#include <stack>
#include <unordered_set>

namespace yaucl {
    namespace graphs {
        namespace algorithms {

            class tarjan {
                primary_memory_graph& graph;
                size_t index;
                std::unordered_map<size_t, size_t> vindex;
                std::unordered_map<size_t, size_t> lowlink;
                std::unordered_set<size_t> onStack;
                std::stack<size_t> S;
                std::stack<std::pair<size_t, std::unordered_map<size_t, edge_collateral>::iterator>> strongconnect_recursion;
                std::vector<adjacency_list_entry>::iterator vertices_iterator, vertices_end;
                std::vector<size_t> current;
            public:
                /**
                 * Initializes Tarjan's algorithms for the definition of the SCC  algorithm.
                 * This implementation is memory efficient, as each SCC is only produced once, and not stored in a vector.
                 *
                 * @param graph  Graph over which the algorithm is going to run
                 */
                tarjan(primary_memory_graph &graph);

                /**
                 * Resets the algorithm to the beginning of the iterator
                 */
                void reset();

                /**
                 * Checks whether there is a next element to analyse
                 * @return
                 */
                bool hasNext();

                /**
                 * If there is a next element, it returns a non-empty cycle vector. An empty vector is returned if no further SCC
                 * is possible
                 *
                 * @return The next SCC or an empty vector
                 */
                std::vector<size_t> next();

            private:

                /**
                 * Iterative implementation of the recursive implementation, the classical definition of the algorithm
                 */
                void run();

                /**
                 * Original recursive implementation of the algorithm
                 * @param u             Current node
                 * @param scc           Place where all the SCC obtained from a given node is given.
                 */
                void strongconnect(const adjacency_list_entry& u, std::vector<std::vector<size_t>>& scc);

                /**
                 * Iteration over the adjacency list of u.
                 * @param u     Node over which the adj list is iterated
                 * @param it    Current position over the iteration list (possibly interrupted in the previous step)
                 * @return It returns true if the iteration is break by a recursive call of the algorithm, that needs to be
                 * simulated via the stack.
                 */
                bool adjIteration(adjacency_list_entry &u,
                                  std::unordered_map<size_t, edge_collateral>::iterator &it) ;
            };

        }
    }
}


#endif //PRIMARY_MEMORY_LATTICE_TARJAN_H
