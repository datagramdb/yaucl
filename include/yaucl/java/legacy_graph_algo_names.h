

#ifndef YAUCL_LEGACY_GRAPH_ALGO_NAMES_H
#define YAUCL_LEGACY_GRAPH_ALGO_NAMES_H

/*
 * structure.h
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

//#include <stdint-gcc.h>
#include <stdint.h>

typedef uint_fast64_t
        LONG_NUMERIC;

typedef uint_fast32_t
        UINT;

#define NUMERIC_MEMORY_OCCUPATION  \
        (sizeof(LONG_NUMERIC))

#include <vector>
typedef std::vector<LONG_NUMERIC>
        NUMERIC_VECTOR;

typedef std::vector<std::pair<LONG_NUMERIC, LONG_NUMERIC>>
        PAIR_NUMERIC_VECTOR;

#endif //YAUCL_LEGACY_GRAPH_ALGO_NAMES_H
