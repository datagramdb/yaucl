//
// Created by giacomo on 11/12/22.
//

#include <utility>
#include <functional>
#include <queue>
#include "yaucl/graphs/algorithms/prim.h"

std::vector<ssize_t> primMST(size_t src,
                             const adjacency_graph& ag,
                             const std::vector<double>& edgeId_to_weight)
{
    /// https://www.geeksforgeeks.org/prims-algorithm-using-priority_queue-stl/
    // Create a priority queue to store vertices that
    // are being primMST.
    // Refer below link for details of this syntax
    // http://geeksquiz.com/implement-min-heap-using-stl/
    std::priority_queue< std::pair<double, size_t>,
            std::vector <std::pair<double, size_t>> ,
            std::greater<std::pair<double, size_t>> > pq;

    auto V = ag.V_size;
    auto INF = std::numeric_limits<double>::max();
    // Create a vector for keys and initialize all
    // keys as infinite (INF)
    std::vector<double> key(V, INF);

    // To store parent array which in turn store MST
    std::vector<ssize_t> parent(V, -1);

    if (src >= ag.V_size) return parent;

    // To keep track of vertices included in MST
    std::vector<bool> inMST(V, false);

    // Insert source itself in priority queue and initialize
    // its key as 0.
    pq.push({0.0, src});
    key[src] = 0;

    /* Looping till priority queue becomes empty */
    while (!pq.empty())
    {
        // The first vertex in pair is the minimum key
        // vertex, extract it from priority queue.
        // vertex label is stored in second of pair (it
        // has to be done this way to keep the vertices
        // sorted key (key must be first item
        // in pair)
        int u = pq.top().second;
        pq.pop();

        //Different key values for same vertex may exist in the priority queue.
        //The one with the least key value is always processed first.
        //Therefore, ignore the rest.
        if(inMST[u]){ continue; }

        inMST[u] = true;  // Include vertex in MST

        // 'i' is used to get all adjacent vertices of a vertex
//        list< pair<int, int> >::iterator i;

        for (auto edgeId : ag.nodes.at(u)) {
            auto v = ag.edge_ids.at(edgeId).second;
            double weight = edgeId_to_weight.at(edgeId);
            // Get vertex label and weight of current adjacent
            // of u.

            //  If v is not in MST and weight of (u,v) is smaller
            // than current key of v
            if ((!inMST[v]) && key[v] > weight) {
                // Updating key of v
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    return parent;
}

const std::vector<ssize_t> &
PrimMST::primMST(const std::vector<double> &edgeId_to_weight, const std::function<bool(size_t)> &edgeIdToIsVisit) {
    /* Looping till priority queue becomes empty */
    while (!pq.empty())
    {
        // The first vertex in pair is the minimum key
        // vertex, extract it from priority queue.
        // vertex label is stored in second of pair (it
        // has to be done this way to keep the vertices
        // sorted key (key must be first item
        // in pair)
        int u = pq.top().second;
        pq.pop();

        //Different key values for same vertex may exist in the priority queue.
        //The one with the least key value is always processed first.
        //Therefore, ignore the rest.
        if(inMST[u]){ continue; }

        inMST[u] = true;  // Include vertex in MST

        // 'i' is used to get all adjacent vertices of a vertex
//        list< pair<int, int> >::iterator i;

        for (auto edgeId : ag.nodes.at(u)) {
            if (!edgeIdToIsVisit(edgeId)) continue;
            auto v = ag.edge_ids.at(edgeId).second;
            double weight = edgeId_to_weight.at(edgeId);
            // Get vertex label and weight of current adjacent
            // of u.

            //  If v is not in MST and weight of (u,v) is smaller
            // than current key of v
            if ((!inMST[v]) && key[v] > weight) {
                // Updating key of v
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }
    return parent;
}

const std::vector<ssize_t> &PrimMST::primMST(size_t src, const std::vector<double> &edgeId_to_weight,
                                             const std::function<bool(size_t)> &edgeIdToIsVisit) {
    if (src >= ag.V_size) return parent;
    pq.push({0.0, src});
    key[src] = 0;
    return primMST(edgeId_to_weight, edgeIdToIsVisit);
}
