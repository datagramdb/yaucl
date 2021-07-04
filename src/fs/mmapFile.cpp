/*
 * mmapFile.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
 *
 * Created on 10/08/17
 *
 * varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with varsorter. If not, see <http://www.gnu.org/licenses/>.
 */



#include <cstring>
#include <iostream>
#include <yaucl/fs/mmapFile.h>

void* yaucl::fs::mmapFile(std::string file, unsigned long* size, int* fd) {
    struct stat filestatus;
    stat( file.c_str(), &filestatus );
    *size = filestatus.st_size;
    char *full_path = realpath(file.data(), NULL);
    *fd = open(full_path,O_RDWR);
    free(full_path);
    void* addr = mmap(NULL,*size, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0 );
    if (addr == MAP_FAILED) {
        std::cout << strerror(errno) << std::endl;
        return nullptr;
    }
    return addr;
}
