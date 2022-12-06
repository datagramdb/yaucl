//
// Created by giacomo on 28/01/2020.
//

#ifndef TUTORIALS_HASHING_H
#define TUTORIALS_HASHING_H

#include <ctime>
#include <functional>

namespace yaucl {
    namespace hashing {
        size_t multihash();

        template <typename T> size_t multihash(T var1) {
            return std::hash<T>()(var1);
        }

        template <typename T, typename... Types> size_t multihash(T var1, Types... var2) {
            return 31 * multihash(var2...) + std::hash<T>()(var1);
        }
    }
}

#include <yaucl/hashing/hash_combine.h>
#include <yaucl/hashing/evaluateHashes.h>
#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/umap_hash.h>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/hashing/vector_hash.h>

#endif //TUTORIALS_HASHING_H
