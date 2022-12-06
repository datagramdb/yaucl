//
// Created by giacomo on 07/07/2020.
//

#ifndef PRIMARY_MEMORY_LATTICE_NESTING_H
#define PRIMARY_MEMORY_LATTICE_NESTING_H

#include <yaucl/graphs/primary_memory_graph.h>

namespace yaucl {
    namespace graphs {
        namespace algorithms {
            size_t graph_nesting(primary_memory_graph &graph, std::vector<size_t> &cluster, size_t root);
        }
    }
}


#endif //PRIMARY_MEMORY_LATTICE_NESTING_H
