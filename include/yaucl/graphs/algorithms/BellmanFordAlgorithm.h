//
// Created by giacomo on 30/11/22.
//

#ifndef INFERENCE_ENGINE_BELLMANFORDALGORITHM_H
#define INFERENCE_ENGINE_BELLMANFORDALGORITHM_H

#include <yaucl/graphs/adjacency_graph.h>
#include <limits>
#include <vector>
#include <algorithm>
#include <yaucl/graphs/FlexibleFA.h>


template <typename NodeLabel, typename EdgeLabel>
struct BellmanFordAlgorithm {
    static constexpr double INF = std::numeric_limits<double>::max();
    const FlexibleFA<NodeLabel, EdgeLabel>& graph;
    std::vector<double>& edge_id_cost;
    BellmanFordAlgorithm(const FlexibleFA<NodeLabel, EdgeLabel> &graph, std::vector<double> &edgeIdCost) : graph(
            graph), edge_id_cost(edgeIdCost) {}

    struct BellmanFordAlgorithmResult {
        std::vector<double> d;
        std::vector<ssize_t> p;
        ssize_t src;
        std::unordered_map<size_t, std::vector<size_t>> dst_to_path;

        BellmanFordAlgorithmResult() { src = -1; }
        BellmanFordAlgorithmResult(size_t src,
                                   const BellmanFordAlgorithm *algo) : d(algo->graph.maximumNodeId(), INF), p(algo->graph.maximumNodeId(), -1), src(src) {
            d[src] = 0;
            for (;;) {
                bool any = false;
                for (size_t j : algo->graph.getEdgeIds()) {
                    const auto& cp = algo->graph.srcDstEdge(j);
                    if (d.at(cp.first) < INF){
                        double& ref = d[cp.second];
                        if (ref > d.at(cp.first) + algo->edge_id_cost.at(j)) {
                            ref = d.at(cp.first) + algo->edge_id_cost.at(j);
                            p[cp.second] = (size_t)cp.first;
                            any = true;
                        }
                    }
                }
                if (!any)  break;
            }
        }
        BellmanFordAlgorithmResult(const BellmanFordAlgorithmResult&) = default;
        BellmanFordAlgorithmResult(BellmanFordAlgorithmResult&&) = default;
        BellmanFordAlgorithmResult& operator=(const BellmanFordAlgorithmResult&) = default;
        BellmanFordAlgorithmResult& operator=(BellmanFordAlgorithmResult&&) = default;
        const std::vector<size_t>& pathTo(size_t t) {
            auto it = dst_to_path.find(t);
            if (it != dst_to_path.end()) return it->second;
            if (d[t] == INF) {
                // No path from src to t!
                dst_to_path[t] = {};
            } else {
                std::vector<size_t> path;
                for (auto cur = (ssize_t)t; cur != -1; cur = p[cur]) {
                    path.push_back(cur);
                }
                std::reverse (path.begin(), path.end());
                dst_to_path[t] = path;
            }
            return dst_to_path.at(t);
        }

    };

    const std::vector<size_t> noPath;
    std::unordered_map<size_t, BellmanFordAlgorithmResult> src_to_results;

    /**
     * This function computes the paths from source and destination using the Bellman-Ford Algorithm, where previously
     * computed results are cached for later use.
     *
     * @param src       The source of the path
     * @param dst       The tatget of the path
     * @return The path is empty if there exists no path between source and destination, and empty otherwise
     */
    const std::vector<size_t>& getAllPathsFromSource(size_t v, size_t t) {
        if (v >= graph.maximumNodeId()) return noPath;
        BellmanFordAlgorithmResult& ref = src_to_results[v];
        if (ref.src == -1) { // if there was no initialization, and therefore there was no running of the algorithm
            ref = {v, (const BellmanFordAlgorithm*)this};
        }
        return ref.pathTo(t);
    }
};


#endif //INFERENCE_ENGINE_BELLMANFORDALGORITHM_H
