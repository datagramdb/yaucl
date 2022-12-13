//
// Created by giacomo on 13/12/22.
//

#include "yaucl/graphs/algorithms/connected_components.h"

void connected_components(const adjacency_graph &input, std::vector<roaring::Roaring64Map> &result) {
    roaring::Roaring64Map visited;
    for (size_t u = 0, N = input.V_size; u<N; u++) {
        if (!visited.contains(u)) {
            adjacency_graph_DFSUtil(u, input, visited);
            result.emplace_back(visited);
        }
    }
    if (!result.empty()) {
        for (size_t nm1 = result.size()-1; nm1>0; nm1--) {
            result[nm1] -= result[nm1-1];
        }
    }
}
