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

namespace yaucl {
    namespace iterators {


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

    }
}

#endif //YAUCL_ITERATORS_H
