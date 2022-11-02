//
// Created by giacomo on 16/10/22.
//

#include <knobab/algorithms/parallelization/CanonicalLoop.h>

namespace multithreading {

    void CanonicalLoop::init(size_t b, size_t e, size_t i, size_t c) {
        base = b;
        end = e;
        incr = i;
        chunk_ = c;
        if (incr > 0)
            count = 1 + (end-base)/incr;
        else
            count = 1+(base-end)/(-incr);
        count = (count+chunk_-1)/chunk_;
        scale = chunk_*incr;
    }

    size_t CanonicalLoop::getCount() const {
        return count;
    }

    size_t CanonicalLoop::getChunk() const {
        return scale / incr;
    }

    bool CanonicalLoop::isLastChunk(size_t iteration) const {
        return (size_t)(iteration) == (count - 1);
    }

    size_t CanonicalLoop::getChunkLower(size_t iteration) const {
        return base + iteration * scale;
    }

    size_t CanonicalLoop::getChunkUpper(size_t iteration) const {
        auto chunkEnd = getChunkLower(iteration) + scale - 1;
        if ((isLastChunk(iteration))) { [[likely]]
            chunkEnd = end;
        }
        return chunkEnd;
    }

    size_t CanonicalLoop::getStride(size_t base, size_t end) const {
        auto res = getChunkUpper(end) - getChunkLower(base) + 1;
        return res;
    }

    std::optional<std::pair<size_t, size_t>>
    CanonicalLoop::compute_cyclic_static_schedule(size_t me, size_t numThreads, size_t iteration) {
        std::optional<std::pair<size_t,size_t>> opt;
        if (base+(me+iteration)*scale < end)  {
            opt = {base+(me+iteration)*scale, std::min(end, base+(me+iteration+1)*scale-incr)};
        }
        return opt;
    }

    std::pair<size_t, size_t> CanonicalLoop::compute_block_static_schedule(size_t me, size_t numThreads) {
        size_t wholeIters = count / numThreads;
        size_t leftover = count % numThreads;
        size_t myBase, extras;
        if (me < leftover) {
            myBase = me * (wholeIters+1);
            extras = 1;
        } else {
            myBase = me * wholeIters + leftover;
            extras = 0;
        }
        return {getChunkLower(myBase),getChunkUpper(myBase+wholeIters-1)+extras*incr};

    }

    std::optional<std::pair<size_t, size_t>>
    CanonicalLoop::compute_guided_dynamic_schedule(size_t &global_next_iteration, size_t threadCount) {
        std::optional<std::pair<size_t, size_t>> opt;
        size_t local_next_iteration = global_next_iteration;
        size_t remaining = getCount() - local_next_iteration;
        if (remaining == 0) {
            return opt;
        }
        size_t myShare = ((remaining+threadCount-1)/threadCount);
        size_t delta = (myShare+1)/2;
        if (non_atomic_compare_exchange_strong(global_next_iteration,
                                               local_next_iteration,
                                               local_next_iteration + delta)) {
            size_t last_iteration = local_next_iteration+delta-1;
            opt = {getChunkLower(local_next_iteration),
                   getChunkUpper(last_iteration)};

        }
        return opt;
    }

    std::optional<std::pair<size_t, size_t>>
    CanonicalLoop::compute_monotonic_dynamic_schedule(size_t &global_next_iteration, size_t threadCount) {
        std::optional<std::pair<size_t, size_t>> opt;
        size_t localNI = global_next_iteration;
        if ((localNI == getCount())) { [[unlikely]]
            return opt;
        }
        if (non_atomic_compare_exchange_strong(global_next_iteration,localNI, localNI+1)) {
            opt = {getChunkLower(localNI), getChunkUpper(localNI)};
        }
        return opt;
    }


} // multithreading