//
// Created by giacomo on 13/12/22.
//

#ifndef AIRLINE_CONNECTED_COMPONENTS_H
#define AIRLINE_CONNECTED_COMPONENTS_H

#include <yaucl/graphs/adjacency_graph.h>



/**
 * Running the classical connected component algorithms
 * @param input         Input graph
 * @param result        Set of distinct connected components
 */
void connected_components(const adjacency_graph& input,
                          std::vector<roaring::Roaring64Map>& result);

/**
 * Running the connected components algorithm starting from some elected initial points.
 * The algorithm will remark the connected components being obtained from these out from
 * the ones that were mined out of these.
 *
 * @param input         The graph over which run the connected components algorithm
 * @param starting      The set of starting points from which get the initial connected components
 * @param result        The set of the distinct connected components
 * @return              This is set to -1 if all of the starting points completely cover the whole graph.
 *                      If the starting points still leave out some nodes as un-visited, we then return
 *                      the index within the results from which we get the connected components associated
 *                      to the remaining nodes
 */
ssize_t connected_components(const adjacency_graph &input,
                             const roaring::Roaring64Map& starting,
                             std::vector<roaring::Roaring64Map> &result);

/**
 * Differently from the two other algorithms given above, this assumes to have separated starting points.
 * The results are going to be associated to each element of the starting point. The ssize_t result from the
 * previous setting can be ignored if the set of starting nodes is no overlapping and if this belongs to different
 * connected components, and therefore the number of the results matches with the number of starting points,
 * (as overlapping connected components might be returned for each associated node). The extra results being
 * provided refer to nodes not considered in the starting set of elements
 *
 * @param input         graph
 * @param starting      Set of set of starting points
 * @param result        Results for connected components associated to each set of starting points, as well as defining the extra CC ones
 * @return
 */
ssize_t connected_components(const adjacency_graph &input,
                             const std::vector<roaring::Roaring64Map>& starting,
                             std::vector<roaring::Roaring64Map> &result);

#endif //AIRLINE_CONNECTED_COMPONENTS_H
