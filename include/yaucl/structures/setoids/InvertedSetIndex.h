//
// Created by gyankos on 22/10/26.
//

#ifndef PARAMONEUR2_INVERTEDSETINDEX_H
#define PARAMONEUR2_INVERTEDSETINDEX_H
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

// =============================================================================
// InvertedSetIndex
// =============================================================================
template <typename T>
class InvertedSetIndex {

    std::unordered_map<T, std::vector<uint64_t>> index;
    std::unordered_map<uint64_t, std::size_t> sizes;
    uint64_t nextId;
public:
    using HasInstanceResult = std::pair<bool, std::variant<uint64_t, std::vector<uint64_t>>>;

    InvertedSetIndex() : nextId(0) {}

    InvertedSetIndex(const std::vector<std::unordered_set<T>>& S) : nextId(0) {
        if (!S.empty()) {
            for (const auto& candidateSet : S) {
                addSet(candidateSet);
            }
        }
    }

    uint64_t findSetId(const std::unordered_set<T>& candidateSet) const {
        const std::size_t candidateSize = candidateSet.size();
        if (candidateSize == 0) {
            for (uint64_t possibleEmptyId = 0; possibleEmptyId < nextId; ++possibleEmptyId) {
                auto it = sizes.find(possibleEmptyId);
                if (it != sizes.end() && it->second == 0) {
                    return possibleEmptyId;
                }
            }
            return -1;
        }

        std::unordered_map<uint64_t, std::size_t> candidateCounts;
        std::vector<uint64_t> candidateOrder;
        for (const auto& element : candidateSet) {
            auto it = index.find(element);
            if (it != index.end()) {
                for (uint64_t setId : it->second) {
                    // Only check sets that have exactly the same size
                    // (Controlliamo solo i set che hanno la stessa identica dimensione)
                    if (sizes.at(setId) == candidateSize) {
                        auto ins = candidateCounts.emplace(setId, 0);
                        if (ins.second) {
                            candidateOrder.push_back(setId);
                        }
                        ++ins.first->second;
                    }
                }
            }
        }

        for (uint64_t setId : candidateOrder) {
            if (candidateCounts.at(setId) == candidateSize) {
                // Exact duplicate found (Trovato duplicato esatto)
                return setId;
            }
        }
        return -1;
    }

    uint64_t addSet(const std::unordered_set<T>& candidateSet) {
        uint64_t existingId = findSetId(candidateSet);
        if (existingId != -1) {
            return existingId;
        }

        uint64_t setId = nextId;
        // Store only the numeric counter (Salviamo solo il contatore numerico)
        sizes[setId] = candidateSet.size();

        for (const auto& element : candidateSet) {
            index[element].push_back(setId);
        }

        ++nextId;
        return setId;
    }

    std::pair<bool, uint64_t> removeSet(const std::unordered_set<T>& targetSet) {
        uint64_t setId = findSetId(targetSet);
        if (setId == -1) {
            return {false, -1};
        }

        return removeSetById(setId);
    }

    std::pair<bool, uint64_t> removeSetById(uint64_t setId) {
        auto sizeIt = sizes.find(setId);
        if (sizeIt == sizes.end()) {
            return {false, -1};
        }

        // Remove the size counter (Rimuoviamo il contatore di dimensione)
        std::size_t targetSize = sizeIt->second;
        sizes.erase(sizeIt);

        // If the set was not empty, remove the ID from the inverted lists of its elements
        // (Se il set non era vuoto, rimuoviamo l'ID dalle liste invertite degli elementi)
        if (targetSize > 0) {
            for (auto it = index.begin(); it != index.end();) {
                std::vector<uint64_t>& ids = it->second;
                auto pos = std::find(ids.begin(), ids.end(), setId);
                if (pos != ids.end()) {
                    ids.erase(pos);
                    if (ids.empty()) {
                        it = index.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }

        return {true, setId};
    }

    HasInstanceResult hasInstanceInS(const std::unordered_set<T>& s, bool allMatches = false) const {
        std::vector<uint64_t> validSubsets;
        std::unordered_map<uint64_t, std::size_t> candidateCounts;
        std::vector<uint64_t> candidateOrder;
        const std::size_t querySize = s.size();

        for (const auto& element : s) {
            auto it = index.find(element);
            if (it != index.end()) {
                for (uint64_t setId : it->second) {
                    if (sizes.at(setId) <= querySize) {
                        auto ins = candidateCounts.emplace(setId, 0);
                        if (ins.second) {
                            candidateOrder.push_back(setId);
                        }
                        ++ins.first->second;
                    }
                }
            }
        }

        for (uint64_t setId : candidateOrder) {
            if (candidateCounts.at(setId) == sizes.at(setId)) {
                if (!allMatches) {
                    return {true, setId};
                }
                validSubsets.push_back(setId);
            }
        }

        // Python dict iteration over sizes follows insertion order, which is
        // ascending id order because ids are assigned monotonically.
        for (uint64_t possibleId = 0; possibleId < nextId; ++possibleId) {
            auto it = sizes.find(possibleId);
            if (it == sizes.end()) {
                continue;
            }
            if (it->second == 0) {
                if (!allMatches) {
                    return {true, possibleId};
                }
                if (std::find(validSubsets.begin(), validSubsets.end(), possibleId) == validSubsets.end()) {
                    validSubsets.push_back(possibleId);
                }
            }
        }

        if (!validSubsets.empty()) {
            return {true, validSubsets};
        }

        if (allMatches) {
            return {false, std::vector<uint64_t>{}};
        }
        return {false, -1};
    }

    std::vector<std::unordered_set<T>> getAllSets() const {
        std::unordered_map<uint64_t, std::unordered_set<T>> reconstructed;
        for (const auto& kv : sizes) {
            reconstructed.emplace(kv.first, std::unordered_set<T>{});
        }

        for (const auto& [element, idList] : index) {
            for (uint64_t setId : idList) {
                auto it = reconstructed.find(setId);
                if (it != reconstructed.end()) {
                    it->second.insert(element);
                }
            }
        }

        std::vector<std::unordered_set<T>> result;
        for (uint64_t setId = 0; setId < nextId; ++setId) {
            auto it = reconstructed.find(setId);
            if (it != reconstructed.end()) {
                result.push_back(it->second);
            }
        }
        return result;
    }
};


#endif //PARAMONEUR2_INVERTEDSETINDEX_H
