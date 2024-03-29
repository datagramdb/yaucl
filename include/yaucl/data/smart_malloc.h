/*
 * smart_malloc.h
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * Created on 15/04/19
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

#ifndef VARSORTER_SMART_MALLOC_H
#define VARSORTER_SMART_MALLOC_H

#include <bits/types/struct_iovec.h>
#include <cstring>
#include <iostream>
#include "new_iovec.h"

namespace yaucl {
    namespace memory {
        void memory_copy(char *dest, const char *src, size_t n);


/**
 * Given that malloc operations are quite expensive, this implementation of the malloc preserves the biggest allocated
 * size, and keeps that until the end of the process.
 */
        class smart_malloc {
        public:
            struct new_iovec malloced_iovec;
            uint_fast64_t id;
            bool moved;

            smart_malloc();

            /**
             * In order to use gnu's sort using the move semantics, it is required to
             * @param element
             */
            //smart_malloc(smart_malloc&& element);

            //smart_malloc(smart_malloc& element);


            /**
             * Part used jointly with the move assignment for setting another field to another value
             * @param rhs
             * @return
             */
            //smart_malloc& operator=(smart_malloc const &rhs);

            /**
             * If this is the first time allocating memory, then it will call the malloc. If this otherwise is the second
             * time allocating the memory, it will call the realloc only if the memory to be allocated is larger than the one
             * previously allocated.
             *
             * @param size  Size to be allocated/reallocated
             * @return      Pointer to the actual memory
             */
            void* domalloc(size_t size);

            /**
             * This function copies the memory from the iovec and uses the domalloc to same memory reallocation time
             * @param toCopy
             * @return
             */
            void docopy(struct iovec& toCopy);
            void docopy(struct iovec* toCopy);



            /**
             * Desctructor taking care of freeing the memory
             */
            ~smart_malloc();
        };

/**
 * While swapping, I do not need to set the moved to false.
 * @param lhs
 * @param rhs
 */
        void swap(smart_malloc& lhs, smart_malloc& rhs);

    }
}



#endif //VARSORTER_SMART_MALLOC_H
