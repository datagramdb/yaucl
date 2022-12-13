//
// Created by giacomo on 13/12/22.
//

#ifndef AIRLINE_CONNECTED_COMPONENTS_H
#define AIRLINE_CONNECTED_COMPONENTS_H

#include <yaucl/graphs/adjacency_graph.h>

void connected_components(const adjacency_graph& input, std::vector<roaring::Roaring64Map>& result);


#endif //AIRLINE_CONNECTED_COMPONENTS_H
