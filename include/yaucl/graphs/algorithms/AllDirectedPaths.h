//
// Created by giacomo on 29/11/22.
//

#ifndef YAUCL_GRAPH_ALLDIRECTEDPATHS_H
#define YAUCL_GRAPH_ALLDIRECTEDPATHS_H

#include <roaring64map.hh>
#include <yaucl/graphs/adjacency_graph.h>
#include <yaucl/design_patterns/Observer.h>
#include <vector>

struct AllDirectedPaths {
    roaring::Roaring64Map visited;
    const struct adjacency_graph& graph;
    IObservee<std::pair<std::vector<size_t>, size_t>> Observee;
    // Create an array to store paths
    std::vector<size_t> path;

    AllDirectedPaths(const adjacency_graph &graph) : graph(graph), path(graph.V_size, 0) {}

    void attachObserver(const std::function<void(std::pair<std::vector<size_t>, size_t>)>& f) {
        Observee.Attach(f);
    }

    void clearObserver() {
        Observee.DetachAll();
    }

    void getAllPaths(size_t source, size_t destination, bool any = false) {
        printAllPaths(source, {destination}, any);
    }

    void getAllPaths(size_t source, const std::unordered_set<size_t>& destination, bool any = false) {
        printAllPaths(source, destination, any);
    }

    void getAllPaths(const std::unordered_set<size_t>& source, const std::unordered_set<size_t>& destination, bool any = false) {
        for (const size_t s : source) {
            visited.clear();
            printAllPaths(s, destination, any);
        }
    }

private:

    void printAllPaths(size_t s, const std::unordered_set<size_t>& d, bool getFirst = false) {
        // Mark all the vertices as not visited
        visited.clear();
        // Call the recursive helper function to print all paths
        printAllPathsUtil(s, d,  getFirst);
    }

    void printAllPathsUtil(size_t src,
                           const std::unordered_set<size_t>& d,
                           bool getFirst = false) {
        visited.clear();
        std::vector<std::pair<size_t, size_t>> nextElements{{src, 0}};
        while (!nextElements.empty()) {
            auto& cp = nextElements.back();
            size_t u = cp.first;
            size_t path_index = cp.second;
            if (visited.contains(u)) {
                visited.add(u);
                if (d.contains(u)) {
                    Observee.Notify(std::make_pair(path, path_index));
                    if (getFirst) return;
                }
                nextElements.pop_back();
                continue;
            }

            visited.add(u);
            // Mark the current node and store it in path[]
            path[path_index] = u;
            path_index++;
            if (d.contains(u)) {
                Observee.Notify(std::make_pair(path, path_index));
                if (getFirst) return;
                nextElements.pop_back();
            } else {
                // Recur for all the vertices adjacent to current vertex
                bool insertion = false;
                for (const size_t edge : graph.nodes.at(u)) {
                    size_t i = graph.edge_ids.at(edge).second;
                    if (!visited.contains(i)) {
                        nextElements.emplace_back(i, path_index+1);
                        insertion = true;
                    }
                }
                if (!insertion) {
                    nextElements.pop_back();
                }
            }
        }
    }
};


#endif //YAUCL_GRAPH_ALLDIRECTEDPATHS_H
