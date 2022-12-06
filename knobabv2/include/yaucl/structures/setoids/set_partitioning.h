//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_SET_PARTITIONING_H
#define KNOBAB_SERVER_SET_PARTITIONING_H

#include <set>
#include <yaucl/structures/default_constructors.h>
#include <yaucl/strings/serializers.h>

template <typename T>
struct partition_sets_result {
    std::vector<std::set<size_t>> decomposedSubsets;
    std::vector<std::pair<size_t, std::set<size_t>*>> decomposedIndexedSubsets;
    std::vector<std::set<T>> minimal_common_subsets;
    std::vector<std::set<size_t>> minimal_common_subsets_composition;

    partition_sets_result(size_t N): decomposedSubsets(N) {}
    DEFAULT_COPY_ASSGN(partition_sets_result)

    friend std::ostream &operator<<(std::ostream &os, const partition_sets_result &result) {
        size_t N = result.minimal_common_subsets.size();
        for (size_t i = 0; i < N; i++) {
            os << "mcs type a, mcs#" << i << "-->" << result.minimal_common_subsets.at(i) << std::endl;
        }
        for (size_t i = 0, M = result.minimal_common_subsets_composition.size(); i<M; i++) {
            os << "mcs type b, mcs#" << i+N << "-->" << result.minimal_common_subsets_composition.at(i) << std::endl;
        }
        for (size_t i = 0, M = result.decomposedIndexedSubsets.size(); i<M; i++) {
            auto& ref = result.decomposedIndexedSubsets.at(i);
            os << "result, set#" << i << " <=> " << ref.first << " is " << *ref.second << std::endl;
        }

        return os;
    }
};

template <typename T>
partition_sets_result<T> partition_sets(const std::vector<std::set<T>>& subsubSets) {
    partition_sets_result<T> result(subsubSets.size());

    {
        std::unordered_map<std::vector<size_t>, std::set<T>> elems_to_sets;
        {
            std::unordered_map<T, std::vector<size_t>> id_to_elems;
            for (size_t i = 0, N = subsubSets.size(); i<N; i++) { // O(N*m)
                for (const auto& u : subsubSets.at(i)) {
                    id_to_elems[u].emplace_back(i);
                }
            }

            for (auto it = id_to_elems.begin(); it != id_to_elems.end(); ) { // O(k>m)
                elems_to_sets[it->second].emplace(it->first);
                it = id_to_elems.erase(it);
            }
        }


        for (auto it = elems_to_sets.begin(); it != elems_to_sets.end(); ) { //O(k*N)
            size_t curr = result.minimal_common_subsets.size();
            for (const size_t set_id : it->first) {
                result.decomposedSubsets[set_id].insert(curr);
            }
            result.minimal_common_subsets.emplace_back(it->second);
            it = elems_to_sets.erase(it);
        }

        for (size_t i = 0, N = subsubSets.size(); i<N; i++) { // O(N)
            auto& ref = result.decomposedSubsets.at(i);
            result.decomposedIndexedSubsets.emplace_back(i, &ref);
        }
    }

    // O(N*log(N))
    std::sort(result.decomposedIndexedSubsets.begin(), result.decomposedIndexedSubsets.end(), [](const std::pair<size_t, std::set<size_t>*>& lhs, const std::pair<size_t, std::set<size_t>*>& rhs) {
        return std::includes(rhs.second->begin(), rhs.second->end(), lhs.second->begin(), lhs.second->end());
    });


    // O(N^2)
    std::vector<size_t> toRemove;
    for (size_t i = 0, N = result.decomposedIndexedSubsets.size(); ((N!=0) && (i<N-1)); i++) {
        auto& refI = result.decomposedIndexedSubsets.at(i).second;
        size_t sizeI = refI->size();
        bool hasElem = false;
        size_t refIId = 0;
        for (size_t j = N-1; j>i; j--) {
            auto& refJ = result.decomposedIndexedSubsets.at(j).second;
            if (refJ->size() == sizeI) break;
            if (std::includes(refJ->begin(), refJ->end(), refI->begin(), refI->end())) {
                if (!hasElem) {
                    hasElem = true;
                    refIId = result.minimal_common_subsets.size() + result.minimal_common_subsets_composition.size();
                    result.minimal_common_subsets_composition.emplace_back(refI->begin(), refI->end());
                }
                std::vector<size_t> diff{refIId};
                std::set_difference(refJ->begin(), refJ->end(), refI->begin(), refI->end(), std::back_inserter(diff));
                refJ->clear();
                refJ->insert(diff.begin(), diff.end());
            }
        }
    }
    return result;
}

#endif //KNOBAB_SERVER_SET_PARTITIONING_H
