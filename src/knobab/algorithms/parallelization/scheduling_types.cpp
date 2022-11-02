//
// Created by giacomo on 16/10/22.
//

#include "knobab/algorithms/parallelization/scheduling_types.h"


std::vector<std::pair<size_t, size_t>> block_static_schedule(multithreading::CanonicalLoop &l, size_t num_threads) {
    std::pair<size_t,size_t> cp;
    std::vector<std::pair<size_t,size_t>> result;
    for (size_t i = 0; i<num_threads; i++) {
        result.emplace_back(l.compute_block_static_schedule(i, num_threads));
    }
    return result;
}

std::vector<std::pair<size_t, size_t>> cyclic_static_schedule(multithreading::CanonicalLoop &l, size_t num_threads) {
    // Cyclic static schedule
    bool allSome;
    std::vector<std::pair<size_t,size_t>> result;
    size_t curr_iteration = 0;
    do {
        allSome = true;
        std::optional<std::pair<size_t,size_t>> cp;
        for (size_t i = 0; i<num_threads; i++) {
            cp = l.compute_cyclic_static_schedule(i, num_threads, curr_iteration);
            allSome = allSome && cp.has_value();
            if (allSome) {
                result.emplace_back(cp.value());
            }
        }
        curr_iteration+=num_threads;
//        if (allSome) l.next();
    } while (allSome);
    return result;
}

std::vector<std::pair<size_t, size_t>> guided_schedule(multithreading::CanonicalLoop &l, size_t num_threads) {
    // Cyclic static schedule
    bool allSome;
    std::vector<std::pair<size_t,size_t>> result;
    size_t curr_iteration = 0;
    do {
        allSome = true;
        std::optional<std::pair<size_t,size_t>> cp;
        for (size_t i = 0; i<1; i++) {
            cp = l.compute_guided_dynamic_schedule(curr_iteration, num_threads);
            allSome = allSome && cp.has_value();
            if (allSome) {
                result.emplace_back(cp.value());
            }
        }
    } while (allSome);
    return result;
}

std::vector<std::pair<size_t, size_t>> monotonic_dynamic_schedule(multithreading::CanonicalLoop &l, size_t num_threads) {
    // Cyclic static schedule
    bool allSome;
    std::vector<std::pair<size_t,size_t>> result;
    size_t curr_iteration = 0;
    do {
        allSome = true;
        std::optional<std::pair<size_t,size_t>> cp;
        for (size_t i = 0; i<num_threads; i++) {
            cp = l.compute_monotonic_dynamic_schedule(curr_iteration, num_threads);
            allSome = allSome && cp.has_value();
            if (allSome) {
                result.emplace_back(cp.value());
            }
        }
    } while (allSome);
    return result;
}
