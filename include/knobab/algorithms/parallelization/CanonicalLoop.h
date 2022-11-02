//
// Created by giacomo on 16/10/22.
//

#ifndef KNOBAB_CANONICALLOOP_H
#define KNOBAB_CANONICALLOOP_H

#include <yaucl/numeric/ssize_t.h>
#include <cstddef>
#include <utility>
#include <optional>
#include <functional>

namespace multithreading {

    class CanonicalLoop {
        size_t base, incr, end, scale, count, chunk_;

    public:
        void init(size_t b, size_t e, size_t i, size_t c);

        size_t getCount() const;
        size_t getChunk() const;

        // Return user-space versions of the bounds of a chunk.
        bool isLastChunk(size_t iteration) const;
        size_t getChunkLower(size_t iteration) const;
        size_t getChunkUpper(size_t iteration) const;
        size_t getStride(size_t base, size_t end) const;

        std::optional<std::pair<size_t,size_t>> compute_cyclic_static_schedule(size_t me, size_t numThreads, size_t iteration);

        std::pair<size_t,size_t> compute_block_static_schedule(size_t me, size_t numThreads);

        bool non_atomic_compare_exchange_strong(size_t& self, size_t& expected, size_t desired) {
            bool result = (self == expected);
            if (result)
                self = desired;
            else
                expected = self;
            return result;
        }

        std::optional<std::pair<size_t, size_t>> compute_guided_dynamic_schedule(size_t& global_next_iteration,
                                                                                 size_t threadCount);

        std::optional<std::pair<size_t, size_t>> compute_monotonic_dynamic_schedule(size_t& global_next_iteration,
                                                                                    size_t threadCount);

    };


} // multithreading

#endif //KNOBAB_CANONICALLOOP_H
