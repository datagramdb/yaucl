//
// Created by giacomo on 27/08/19.
//

#ifndef INCONSISTENCY_LINKEDHASHMULTIMAP_H
#define INCONSISTENCY_LINKEDHASHMULTIMAP_H

#include <vector>
#include <map>

namespace yaucl {
    namespace structures {
        template <typename K, typename V>
        struct LinkedHashMultimap {
            std::vector<K> data;
            std::map<K, std::vector<V>> map;
            void put(K key, V value) {
                auto it = map.find(key);
                if (it == map.end()) {
                    data.emplace_back(key);
                    map[key].emplace_back(value);
                } else {
                    it->second.emplace_back(value);
                }
            }

            void clear() {
                data.clear();
                map.clear();
            }

        };

    }
}

#endif //INCONSISTENCY_LINKEDHASHMULTIMAP_H
