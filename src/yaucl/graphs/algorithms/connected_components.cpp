//
// Created by giacomo on 13/12/22.
//

#include "yaucl/graphs/algorithms/connected_components.h"

ssize_t connected_components(const adjacency_graph &input,
                           const std::vector<roaring::Roaring64Map>& starting,
                           std::vector<roaring::Roaring64Map> &result) {
    ssize_t missing_starting_from = -1;
    std::vector<ssize_t> vertex_id_to_connected_component(input.V_size, -1);

    roaring::Roaring64Map visitedGlobal;
    for (const auto& set : starting) {
        roaring::Roaring64Map visited;
        assert(!set.isEmpty());
        for (size_t src : set) {
            if ((!visited.contains(src)) && (!visitedGlobal.contains(src))) {
                vertex_id_to_connected_component[src] = result.size();
                adjacency_graph_DFSUtil(src, input, visited);
            }
        }
        if (visited.isEmpty()) {
            // If that is empty, it means that the nodes were previously visited: associating those
            // to the first connected component so far
            result.emplace_back(result.at(
                    vertex_id_to_connected_component.at(*set.begin())));
        } else {
            for (size_t x : visited)
                vertex_id_to_connected_component[x] = result.size();
            result.emplace_back(visited);
            visitedGlobal |= visited;
        }
    }
    roaring::Roaring64Map remaining;
    remaining.addRange(0, input.V_size);
    remaining -= visitedGlobal;
    if (!remaining.isEmpty()) {
        missing_starting_from = (ssize_t)result.size();
        for (size_t src : remaining) {
            if (!visitedGlobal.contains(src)) {
                adjacency_graph_DFSUtil(src, input, visitedGlobal);
                result.emplace_back(visitedGlobal);
            }
        }
    }
    if ((!result.empty()) && (!remaining.isEmpty())) {
        for (size_t nm1 = result.size()-1; nm1>=missing_starting_from; nm1--) {
            result[nm1] -= result[nm1-1];
        }
    }
    return missing_starting_from;
}

ssize_t connected_components(const adjacency_graph &input,
                             const roaring::Roaring64Map& starting,
                             std::vector<roaring::Roaring64Map> &result) {
    roaring::Roaring64Map visited;
    ssize_t missing_starting_from;
    missing_starting_from = -1;
    for (size_t src : starting) {
        if (!visited.contains(src)) {
            adjacency_graph_DFSUtil(src, input, visited);
            result.emplace_back(visited);
        }
    }
    roaring::Roaring64Map remaining;
    remaining.addRange(0, input.V_size);
    remaining -= visited;
    if (!remaining.isEmpty()) {
        missing_starting_from = (ssize_t)result.size();
        for (size_t src : remaining) {
            if (!visited.contains(src)) {
                adjacency_graph_DFSUtil(src, input, visited);
                result.emplace_back(visited);
            }
        }
    }
    if (!result.empty()) {
        for (size_t nm1 = result.size()-1; nm1>0; nm1--) {
            result[nm1] -= result[nm1-1];
        }
    }
    if (missing_starting_from == -1) {
        assert(starting.cardinality() == result.size());
    } else {
        assert(starting.cardinality() < result.size());
    }
    return missing_starting_from;
}

void connected_components(const adjacency_graph &input,
                          std::vector<roaring::Roaring64Map> &result,
                          roaring::Roaring64Map& visited) {
    result.clear();
    roaring::Roaring64Map orig = visited;
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
        result[0] -= orig;
    }
}

void connected_components(const adjacency_graph &input,
                          std::vector<roaring::Roaring64Map> &result) {
    roaring::Roaring64Map visited;
    connected_components(input, result, visited);
}

//#include <yaucl/strings/serializers.h>
//#include <yaucl/structures/RoaringBitmapWrapper.h>
//#include <iostream>

void connected_components_with_edge_prop(const adjacency_graph &input,
                          std::vector<roaring::Roaring64Map> &result,
                          roaring::Roaring64Map& visited,
                          const std::function<bool(size_t)>& edgeProp) {
    result.clear();
    roaring::Roaring64Map orig = visited;
    for (size_t u = 0, N = input.V_size; u<N; u++) {
        if (!visited.contains(u)) {
            adjacency_graph_DFSUtil_with_edge_prop(u, input, visited);
            result.emplace_back(visited);
        }
    }
    if (!result.empty()) {
        for (size_t nm1 = result.size()-1; nm1>0; nm1--) {
            result[nm1] -= result[nm1-1];
        }
//        RoaringBitmapWrapper wrap{result[0]};
//        RoaringBitmapWrapper wrapOrig{orig};
//        std::cout << wrap.asVector() << " vs. " << wrapOrig.asVector() << std::endl;
        result[0] -= orig;
    }
}



