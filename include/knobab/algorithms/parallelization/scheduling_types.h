//
// Created by giacomo on 16/10/22.
//

#ifndef KNOBAB_SCHEDULING_TYPES_H
#define KNOBAB_SCHEDULING_TYPES_H

#include <vector>
#include <utility>
#include <cstdio>

enum scheduling_type {
    BLOCK_STATIC_SCHEDULE,
    CYCLIC_STATIC_SCHEDULE,
    GUIDED_SCHEDULE,
    MONOTONIC_DYNAMIC_SCHEDULE,
    TASK_SIZE_PREDICTION_BLOCK_STATIC,
    TASK_SIZE_PREDICTION_UNBALANCED_DYNAMIC
};

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <BS_thread_pool_light.hpp>
#include "CanonicalLoop.h"


template<typename T, typename quantifier>
std::vector<std::vector<size_t>> split_tasks_uniform_assumption(const std::vector<T>& orig,
                                                                const quantifier& S,
                                                                int k) {
    std::vector<size_t> l(k, 0);
    size_t min = std::numeric_limits<size_t>::max();
    size_t idx = 0;
    std::vector<T> V(orig.size());
    partial_sort_copy(begin(orig), end(orig), begin(V), end(V));
    std::sort(V.begin(), V.end(), [&S](const T& x, const T& y) {
        return S(x)<S(y);
    });
    std::vector<std::vector<size_t>> tasks(k, std::vector<size_t>{});
    long long i;
    long long M = V.size()-1;
    for (i = M; 2*i-M >= 2*k; i -= k) {
        for (size_t block = 0; block<k; block++) {
            l[k] += S(V.at(i-block))+S(V.at(M-i+block));
            tasks[block].push_back(i-block);
            tasks[block].push_back(M-i+block);
        }
    }
    for (size_t h = 0; h < k; h++) {
        if (l.at(h) < min) {
            idx = h;
            min = l.at(h);
        }
    }
    for (long long j = M-i; j<i; j++) {
        tasks[idx].push_back(j);
    }
    return tasks;
}

template<typename T>
void remove_index(std::vector<T>& vector,
                  const std::vector<size_t>& to_remove)
{
    auto vector_base = vector.begin();
    typename std::vector<T>::size_type down_by = 0;

    for (auto iter = to_remove.cbegin();
         iter < to_remove.cend();
         iter++, down_by++)
    {
        typename std::vector<T>::size_type next = (iter + 1 == to_remove.cend()
                                                   ? vector.size()
                                                   : *(iter + 1));

        std::move(vector_base + *iter + 1,
                  vector_base + next,
                  vector_base + *iter - down_by);
    }
    vector.resize(vector.size() - to_remove.size());
}

template<typename T, typename quantifier> class STDA {
    const quantifier& S;
    size_t k;

public:
    STDA(const quantifier& s, size_t k) : S(s), k(k) {
    }

    std::vector<std::vector<size_t>> split_tasks_disuniform_assumption
            (const std::vector<T>& V, std::vector<size_t>& to_recursively_split) const {
        std::vector<size_t> l(k, 0);
        size_t min = std::numeric_limits<size_t>::max();
        size_t idx = 0;
        std::vector<std::vector<size_t>> chunks;
        std::vector<size_t> element_chunk;
        size_t max_size = S(*V.rbegin());
        chunks.emplace_back(std::vector<size_t>{V.size()-1});
        size_t local_sum = 0;
        for (ssize_t i = V.size()-2; i>=0; i--) {
            size_t local_i_comp = S(V.at(i));
            if (local_sum + local_i_comp > max_size) {
                chunks.emplace_back(element_chunk);
                if (element_chunk.size() > k) {
                    to_recursively_split.push_back(chunks.size()-1);
                }
                element_chunk.clear();
                local_sum = 0;
            }
            local_sum += S(V.at(i));
            element_chunk.emplace_back(i);
        }
        chunks.emplace_back(element_chunk);
        if (element_chunk.size() > k) {
            to_recursively_split.push_back(chunks.size()-1);
        }
        element_chunk.clear();
        return chunks;
    }

    std::vector<std::vector<size_t>> next_elements
            (const std::vector<T>& orig) const {
        std::vector<T> V(orig.size());
        std::partial_sort_copy(begin(orig), end(orig), begin(V), end(V));
        std::vector<size_t> to_recursively_split;
        std::vector<std::vector<size_t>> curr = split_tasks_disuniform_assumption(V, to_recursively_split);
        for (const size_t i : to_recursively_split) {
            const auto& ref = curr.at(i);
            std::vector<T> tmp(ref.size());
            std::vector<size_t> id_mapping(ref.size());
            for (size_t j = 0; j<ref.size(); j++) {
                tmp[j] = V.at(ref.at(j));
                id_mapping[j] = ref.at(j);
            }
            for (std::vector<size_t>& KIND2_INDICES : next_elements(tmp)) {
                for (size_t j = 0, N = KIND2_INDICES.size(); j<N; j++) {
                    KIND2_INDICES[j] = id_mapping.at(KIND2_INDICES.at(j));
                }
                curr.emplace_back(KIND2_INDICES);
            }
        }
        remove_index(curr, to_recursively_split);
        return curr;
    }
};

std::vector<std::pair<size_t,size_t>> block_static_schedule(multithreading::CanonicalLoop& l, size_t num_threads);
std::vector<std::pair<size_t,size_t>> cyclic_static_schedule(multithreading::CanonicalLoop& l, size_t num_threads);
std::vector<std::pair<size_t, size_t>> monotonic_dynamic_schedule(multithreading::CanonicalLoop& l, size_t num_threads);
std::vector<std::pair<size_t,size_t>> guided_schedule(multithreading::CanonicalLoop& l, size_t num_threads);

template <typename T, typename F, typename S>
void schedule(BS::thread_pool_light& pool,
              scheduling_type casusu,
              size_t blocks,
              const std::vector<T>& V,
              const S& size_quantifier,
              F&& task) {
    if (V.empty()) return;
//    std::cout << "ComputingTask" << std::endl;
    multithreading::CanonicalLoop cl;
    cl.init(0, V.size()-1, 1, blocks);
    std::vector<size_t> elems;
    std::vector<std::pair<size_t,size_t>> result;
    bool doSomethingWithResult = false;
    switch (casusu) {
        case BLOCK_STATIC_SCHEDULE:
            result = block_static_schedule(cl, pool.get_thread_count());
            doSomethingWithResult = true;
            break;
        case CYCLIC_STATIC_SCHEDULE:
            result = cyclic_static_schedule(cl, pool.get_thread_count());
            doSomethingWithResult = true;
            break;

        case GUIDED_SCHEDULE:
            result = guided_schedule(cl, pool.get_thread_count());
            doSomethingWithResult = true;
            break;

        case MONOTONIC_DYNAMIC_SCHEDULE:
            result = monotonic_dynamic_schedule(cl, pool.get_thread_count());
            doSomethingWithResult = true;
            break;

        case TASK_SIZE_PREDICTION_BLOCK_STATIC:
            for (const auto& elemsc : split_tasks_uniform_assumption(V, size_quantifier, pool.get_thread_count())) {
                pool.push_task(std::forward<F>(task), elemsc);
            }
            break;
        case TASK_SIZE_PREDICTION_UNBALANCED_DYNAMIC: {
            STDA<T, S> obj{size_quantifier, pool.get_thread_count()};
            for (const auto& elemsc :obj.next_elements(V)) {
                pool.push_task(std::forward<F>(task), elemsc);
            }
            break;
        }
    }

    if (doSomethingWithResult) {
        for (const std::pair<size_t, size_t>& cp : result) {
            for (size_t i = cp.first; i<=cp.second; i++)
                elems.push_back(i);
            pool.push_task(std::forward<F>(task), elems);
            elems.clear();
        }
    }
//    std::cout << "Splitting & Scheduling: done" << std::endl;
}

#endif //KNOBAB_SCHEDULING_TYPES_H
