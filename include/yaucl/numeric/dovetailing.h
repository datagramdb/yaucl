/*
 * dovetailing.h
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
// Created by giacomo on 26/07/17.
//

#include <random>
#include <iostream>
#include <values.h>
#include <list>

#ifndef PROJECT_DOVETAILING_H
#define PROJECT_DOVETAILING_H

#include "../commons/structure.h"

/*
 * It can be showed that, in order to represent in secondary memory matrices with truncated files
 * having 10^9 vertices, and if we want to use dovetailing for defining new indices, then we have to
 * check which is the most large representation allowing to represent 10^9 vertices with dovetailing
 *
 * Moreover, the problem with dovetailing is that it can index all the possible integers, and hence the
 * actual representation can explode immediately. We want to define a dovetailing function which is limited
 * by a given value.
 *
 * The way a matrix is indexed provides a great approximation of this:
 *
 * sequential_index(i,j) = i*m + j
 *
 * where, in particular m is the maximum j representable. Hereby, we want that the maximum dovetailing
 * fits into a machine representation which is M = 2^n bits wide, and that i, j fit in this representation
 * too. Hereby, given a the maximum value for both i, j (and consequently, m), we have that
 *
 * i * m + j <= M
 * a(a+1) + a <= M
 *
 * this means that the maximum value is solved by
 *
 * a (a+1) + a = M
 *
 * the non negative solution is then
 *
 * a = sqrt(M + 1) - 1
 *
 * We can observe that the M that allows to represent  a = 10^9 is 2^4, and hence the matrix shall be
 * indexed by only using uint64_t integers. Since the result is 4, this means that the maximum nesting
 * levels for the whole representation is 4
 */

#define MAXLEVEL              4
constexpr uint64_t MAXVALUE = floor(sqrt(exp2(64) + 1.0) - 1.0);

#define undovetail_left_debug(x,MAX)    ((x) / (MAX))
#define undovetail_left(x)              undovetail_left_debug(x,MAXVALUE)

#define undovetail_right_debug(x,MAX)    ((x) % MAX)
#define undovetail_right(x)              undovetail_right_debug(x,MAXVALUE)

#define get_id_for_level(i,x) dovetail(i,x)
#define get_level(c)          undovetail_left(c)
#define get_orig_id(c)        undovetail_right(c)

#define dovetail_debug(x,y,MAX) ((uint64_t)(((uint64_t)x) * (MAX) + ((uint64_t)y)))
#define dovetail(x,y)           dovetail_debug(x,y,MAXVALUE)
//void undovetail(uint64_t, uint64_t*, uint64_t*);
//void benchmark_dovetail();

#endif //PROJECT_DOVETAILING_H
