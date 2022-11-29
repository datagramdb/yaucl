//
// Created by giacomo on 29/11/22.
//

// Conversion of https://github.com/jackbergus/java_logicalconsistency/blob/best_branch/expansion/src/main/java/algos/graphs/AllCycles.java

#include <yaucl/graphs/algorithms/all_cycles.h>

AllCycles::AllCycles(const adjacency_graph &graph) : graph(graph) {}

void AllCycles::dfsLoopVisit(size_t x, size_t i, std::vector<size_t> &stack_array) {
    visited.add(x);
    stack_array.emplace_back(x);
    for (const size_t edgeId : graph.getOutgoingEdgesId(x)) {
        size_t dst = graph.edge_ids.at(edgeId).second;
        if (visited.contains(dst)) {
            // Checking whether the current element has been visited in the current path
            auto it = std::find(stack_array.begin(), stack_array.end(),dst);
            if (it != stack_array.end()) {
                // If it is present in the stack, then I found a loop
                cycles.emplace(it, stack_array.end());
            }
            // Otherwise, block the recursive iteration
        } else {
            dfsLoopVisit(dst, i+1, stack_array);
        }
    }
    stack_array.pop_back();
}

std::vector<std::vector<size_t>> AllCycles::operator()() { // run
    visited.clear();
    cycles.clear();

    std::vector<std::vector<size_t>> toret;
    std::vector<size_t> array;

    // Getting all the distinct cycles
    array.reserve(graph.V_size+1);
    for (size_t i = 0; i<graph.V_size; i++) {
        if (!visited.contains(i)) {
            dfsLoopVisit(i, 0, array);
        }
    }


    // Moving that into a list, so we can iterate over those and access by id
    toret = {cycles.begin(), cycles.end()};
    std::sort(toret.begin(), toret.end(), [](auto l, auto r) {
        return l.size() < r.size();
    });


    // Removing all the redundant cycles
    for (size_t i = 0, n = cycles.size(); i<n; i++) {
        auto& curI = toret[i];
        std::sort(curI.begin(), curI.end());
        for (int j = 0; j<i; j++) {
            // already sorted per j<i!
            auto& curJ = toret.at(j);
            if (!cycles.contains(curJ)) continue;

            // Removing all the cycles that are contained in bigger cycles: we reduce the number of the loops to visit
            if (std::includes(curI.begin(), curI.end(), curJ.begin(), curJ.end())) {
                cycles.erase(curJ);
            }
        }
    }

    return {cycles.begin(), cycles.end()};
}
