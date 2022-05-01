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
#include <yaucl/data/mmapFile.h>

void* mmapFile(std::string file, unsigned long* size, int* fd) {
    struct stat filestatus;
    stat( file.c_str(), &filestatus );
    *size = filestatus.st_size;
    *fd = open(file.c_str(),O_RDWR);
    void* addr = mmap(NULL,*size, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0 );
    if (addr == MAP_FAILED) {
        std::cout << strerror(errno) << std::endl;
        return nullptr;
    }
    return addr;
}

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif

size_t availableMemory() {
#ifdef _MSC_VER
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
#else
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
#endif
}
