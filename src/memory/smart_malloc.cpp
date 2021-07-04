/*
 * smart_malloc.cpp
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

#include <yaucl/memory/smart_malloc.h>

using namespace yaucl::memory;

void yaucl::memory::memory_copy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n; i++)
        dest[i] = src[i];
}

void *yaucl::memory::smart_malloc::domalloc(size_t size) {
    if (malloced_iovec.iov_base == nullptr) {
        malloced_iovec.iov_base = malloc(size);
        memset(malloced_iovec.iov_base, 0, size);
        malloced_iovec.iov_len = size;
    } else if (malloced_iovec.iov_len < size) {
        if (malloced_iovec.iov_base) {
            malloced_iovec.iov_base = realloc(malloced_iovec.iov_base, size);
            malloced_iovec.iov_len = size;
        }
        memset(malloced_iovec.iov_base, 0, size);
    }
    return malloced_iovec.iov_base;
}

yaucl::memory::smart_malloc::~smart_malloc() {
    if ((!moved) && (malloced_iovec.iov_len && malloced_iovec.iov_base)) {
        free(malloced_iovec.iov_base);
    }
    malloced_iovec.iov_base = nullptr;
    malloced_iovec.iov_len = 0;
}

void yaucl::memory::smart_malloc::docopy(struct iovec &toCopy) {
    domalloc(toCopy.iov_len);
    memory_copy((char*)malloced_iovec.iov_base, (char*)toCopy.iov_base, toCopy.iov_len);
}

void yaucl::memory::smart_malloc::docopy(struct iovec *toCopy) {
    domalloc(toCopy->iov_len);
    memory_copy((char*)malloced_iovec.iov_base, (char*)toCopy->iov_base, toCopy->iov_len);
}

yaucl::memory::smart_malloc::smart_malloc() {
    moved = false;
    malloced_iovec.iov_len = 0;
    malloced_iovec.iov_base = nullptr;
}

/*
smart_malloc::smart_malloc(smart_malloc &&element) {
    malloced_iovec.iov_len = element.malloced_iovec.iov_len;
    malloced_iovec.iov_base = element.malloced_iovec.iov_base;
    element.malloced_iovec.iov_len = 0;
    element.malloced_iovec.iov_base = nullptr;
    element.moved = true;
    moved = true;
}

smart_malloc::smart_malloc(smart_malloc &element) {
    malloced_iovec.iov_len = element.malloced_iovec.iov_len;
    malloced_iovec.iov_base = element.malloced_iovec.iov_base;
    element.malloced_iovec.iov_len = 0;
    element.malloced_iovec.iov_base = nullptr;
    element.moved = true;
    moved = true;
}



smart_malloc &smart_malloc::operator=(smart_malloc const &rhs) {
    if(this==&rhs)
        return *this;
    malloced_iovec.iov_base = rhs.malloced_iovec.iov_base;
    malloced_iovec.iov_len = rhs.malloced_iovec.iov_len;
    return *this;
}*/

void yaucl::memory::swap(yaucl::memory::smart_malloc &lhs, yaucl::memory::smart_malloc &rhs) {
    void* tmp = lhs.malloced_iovec.iov_base;
    lhs.malloced_iovec.iov_base = rhs.malloced_iovec.iov_base;
    rhs.malloced_iovec.iov_base = tmp;
    size_t tmp_len = lhs.malloced_iovec.iov_len;
    lhs.malloced_iovec.iov_len =  rhs.malloced_iovec.iov_len;
    rhs.malloced_iovec.iov_len = tmp_len;
    std::swap(lhs.moved, rhs.moved);
}
