/*
 * hash_combine.h
 * This file is part of bpm21
 *
 * Copyright (C) 2020 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */

//
// Created by giacomo on 17/01/21.
//

#ifndef INCONSISTENCY_DETECTOR_SET_OPERATIONS_H
#define INCONSISTENCY_DETECTOR_SET_OPERATIONS_H

#include <unordered_set>


template<typename T>
std::unordered_set<T> unordered_intersection(const std::unordered_set<T> &a,
                                             const std::unordered_set<T> &b){
    if (a.size() > b.size()) return unordered_intersection(b, a);


    std::unordered_set<T> v3;
    for (auto i = a.begin(); i != a.end(); i++) {
        if (b.find(*i) != b.end()) v3.insert(*i);
    }
    return v3;
}

template<typename T>
std::unordered_set<T> unordered_difference(const std::unordered_set<T> &a,
                                           const std::unordered_set<T> &b){
    std::unordered_set<T> v3;
    for (auto i = a.begin(); i != a.end(); i++) {
        if (b.find(*i) == b.end()) v3.insert(*i);
    }
    return v3;
}

template<typename T>
double unodreded_distance(const std::unordered_set<T> &a,
                          const std::unordered_set<T> &b) {
    if (a.size() < b.size()) return unodreded_distance(b, a);
    double total = a.size();
    for (auto i = b.begin(); i != b.end(); i++) {
        if (a.find(*i) == a.end()) total++;
    }
    if (total == 0.0) return 0.0;
    return (((double )unordered_difference(a,b).size())+((double )unordered_difference(b,a).size()))/total;
}

#include <vector>
#include <cmath>

template
        <typename T>
std::vector<std::unordered_set<T>> powerset(const std::unordered_set<T> & a) {
    std::vector<T> set{a.begin(), a.end()};
    std::vector<std::unordered_set<T>> result;
    /*set_size of power set of a set with set_size
    n is (2**n -1)*/

    unsigned long long int pow_set_size = std::pow(2, a.size()), set_size = a.size();
    unsigned long long int counter, j;

    /*Run from counter 000..0 to 111..1*/
    for(counter = 0; counter < pow_set_size; counter++)
    {
        std::unordered_set<T> subset;
        for(j = 0; j < set_size; j++)
        {

            /* Check if jth bit in the counter is set
                If set then print jth element from set */
            if(counter & (1 << j))
                subset.insert(set.at(j));
        }
        result.emplace_back(subset);
    }
    return result;
}

#include <ostream>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T> &s)
{
    os << '{';
    for (auto const& i: s) {
        os << i << ", ";
    }
    return os << '}';
}

#include <numeric>
#include <vector>

template <typename T>
std::unordered_set<std::unordered_set<T>> cartesian_product(const std::vector<std::vector<T>> & v ) {
    const long long N = std::accumulate( v.begin(), v.end(), 1LL, []( long long a, const std::vector<T>& b ) { return a*b.size(); } );
    std::unordered_set<std::unordered_set<T>> result;
    for( long long n=0 ; n<N ; ++n ) {
        lldiv_t q { n, 0 };
        std::unordered_set<T> u;
        for( long long i=v.size()-1 ; 0<=i ; --i ) {
            q = std::div( q.quot, v[i].size() );
            u.insert(v[i][q.rem]);
            //u[i] = v[i][q.rem];
        }
        result.insert(u);
    }
    return result;
}
/*
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> &s)
{
    os << '[';
    for (auto const& i: s) {
        os << i << ", ";
    }
    return os << ']';
}*/

#include <map>
#include <functional>
#include <yaucl/functional/assert.h>

template <typename Iterator, typename Key, typename Value>
std::map<Key, std::vector<Value>> GroupByKeyExtractor(Iterator begin, Iterator end, std::function<Key(const Value&)> keyExtractor)
{
    DEBUG_ASSERT(std::is_sorted(begin, end));
    std::map<Key, std::vector<Value>> groups;
    decltype(end) upper;

    for(auto lower = begin; lower != end; lower = upper)
    {
        Key k = keyExtractor(*lower);

        // get the upper position of all elements with the same ID
        upper = std::upper_bound(begin, end,  *lower,[keyExtractor](const Value& x, const Value& y) { return keyExtractor(x) < keyExtractor(y); });

        // add those elements as a group to the output vector
        groups[k] = {lower, upper};
    }

    return groups;
}

template <typename Iterator, typename Key, typename Value>
std::vector<std::pair<Key, std::vector<Value>>> GroupByKeyExtractorAsVector(Iterator begin, Iterator end, std::function<Key(const Value&)> keyExtractor)
{
    DEBUG_ASSERT(std::is_sorted(begin, end));
    std::vector<std::pair<Key, std::vector<Value>>> groups;
    decltype(end) upper;

    for(auto lower = begin; lower != end; lower = upper)
    {
        Key k = keyExtractor(*lower);

        // get the upper position of all elements with the same ID
        upper = std::upper_bound(begin, end,  *lower,[keyExtractor](const Value& x, const Value& y) { return keyExtractor(x) < keyExtractor(y); });

        // add those elements as a group to the output vector
        groups.emplace_back(k, std::vector<Value>{lower, upper});
    }

    return groups;
}

#include <cassert>

template <typename Iterator, typename Key, typename Value>
std::vector<std::vector<Value>> GroupByKeyExtractorIgnoreKey(Iterator begin, Iterator end, std::function<Key(const Value&)> keyExtractor)
{
    DEBUG_ASSERT(std::is_sorted(begin, end));
    std::vector<std::vector<Value>> groups;
    decltype(end) upper;

    for(auto lower = begin; lower != end; lower = upper)
    {
        Key k = keyExtractor(*lower);

        // get the upper position of all elements with the same ID
        upper = std::upper_bound(begin, end,  *lower,[keyExtractor](const Value& x, const Value& y) { return keyExtractor(x) < keyExtractor(y); });

        // add those elements as a group to the output vector
        groups.emplace_back(std::vector<Value>{lower, upper});
    }

    return groups;
}

template <typename T>
void remove_duplicates(std::vector<T>& vec){
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

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

#endif //INCONSISTENCY_DETECTOR_SET_OPERATIONS_H
