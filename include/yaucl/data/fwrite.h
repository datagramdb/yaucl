/*
 * fwrite.h
 * This file is part of graphSampler
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * graphSampler is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * graphSampler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with graphSampler. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 18/07/17.
//

#ifndef PROJECT_FWRITE_H
#define PROJECT_FWRITE_H

#include <stdio.h>
#include <string.h>

#ifndef DEBUG
#define my_fwrite   fwrite
#else
size_t my_fwrite(const void *src, size_t size, size_t nmemb, void *file);
#endif

#ifdef DEBUG
static inline size_t my_fwrite(const void *src, size_t size, size_t nmemb, void *file) {
    size_t toret = memcpy(*((void**)file), src, size * nmemb) == *((void**)file) ? size : 0;
    *((char**)file) += size;
    return toret;
}
#endif

#endif //PROJECT_FWRITE_H