/*
 * memory_utils.cpp
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2019 - Giacomo Bergami
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

#include <yaucl/memory/memory_utils.h>
#include <sys/sysinfo.h>

using namespace yaucl::memory;

unsigned long yaucl::memory::get_ram_avail_bytes() {
    struct sysinfo myinfo{};
    unsigned long total_bytes;
    sysinfo(&myinfo);
    return myinfo.mem_unit * myinfo.totalram;
}
