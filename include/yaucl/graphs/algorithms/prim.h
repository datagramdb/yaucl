//
// Created by giacomo on 11/12/22.
//

#ifndef AIRLINE_PRIM_H
#define AIRLINE_PRIM_H

#include <vector>
#include "yaucl/graphs/adjacency_graph.h"


std::vector<ssize_t> primMST(size_t src,
                             const adjacency_graph& ag,
                             const std::vector<double>& edgeId_to_weight);


#endif //AIRLINE_PRIM_H
