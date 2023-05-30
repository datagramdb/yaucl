//
// Created by giacomo on 17/01/23.
//

#ifndef AIRLINE_TRANSITIVE_CLOSURE_H
#define AIRLINE_TRANSITIVE_CLOSURE_H

#include <unordered_map>
#include <unordered_set>

template <typename ObjectIdentifier> static inline
std::unordered_map<ObjectIdentifier, std::unordered_set<ObjectIdentifier>> transitive_closure_from_map(const std::unordered_map<ObjectIdentifier, std::unordered_set<ObjectIdentifier>>& adj) {
    auto result = adj;

    // Calculating the implicit vertex set
    std::unordered_set<ObjectIdentifier> V;
    for (const auto& ref : adj) {
        V.insert(ref.first);
        V.insert(ref.second.begin(), ref.second.end());
    }

    for (const auto& i : V) {
        auto it = result.find(i);
        if (it == result.end()) continue; // If this is not a node with an outgoing edge, skipping it!
        auto tmpSetI = it->second;
        for (const auto& j : tmpSetI) {
            auto it2 = result.find(j);
            if (it2 == result.end()) continue; // Also, discarding this if there is no outgoing arc from j.
            auto tmpSetJ = it2->second;
            for (const auto& k : tmpSetJ) {
                result[i].insert(k);
            }
        }
    }
    return result;
}

#endif //AIRLINE_TRANSITIVE_CLOSURE_H
