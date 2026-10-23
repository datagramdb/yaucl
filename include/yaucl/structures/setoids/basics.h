//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_BASICS_H
#define KNOBAB_SERVER_BASICS_H

#include <set>
#include <unordered_set>
#include <ostream>
#include <algorithm>
#include <vector>

template <typename T>
std::vector<T> common_shared_subvector(const std::vector<T>& l,
                                       const std::vector<T>& r) {
    std::vector<T> result;
    size_t N = std::min(l.size(), r.size());
    result.reserve(N);
    for (size_t i = 0; i<N; i++) {
        if (l.at(i) == r.at(i)) result.emplace_back(l.at(i));
        else return result;
    }
    return result;
}

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
std::set<T> ordered_intersection(const std::set<T> &a, const std::set<T> &b){
    std::set<T> v3;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(v3, v3.begin()));
    return v3;
}

template<typename T>
bool isSubsetOf(const std::set<T> &a, const std::set<T> &b) {
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
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
std::set<T> ordered_difference(const std::set<T> &a, const std::set<T> &b){
    std::set<T> v3;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(v3, v3.begin()));
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

static inline
std::vector<int> getOnLocations(int a) {
    std::vector<int> result;
    int place = 0;
    while (a != 0) {
        if (a & 1) {
            result.push_back(place);
        }
        ++place;
        a >>= 1;
    }
    return result;
}


template<typename T>
std::vector<std::unordered_set<T> > powerSet(const std::vector<T>& set, size_t maxSize) {
    std::vector<std::unordered_set<T> > result;
    int numPowerSets = static_cast<int>(std::pow(2.0, static_cast<double>(set.size())));
    for (size_t i = 0; i < numPowerSets; ++i) {
        std::vector<int> onLocations = getOnLocations(i);
        if (onLocations.size() > maxSize) continue;
        if (onLocations.empty()) continue;
        std::unordered_set<T> subSet;
        for (size_t j = 0; j < onLocations.size(); ++j) {
            subSet.insert(set.at(onLocations.at(j)));
        }
        result.push_back(subSet);
    }
    return result;
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
            q = std::lldiv( q.quot, v[i].size() );
            u.insert(v[i][q.rem]);
            //u[i] = v[i][q.rem];
        }
        result.insert(u);
    }
    return result;
}

#include <functional>

template <typename T>
void cartesian_product( const std::vector<std::vector<T>> & v, const std::function<void(const std::vector<T>&)>& function) {
    const long long N = std::accumulate( v.begin(), v.end(), 1LL, []( long long a, const std::vector<T>& b ) { return a*b.size(); } );
    std::vector<T> u(v.size());
    for( long long n=0 ; n<N ; ++n ) {
        lldiv_t q { n, 0 };
        for( long long i=v.size()-1 ; 0<=i ; --i ) {
            q = lldiv( q.quot, v[i].size() );
            u[i] = v[i][q.rem];
        }
        function(u);
    }
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
#include <algorithm>
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


template<class InputIt2, class OutputIt>
OutputIt set_complement(size_t max_val,
                        InputIt2 first2, InputIt2 last2, OutputIt d_first)
{
    size_t i = 0;
    while (i<max_val) {
        if (first2 == last2) {
            while (i<max_val) {
                *d_first++ = i++;
            }
            return d_first;
        }
        if (i < *first2) {
            *d_first++ = i++;
        } else {
            if (!(*first2 < i))
                i++;
            ++first2;
        }
    }
    return d_first;
}

template <typename T, typename F>
void remove_duplicates(std::vector<T>& vec, F comparator){
    std::sort(vec.begin(), vec.end(), comparator);
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template <typename T, typename D>
void iterate_union(const T& s1, const T& s2, D d) {
    auto it1 = s1.begin();
    auto it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end()) {
        if (*it1 < *it2) {
            // only in set1
            d(*it1);
            ++it1;
        } else if (*it2 < *it1) {
            // only in set2
            d(*it2);
            ++it2;
        } else {
            // in both
            d(*it1);
            ++it1;
            ++it2;
        }
    }
    for (; it1 != s1.end(); ++it1) {
        // only in set1
        d(*it1);
    }
    for (; it2 != s2.end(); ++it2) {
        // only in set2
        d(*it2);
    }
}


template <typename Container>
void in_place_ordered_intersection(Container& set_1, const Container& set_2) {
    //https://stackoverflow.com/a/1773620/1376095
    auto it1 = set_1.begin();
    auto it2 = set_2.begin();
    while ( (it1 != set_1.end()) && (it2 != set_2.end()) ) {
        if (*it1 < *it2) {
            set_1.erase(it1++);
        } else if (*it2 < *it1) {
            ++it2;
        } else { // *it1 == *it2
            ++it1;
            ++it2;
        }
    }
    set_1.erase(it1, set_1.end());
}

template<typename T, typename S>
void remove_index(std::vector<T>& vector, const S& to_remove)
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

template <class C, class V>
auto append(C& container, V&& value, int)
-> decltype(container.push_back(std::forward<V>(value)), void())
{
    container.push_back(std::forward<V>(value));
}

template <class C, class V>
void append(C& container, V&& value, ...)
{
    container.insert(std::forward<V>(value));
}

template <class C, class V>
void AddToContainer(C& container, V&& value) {
    append(container, std::forward<V>(value), 0);
}

template <class T>
void reorder(std::vector<T>& vA, std::vector<size_t>& vI)
{
    DEBUG_ASSERT(vA.size() == vI.size());
    std::vector<T> slowReorder;
    slowReorder.reserve(vI.size());
    for (size_t idx : vI) {
        slowReorder.emplace_back(vA[idx]);
    }
    std::swap(vA, slowReorder);
}

static inline
std::vector<std::vector<int>> generate_all_indipendent_maximal_sets(
    int N, const std::set<std::pair<int, int>>& C) {

    std::vector<std::unordered_set<int>> conflitti(N);
    for (const auto& uv : C) {
        int u = uv.first;
        int v = uv.second;
        conflitti[u].insert(v);
        conflitti[v].insert(u);
    }

    std::vector<std::unordered_set<int>> compatibilita(N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i != j && conflitti[i].find(j) == conflitti[i].end()) {
                compatibilita[i].insert(j);
            }
        }
    }

    std::vector<std::vector<int>> risultato;

    std::function<void(std::unordered_set<int>, std::unordered_set<int>, std::unordered_set<int>)> bronKerbosch;
    bronKerbosch = [&](std::unordered_set<int> R, std::unordered_set<int> P, std::unordered_set<int> X) {
        if (P.empty() && X.empty()) {
            std::vector<int> clique(R.begin(), R.end());
            std::sort(clique.begin(), clique.end());
            risultato.push_back(clique);
            return;
        }

        std::unordered_set<int> unionePX = P;
        unionePX.insert(X.begin(), X.end());

        // pivot = max(unione_px, key=lambda x: len(compatibilita[x] & P))
        // First maximal element in iteration order is kept (strict '>').
        int pivot = -1;
        std::size_t bestScore = 0;
        bool firstElement = true;
        for (int x : unionePX) {
            std::size_t score = 0;
            for (int p : P) {
                if (compatibilita[x].find(p) != compatibilita[x].end()) {
                    ++score;
                }
            }
            if (firstElement || score > bestScore) {
                pivot = x;
                bestScore = score;
                firstElement = false;
            }
        }

        // candidati_da_esplorare = list(P - compatibilita[pivot])
        std::vector<int> candidatiDaEsplorare;
        for (int p : P) {
            if (compatibilita[pivot].find(p) == compatibilita[pivot].end()) {
                candidatiDaEsplorare.push_back(p);
            }
        }

        for (int nodo : candidatiDaEsplorare) {
            std::unordered_set<int> nuovoP;
            for (int p : P) {
                if (compatibilita[nodo].find(p) != compatibilita[nodo].end()) {
                    nuovoP.insert(p);
                }
            }
            std::unordered_set<int> nuovoX;
            for (int x : X) {
                if (compatibilita[nodo].find(x) != compatibilita[nodo].end()) {
                    nuovoX.insert(x);
                }
            }

            std::unordered_set<int> nuovoR = R;
            nuovoR.insert(nodo);
            bronKerbosch(nuovoR, nuovoP, nuovoX);

            P.erase(nodo);
            X.insert(nodo);
        }
    };

    std::unordered_set<int> allNodes;
    for (int i = 0; i < N; ++i) {
        allNodes.insert(i);
    }
    bronKerbosch(std::unordered_set<int>{}, allNodes, std::unordered_set<int>{});
    return risultato;
}

// Equivalent of itertools.product(pool, repeat=repeat), iterated lazily.
// Yields one empty tuple when repeat == 0 (as Python's product does).
// Iteration stops as soon as fn returns true (short-circuit).
template<typename T>
static void forEachTuple(const std::vector<T>& pool,
                         std::size_t repeat,
                         const std::function<bool(const std::vector<T>&)>& fn) {
    if (repeat == 0) {
        std::vector<T> empty;
        fn(empty);
        return;
    }
    if (pool.empty()) {
        return;
    }
    std::vector<std::size_t> idx(repeat, 0);
    while (true) {
        std::vector<T> tuple;
        tuple.reserve(repeat);
        for (std::size_t i : idx) {
            tuple.push_back(pool[i]);
        }
        if (fn(tuple)) {
            return;
        }
        std::size_t p = repeat;
        bool carried = true;
        while (carried && p > 0) {
            --p;
            if (++idx[p] < pool.size()) {
                carried = false;
            } else {
                idx[p] = 0;
            }
        }
        if (carried) {
            return;
        }
    }
}

// Equivalent of set(x) >= S, where x is a tuple of ints and S is a set of ints.
template<typename T>
static bool supersetOf(const std::vector<T>& x, const std::set<T>& S) {
    std::set<T> xs(x.begin(), x.end());
    return std::includes(xs.begin(), xs.end(), S.begin(), S.end());
}

#endif //KNOBAB_SERVER_BASICS_H
