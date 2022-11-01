/*
 * iterators.h
 * This file is part of yaucl
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef YAUCL_ITERATORS_H
#define YAUCL_ITERATORS_H

#include <unordered_map>
#include <map>
#include <vector>
#include <numeric>


namespace yaucl {
    namespace iterators {

        template <typename T, typename Lambda>
        std::vector<std::vector<T>> cartesian( std::vector<std::vector<T> >& v, Lambda tester ) {
            auto product = []( long long a, auto& b ) { return a*b.size(); };
            const long long N = std::accumulate( v.begin(), v.end(), 1LL, product );
            std::vector<T> u(v.size());
            std::vector<std::vector<T>> result;
            for( long long n=0 ; n<N ; ++n ) {
                lldiv_t q { n, 0 };
                for( long long i=v.size()-1 ; 0<=i ; --i ) {
                    q = std::div( q.quot, v[i].size() );
                    u[i] = v[i][q.rem];
                }
                if (tester(u)) {
                    result.emplace_back(u);
                }
            }
            return result;
        }

        template <typename typemapite, typename lambda>
        void map_iterate_key_intersection(typemapite it_left, typemapite e_left, typemapite it_right, typemapite e_right,  lambda function) {
            while ((it_left != e_left) && (it_right != e_right)) {
                if (it_left->first < it_right->first)
                    it_left++;
                else if (it_left->first > it_right->first)
                    it_right++;
                else {
                    function(it_left, it_right);
                    it_left++;
                    it_right++;
                }
            }
        }


        /**
 * This utility function removes from a vector v all the elements at the (sorted) positions in [begin,end)
 *
 * @tparam T        type parameter
 * @tparam Iter     index iterator parameter
 * @param v         Vector which elements are to be removed.
 * @param begin     Beginning of sorted indices
 * @param end       End of sorted indices
 */
        template <typename T, typename Iter>
        void removeIndicesFromVector(std::vector<T>& v, Iter begin, Iter end)
// requires std::is_convertible_v<std::iterator_traits<Iter>::value_type, std::size_t>
        {
            ///assert(std::is_sorted(begin, end));
            if (v.empty()) return; // do not allocate and perform extra operations if the vector is empty
            auto rm_iter = begin;
            std::size_t current_index = 0;

            const auto pred = [&](const T&){
                // any more to remove?
                if (rm_iter == end) { return false; }
                // is this one specified?
                if (*rm_iter == current_index++) { return ++rm_iter, true; }
                return false;
            };

            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        }


    }
}

#endif //YAUCL_ITERATORS_H
