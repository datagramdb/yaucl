/*
 * assert.h
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
// Created by giacomo on 15/03/2022.
//

#ifndef KNOBAB_ASSERT_H
#define KNOBAB_ASSERT_H

//#define DEBUG
#ifdef DEBUG
#include <cassert>
#define DEBUG_ASSERT(x)     assert(x)
#else
#define DEBUG_ASSERT(x)     do {} while(0)
#endif

#endif //KNOBAB_ASSERT_H
