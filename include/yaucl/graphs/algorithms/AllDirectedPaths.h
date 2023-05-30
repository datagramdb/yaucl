//
// Created by giacomo on 29/11/22.
//

#ifndef YAUCL_GRAPH_ALLDIRECTEDPATHS_H
#define YAUCL_GRAPH_ALLDIRECTEDPATHS_H

#include <roaring64map.hh>
#include <yaucl/design_patterns/Observer.h>
#include <vector>
#include "yaucl/graphs/FlexibleFA.h"

template <typename NodeLabel, typename EdgeLabel>
struct AllDirectedPaths {
    roaring::Roaring64Map visited;
    const FlexibleFA<NodeLabel, EdgeLabel>& graph;
    std::function<bool(std::pair<const std::vector<size_t>*, size_t>)> pred;
    std::function<void(std::pair<const std::vector<size_t>*, size_t>)> fLoc;
    IObservee<std::pair<const std::vector<size_t>*, size_t>> Observee;
    // Create an array to store paths
    std::vector<size_t> path;
    bool doStop;

    AllDirectedPaths(const FlexibleFA<NodeLabel, EdgeLabel> &graph) : graph(graph), path(graph.g.V_size, 0), doStop{false} {}

    void attachObserver(const std::function<bool(std::pair<const std::vector<size_t>*, size_t>)>& f) {
        fLoc = [f](const auto& cp) { f(cp); };
        pred = f;
        Observee.Attach(fLoc);
    }

    void clearObserver() {
        Observee.DetachAll();
    }

    void getAllPaths(size_t source, size_t destination,  size_t multiplicity = std::numeric_limits<size_t>::max(), size_t max_size = std::numeric_limits<size_t>::max()) {
        doStop = false;
        roaring::Roaring64Map d;
        d.add(destination);
        printAllPaths(source, d, multiplicity, max_size);
    }

    void getAllPaths(size_t source, const std::unordered_set<size_t>& destination,  size_t multiplicity = std::numeric_limits<size_t>::max(), size_t max_size = std::numeric_limits<size_t>::max()) {
        doStop = false;
        roaring::Roaring64Map d;
        for (size_t x : destination) d.add(x);
        printAllPaths(source, d, multiplicity, max_size);
    }

    void getAllPathsAsEdges(size_t source,
                     const roaring::Roaring64Map& d,
                     size_t multiplicity = std::numeric_limits<size_t>::max(),
                     size_t max_size = std::numeric_limits<size_t>::max()) {
        doStop = false;
        // Mark all the vertices as not visited
        visited.clear();
        // Call the recursive helper function to print all paths
        printAllPathsEdgesUtil(source, d,  multiplicity, max_size);
    }

    void getAllPaths(size_t source,
                     const roaring::Roaring64Map& d,
                     size_t multiplicity = std::numeric_limits<size_t>::max(),
                     size_t max_size = std::numeric_limits<size_t>::max()) {
        doStop = false;
        printAllPaths(source, d, multiplicity, max_size);
    }

    void getAllPaths(const std::unordered_set<size_t>& source, const std::unordered_set<size_t>& destination,  size_t multiplicity = std::numeric_limits<size_t>::max(), size_t max_size = std::numeric_limits<size_t>::max()) {
        doStop = false;
        roaring::Roaring64Map d;
        for (size_t x : destination) d.add(x);
        for (const size_t s : source) {
            visited.clear();
            printAllPaths(s, d, multiplicity, max_size);
        }
    }

    void stop() {
        doStop = true;
    }

private:

    inline void printAllPathsEdgesUtil(size_t src,
                           const roaring::Roaring64Map& d,
                           size_t multiplicity = std::numeric_limits<size_t>::max(),
                           size_t max_size = std::numeric_limits<size_t>::max()) {
        if (multiplicity == 0) return;
        visited.clear();
        std::vector<std::tuple<size_t, ssize_t, size_t>> nextElements{{-1,-1,-1}, {src, -1, 0}};
        while ((!doStop) && (!nextElements.empty())) {
            auto& cp = nextElements.back();
            size_t u = std::get<0>(cp);
            ssize_t edge_id = std::get<1>(cp);
            size_t path_index = std::get<2>(cp);
            nextElements.pop_back();
            if (path_index == (size_t)-1) {
//                path_index--;
                visited.remove(u);
                continue;
            }

            visited.add(u);
            // Mark the current node and store it in path[]
            if (edge_id != -1) {
//#ifdef DEBUG
//                if (path_index == 0)
//                    DEBUG_ASSERT(graph.g.edge_ids.at((size_t)edge_id).first == src);
//#endif
                path[path_index] = (size_t)edge_id;
                path_index++;
            }
            std::pair<const std::vector<size_t>*, size_t> cp2 = std::make_pair(&path, path_index);
            if (path_index > max_size) {
                visited.remove(u);
            } else if (d.contains(u) && pred(cp2)) {
                Observee.Notify(cp2);
                multiplicity--;
                if (multiplicity==0) return;
                visited.remove(u);
            } else {
                // Recur for all the vertices adjacent to current vertex
                bool insertion = false;
                nextElements.emplace_back(u, -1, -1);
                for (const size_t & out_edge_id : graph.g.getOutgoingEdgesId(u)) {
                    auto &ref = graph.g.edge_from_id(out_edge_id).second;
//                    DEBUG_ASSERT( graph.g.edge_from_id(out_edge_id).first == u);
                    if ((!graph.removed_nodes.contains(ref)) && (!graph.removed_edges.contains(out_edge_id))) {
                        if (!visited.contains(ref)) {
                            nextElements.emplace_back(ref, out_edge_id, path_index);
                            insertion = true;
                        }
                    }
                }
                if (!insertion) {
                    nextElements.pop_back();
                    path_index--;
                    visited.remove(u);
                }
            }
        }
    }

    void printAllPaths(size_t s, const roaring::Roaring64Map& d, size_t multiplicity = std::numeric_limits<size_t>::max(), size_t max_size = std::numeric_limits<size_t>::max()) {
        // Mark all the vertices as not visited
        visited.clear();
        // Call the recursive helper function to print all paths
        printAllPathsUtil(s, d,  multiplicity, max_size);
    }

    void printAllPathsUtil(size_t src,
                           const roaring::Roaring64Map& d,
                           size_t multiplicity = std::numeric_limits<size_t>::max(),
                           size_t max_size = std::numeric_limits<size_t>::max()) {
        if (multiplicity == 0) return;
        visited.clear();
        std::vector<std::pair<size_t, size_t>> nextElements{{-1,-1}, {src, 0}};
        while ((!doStop) && (!nextElements.empty())) {
            auto& cp = nextElements.back();
            size_t u = cp.first;
            size_t path_index = cp.second;
            nextElements.pop_back();
            if (path_index == (size_t)-1) {
//                path_index--;
                visited.remove(u);
                continue;
            }

            visited.add(u);
            // Mark the current node and store it in path[]
            path[path_index] = u;
            path_index++;
            std::pair<const std::vector<size_t>*, size_t> cp2 = std::make_pair(&path, path_index);
            if (path_index > max_size) {
                visited.remove(u);
            } else if (d.contains(u) && pred(cp2)) {
                Observee.Notify(cp2);
                multiplicity--;
                if (multiplicity==0) return;
                visited.remove(u);
            } else {
                // Recur for all the vertices adjacent to current vertex
                bool insertion = false;
                nextElements.emplace_back(u, -1);
                for (const auto& cp : graph.outgoingEdges(u)) {
                    if (!visited.contains(cp.second)) {
//                        std::cout << graph.getNodeLabel(u) << "-->" << graph.getNodeLabel(cp.second) << "@" << path_index << std::endl;
                        nextElements.emplace_back(cp.second, path_index);
//                        std::cout << nextElements << std::endl<< std::endl;
                        insertion = true;
                    }
                }
                if (!insertion) {
                    nextElements.pop_back();
                    path_index--;
                    visited.remove(u);
                }
            }
        }
    }
};


#endif //YAUCL_GRAPH_ALLDIRECTEDPATHS_H
