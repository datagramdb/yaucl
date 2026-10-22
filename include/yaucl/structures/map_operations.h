//
// Created by gyankos on 22/10/26.
//

#ifndef PARAMONEUR2_MAP_OPERATIONS_H
#define PARAMONEUR2_MAP_OPERATIONS_H
#include <unordered_map>

template<typename K, typename V>
std::unordered_map<K, V> composeMap(
    const std::unordered_map<K, V>& l,
    const std::unordered_map<K, V>& r) {
    std::unordered_map<K, V> result;
    for (const auto& [k, v] : l) {
        auto it = r.find(v.name);
        if (it != r.end()) {
            result[k] = it->second;
        } else {
            result[k] = v;
        }
    }
    return result;
}

#endif //PARAMONEUR2_MAP_OPERATIONS_H
