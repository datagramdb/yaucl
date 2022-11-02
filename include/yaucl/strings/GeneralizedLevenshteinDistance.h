//
// Created by giacomo on 17/01/2022.
//

#ifndef KNOBAB_GENERALIZEDLEVENSHTEINDISTANCE_H
#define KNOBAB_GENERALIZEDLEVENSHTEINDISTANCE_H

#include <algorithm>
#include <vector>

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++
template<typename T>
typename T::size_type GeneralizedLevenshteinDistance(const T &source,
                                                     const T &target,
                                                     typename T::size_type insert_cost = 1,
                                                     typename T::size_type delete_cost = 1,
                                                     typename T::size_type replace_cost = 1) {
    if (source.size() > target.size()) {
        return GeneralizedLevenshteinDistance(target, source, delete_cost, insert_cost, replace_cost);
    }

    using TSizeType = typename T::size_type;
    const TSizeType min_size = source.size(), max_size = target.size();
    std::vector<TSizeType> lev_dist(min_size + 1);

    lev_dist[0] = 0;
    for (TSizeType i = 1; i <= min_size; ++i) {
        lev_dist[i] = lev_dist[i - 1] + delete_cost;
    }

    for (TSizeType j = 1; j <= max_size; ++j) {
        TSizeType previous_diagonal = lev_dist[0], previous_diagonal_save;
        lev_dist[0] += insert_cost;

        for (TSizeType i = 1; i <= min_size; ++i) {
            previous_diagonal_save = lev_dist[i];
            if (source[i - 1] == target[j - 1]) {
                lev_dist[i] = previous_diagonal;
            } else {
                lev_dist[i] = std::min(std::min(lev_dist[i - 1] + delete_cost, lev_dist[i] + insert_cost), previous_diagonal + replace_cost);
            }
            previous_diagonal = previous_diagonal_save;
        }
    }

    return lev_dist[min_size];
}

#endif //KNOBAB_GENERALIZEDLEVENSHTEINDISTANCE_H
