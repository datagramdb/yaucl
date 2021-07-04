/*
 * memory_utils.h
 * This file is part of fuzzyStringMatching
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


//
// Created by giacomo on 31/12/18.
//

#ifndef INCONSISTENCY_MEMORY_UTILS_H
#define INCONSISTENCY_MEMORY_UTILS_H

namespace yaucl {
    namespace memory {
        /**
         * This function provides a real-time result on which is the available RAM memory.
         * This must be used to check whether we can allocate a file in primary memory or not.
         *
         * @return
         */
        unsigned long get_ram_avail_bytes();
    }
}

#endif //INCONSISTENCY_MEMORY_UTILS_H
