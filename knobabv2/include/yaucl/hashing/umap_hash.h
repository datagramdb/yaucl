//
// Created by giacomo on 10/11/22.
//

#ifndef KNOBAB_SERVER_UMAP_HASH_H
#define KNOBAB_SERVER_UMAP_HASH_H

#include <yaucl/hashing/hash_combine.h>
#include <unordered_map>

namespace std {


    template <typename T, typename K>
    struct hash<std::unordered_map<T, K>>
    {

        std::hash<T> thash;
        std::hash<K> khash;
        hash() {}
        ~hash() {}
        std::size_t operator()(const std::unordered_map<T, K>& k) const
        {
            size_t v = 0;
            size_t init = 31;
            constexpr size_t someDigits = std::numeric_limits<size_t>::digits/3;
            for (const auto& x : k) {
                init = std::rotl(init,someDigits) ^ yaucl::hashing::distribute((uint32_t)(thash(x.first) ^ khash(x.second)));
            }
            return init;
        }
    };

}

#endif //KNOBAB_SERVER_UMAP_HASH_H
