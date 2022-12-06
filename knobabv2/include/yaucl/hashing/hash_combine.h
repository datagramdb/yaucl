/*
 * hash_combine.h
 * This file is part of ProbabilisticTraceAlignment
 *
 * Copyright (C) 2020 - Giacomo Bergami
 *
 * ProbabilisticTraceAlignment is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ProbabilisticTraceAlignment is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProbabilisticTraceAlignment. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 20/10/20.
//

#ifndef FUZZYSTRINGMATCHING2_HASH_COMBINE_H
#define FUZZYSTRINGMATCHING2_HASH_COMBINE_H

// @author: https://stackoverflow.com/a/50978188/1376095
#include <limits>
//#include <cstdint>
#include <type_traits>
#include <string>
#include <bit>

namespace yaucl {
    namespace hashing {

        template<typename T>
        T xorshift(const T& n,int i){
            return n^(n>>i);
        }

        uint32_t distribute(const uint32_t& n);

        uint64_t hash(const uint64_t& n);

// if c++20 rotl is not available:
        /*template <typename T,typename S>
        typename std::enable_if<std::is_unsigned<T>::value,T>::type
        constexpr rotl(const T n, const S i){
            const T m = (std::numeric_limits<T>::digits-1);
            const T c = i&m;
            return (n<<c)|(n>>((T(0)-c)&m)); // this is usually recognized by the compiler to mean rotation, also c++20 now gives us rotl directly
        }*/

        template <class T> size_t hash_combine(const std::size_t seed, const T& v)
        {
            std::hash<T> hash1;
            return std::rotl(seed,std::numeric_limits<size_t>::digits/3) ^ distribute((uint32_t)hash1(v));
        }
    }
}


#endif //FUZZYSTRINGMATCHING2_HASH_COMBINE_H
