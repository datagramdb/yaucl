/*
 * ssize_t.h
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
// Created by giacomo on 02/03/2022.
//

#ifndef KNOBAB_SSIZE_T_H
#define KNOBAB_SSIZE_T_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <BaseTsd.h>
#define ssize_t SSIZE_T
#else
#include <sys/types.h>
#endif

#endif //KNOBAB_SSIZE_T_H
