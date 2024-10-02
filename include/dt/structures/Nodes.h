//
// Created by giacomo on 02/10/24.
//

#ifndef DT_NODES_H
#define DT_NODES_H

#include <dt/dt_predicate.h>


struct Nodes {
    size_t begin = 0, end = 0;
    size_t rightOffset, split;
    std::pair<dt_predicate,double> candidate;

    Nodes(size_t begin, size_t end, size_t max_height);

    bool isLeaf;
    int majority_class;
    double majority_class_precision;
    size_t max_height;
};


#endif //DT_NODES_H
