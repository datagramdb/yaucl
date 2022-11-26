/*
 * hashing.h
 * This file is part of yaucl
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 28/01/2020.
//

#ifndef TUTORIALS_HASHING_H
#define TUTORIALS_HASHING_H

#include <ctime>
#include <functional>

namespace yaucl {
    namespace hashing {
        size_t multihash();

        template <typename T> size_t multihash(T var1) {
            return std::hash<T>()(var1);
        }

        template <typename T, typename... Types> size_t multihash(T var1, Types... var2) {
            return 31 * multihash(var2...) + std::hash<T>()(var1);
        }
    }
}


#endif //TUTORIALS_HASHING_H
