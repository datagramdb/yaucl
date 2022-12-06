/*
 * default_constructors.h
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
// Created by giacomo on 02/01/2022.
//

#ifndef KNOBAB_DEFAULT_CONSTRUCTORS_H
#define KNOBAB_DEFAULT_CONSTRUCTORS_H

#define DEFAULT_ASSIGNMENT(name)   name& operator=(const name&) = default; name& operator=(name && ) = default;
#define DEFAULT_COPY_CONSTRS(name) name (const name& ) = default; name (name && ) = default;
#define DEFAULT_COPY_ASSGN(name)   DEFAULT_COPY_CONSTRS(name) DEFAULT_ASSIGNMENT(name)
#define DEFAULT_CONSTRUCTORS(name) name () = default; DEFAULT_COPY_ASSGN(name)

#endif //KNOBAB_DEFAULT_CONSTRUCTORS_H
