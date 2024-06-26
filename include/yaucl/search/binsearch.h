/*
 * binsearch.h
 * This file is part of graphSampler
 *
 * Copyright (C) 2017 - Giacomo Bergami
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


//
// Created by giacomo on 11/11/17.
//

#ifndef GRAPHSAMPLER_BINSEARCH_H
#define GRAPHSAMPLER_BINSEARCH_H

#include <vector>
//#include <serializers/commons/structure.h>

namespace yaucl {
    namespace algorithms {

/**
* Performs the binary search over a vector of Ts
* @tparam T    Type of the vector
* @param A     Vector
* @param v     Value to be searched within the vector
* @param cmp   Comparison between the vector element (left) and the value (right)
* @return      if the value is present or not
*/
        template <typename T> bool binVectorSearch(std::vector<T> A, T v, void (*cmp)(T, T, int*)) {
            long int i = 0;
            long int size = A.size();
            long int j = size-1;
            while ((i<=j) && (i<size) && (j<size)) {
                int cr = 0;
                long int m = (i+j)/2;
                (*cmp)(A[m],v,&cr);
                if (cr==0) return true;
                else if (cr<0) {
                    i = m+1;
                } else {
                    j = m-1;
                }
            }
            return false;
        }

//#include <yaucl/java/legacy_graph_algo_names.h>

/**
 * Performs the binary search over a fixed element of Ts
 * @tparam T    Type of the vector
 * @param A     Vector
 * @param v     Value to be searched within the vector
 * @param cmp   Comparison between the vector element (left) and the value (right)
 * @return      if the value is present or not
 */
        template <typename T> T* binVectorFixedSizeArray(T* A, size_t size, T* v, void (*cmp)(T*, T*, int*)) {
            size_t i = 0;
            size_t j = size-1;
            while (i<=j && (i<size) && (j<size)) {
                int cr = 0;
                size_t m = (i+j)/2;
                (*cmp)(&A[m],v,&cr);
                if (cr==0) return &A[m];
                else if (cr<0) {
                    i = m+1;
                } else {
                    j = m-1;
                }
            }
            return nullptr;
        }

// TODO: binsearch with the offset vector of a given size, plus the pointer to the data structure

        template <typename T> T* binVectorMutableSizeArray(const char* A, const size_t* offset_vector, size_t size, T* v, void (*cmp)(T*, T*, int*)) {
            size_t i = 0;
            size_t j = size-1;
            while ((i<=j) && (i<size) && (j<size)) {
                int cr = 0;
                size_t m = (i+j)/2;
                size_t offset = offset_vector[m];
                (*cmp)((T*)(&A[offset]),v,&cr);
                if (cr==0) return (T*)(&A[offset]);
                else if (cr<0) {
                    i = m+1;
                } else {
                    j = m-1;
                }
            }
            return nullptr;
        }


    }
}

#ifdef __cplusplus
extern "C" {
#endif
static inline void compareU_Int(unsigned int left, unsigned int right, int *toret) {
    *toret = (left==right) ? 0 : (left<right ? -1 : 1);
}

static inline  void compareUL_Int(unsigned long a, unsigned long b, int *res) {
    if (a<b) *res = -1;
    else if (a>b) *res = 1;
    else *res=0;
}

static inline  void pairCompare(std::pair<unsigned long, unsigned long> left, std::pair<unsigned long, unsigned long> right, int *toret) {
    *toret = (left.first==right.first && left.second == right.second) ? 0 :
             ((left.first < right.first || (left.first == right.first && left.second < right.second)) ? -1 : 1);
}
#ifdef __cplusplus
}
#endif
#endif