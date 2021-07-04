/*
 * smart_index_pointer.h
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with varsorter. If not, see <http://www.gnu.org/licenses/>.
 */
//
// Created by giacomo on 08/05/19.
//

#ifndef VARSORTER_SMART_INDEX_POINTER_H
#define VARSORTER_SMART_INDEX_POINTER_H

#include <bits/types/struct_iovec.h>
#include <cstdint>
#include "index.h"
#include "smart_malloc.h"

namespace yaucl {
    namespace memory {

/**
 * This data structure provides a smart index pointer: if I have an external data memory providing me the offset for each
 * block, then I use the external information to define the pointer. Otherwise, I calculate the offset by knowing which
 * is the fixed data size in secondary memory
 *
 * WARNING: this is not thread safe
 */
        struct smart_index_pointer {
            smart_malloc malloc;
            struct iovec tmp;
            struct index* external_pointer;
            uint_fast64_t fix_block_size;
            struct index unthread_safe;

            smart_index_pointer();

            /**
             * External memory evaluation
             * @param externalPointer
             */
            smart_index_pointer(struct index *externalPointer);

            /**
             * Offset mimicking
             * @param fixBlockSize
             */
            smart_index_pointer(uint_fast64_t fixBlockSize);

            void open(struct index* ptr);
            void open(uint_fast64_t len);

            /**
             * Accessing the data structure, either for real, or mimicking the results.
             * @param id
             * @return
             */
            struct index* operator[](const uint_fast64_t& id);

            /**
             * Assumes that it is safe to swap the two elements in externalMemory without any data boundary check (e.g., all
             * the blocks have the same size)
             * @param i
             * @param j
             * @param externalMemory
             */
            void unsafe_swap(uint_fast64_t i, const uint_fast64_t &j, char *externalMemory);

        };
    }
}


#endif //VARSORTER_SMART_INDEX_POINTER_H
