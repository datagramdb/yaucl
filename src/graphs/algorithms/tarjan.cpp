//
// Created by giacomo on 06/07/2020.
//

#include <iostream>
#include <yaucl/graphs/algorithms/tarjan.h>
using namespace yaucl::graphs;
using namespace yaucl::graphs::algorithms;

yaucl::graphs::algorithms::tarjan::tarjan(yaucl::graphs::primary_memory_graph &graph) : graph(graph) {
    reset();
}

void tarjan::run() {
    for (; vertices_iterator != vertices_end; vertices_iterator++) {
        const yaucl::graphs::adjacency_list_entry& v = *vertices_iterator;
        if (vindex.find(v.id) == vindex.end()) {
            if (strongconnect_recursion.empty())
                strongconnect_recursion.push(std::make_pair(v.id, graph.get_vertex(v.id).outgoing.end()));
            while (!strongconnect_recursion.empty()) {
                std::pair<size_t, std::unordered_map<size_t, edge_collateral>::iterator> cp = strongconnect_recursion.top();
                auto& u = graph.get_vertex(cp.first);

                bool doBreak = false;
                if (cp.second == u.outgoing.end()) {
                    vindex[u.id] = lowlink[u.id] = ++index;
                    S.push(u.id);
                    onStack.insert(u.id);

                    auto it = u.outgoing.begin();
                    if (adjIteration(u, it)) {
                        strongconnect_recursion.pop();
                        strongconnect_recursion.push(std::make_pair(u.id, it));
                        strongconnect_recursion.push(std::make_pair(it->first, graph.get_vertex(it->first).outgoing.end()));
                        doBreak = true;
                    }
                } else {
                    auto it = cp.second;
                    size_t v = it->first;
                    lowlink[u.id] = std::min(lowlink[u.id], lowlink[v]);
                    if (u.outgoing.end() != it) {
                        it++;
                        if (adjIteration(u, it)) {
                            strongconnect_recursion.pop();
                            strongconnect_recursion.push(std::make_pair(u.id, it));
                            strongconnect_recursion.push(std::make_pair(it->first, graph.get_vertex(it->first).outgoing.end()));
                            doBreak = true;
                        }
                    }
                }
                if (!doBreak) {
                    size_t w = 0;  // To store stack extracted vertices
                    if (lowlink[u.id] == vindex[u.id]) {
                        std::vector<size_t> tmp;
                        tmp.reserve(S.size());
                        while (S.top() != u.id) {
                            w =  S.top();
                            tmp.emplace_back(w);
                            onStack.erase(onStack.find(w));
                            S.pop();
                        }
                        w =  S.top();
                        tmp.emplace_back(w);
                        if (tmp.size() > 1) current = tmp;
                        onStack.erase(onStack.find(w));
                        S.pop();
                    }
                    strongconnect_recursion.pop();
                    if (!current.empty())
                        return;
                }
            }
            //strongconnect(v, scc);
        }
    }

    /*for (const adjacency_list_entry& v : graph.vertices) {

    }*/
    current.clear();
}

bool yaucl::graphs::algorithms::tarjan::adjIteration(yaucl::graphs::adjacency_list_entry &u,
                          std::unordered_map<size_t, yaucl::graphs::edge_collateral>::iterator &it)  {
    bool doBreak = false;
    for  (; it != u.outgoing.end(); it++) {
        size_t v = it->first;
        if (vindex.find(v) == vindex.end()) {
            doBreak = true;
            break;
        } else if (onStack.find(v) != onStack.end()) {
            lowlink[u.id] = std::min(lowlink[u.id], vindex[v]);
        }
    }
    return doBreak;
}

void yaucl::graphs::algorithms::tarjan::strongconnect(const yaucl::graphs::adjacency_list_entry &u, std::vector<std::vector<size_t>> &scc) {
    // Initialize discovery time and low value
    vindex[u.id] = lowlink[u.id] = ++index;
    S.push(u.id);
    onStack.insert(u.id);

    // Go through all vertices adjacent to this
    for  (auto it = u.outgoing.begin(); it != u.outgoing.end(); it++) {
        size_t v = it->first;
        if (vindex.find(v) == vindex.end()) {
            strongconnect(graph.read_vertex(v), scc);
            lowlink[u.id] = std::min(lowlink[u.id], lowlink[v]);
        } else if (onStack.find(v) != onStack.end()) {
            lowlink[u.id] = std::min(lowlink[u.id], vindex[v]);
        }
    }

    // head node found, pop the stack and print an SCC
    size_t w = 0;  // To store stack extracted vertices
    if (lowlink[u.id] == vindex[u.id]) {
        std::vector<size_t> tmp;
        tmp.reserve(S.size());
        while (S.top() != u.id) {
            w =  S.top();
            tmp.emplace_back(w);
            onStack.erase(onStack.find(w));
            S.pop();
        }
        w =  S.top();
        tmp.emplace_back(w);
        if (tmp.size() > 1) scc.emplace_back(tmp);
        onStack.erase(onStack.find(w));
        S.pop();
    }


}

void yaucl::graphs::algorithms::tarjan::reset() {
    index = 0;
    vertices_iterator = graph.vertices.begin();
    vertices_end = graph.vertices.end();
    while (!strongconnect_recursion.empty()) strongconnect_recursion.pop();
    run();
}

bool yaucl::graphs::algorithms::tarjan::hasNext() {
    return !current.empty();
}

std::vector<size_t> yaucl::graphs::algorithms::tarjan::next() {
    std::vector<size_t> toret{current};
    current.clear();
    run();
    return toret;
}
