//
// Created by gyankos on 22/10/26.
//

#ifndef PARAMONEUR2_MAP_OPERATIONS_H
#define PARAMONEUR2_MAP_OPERATIONS_H
#include <unordered_map>
#include <vector>

template<typename K, typename V>
std::unordered_map<K, V> composeMap(
    const std::unordered_map<K, V>& l,
    const std::unordered_map<K, V>& r) {
    std::unordered_map<K, V> result;
    for (const auto& [k, v] : l) {
        auto it = r.find(v.name);
        if (it != r.end()) {
            result.emplace(k, it->second);
        } else {
            result.emplace(k, v);
        }
    }
    return result;
}

// Equivalent to dict(zip(lFV, x)).
template<typename K, typename V>
static std::unordered_map<K, V> zipToMap(const std::vector<K>& keys,
                                                   const std::vector<V>& values) {
    std::unordered_map<K, V> result;
    for (std::size_t i = 0; i < keys.size() && i < values.size(); ++i) {
        result.emplace(keys[i],values[i]);
    }
    return result;
}

#endif //PARAMONEUR2_MAP_OPERATIONS_H
