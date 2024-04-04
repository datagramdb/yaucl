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


void* mmapFile(std::string file, unsigned long* size, mmap_file* fd) {
#ifdef _MSC_VER
    TCHAR *lpFileName = file.data();
    LPVOID lpBasePtr;
    LARGE_INTEGER liFileSize;
    fd->hFile = CreateFile(lpFileName,
                       GENERIC_READ,                          // dwDesiredAccess
                       0,                                     // dwShareMode
                       NULL,                                  // lpSecurityAttributes
                       OPEN_EXISTING,                         // dwCreationDisposition
                       FILE_ATTRIBUTE_NORMAL,                 // dwFlagsAndAttributes
                       0);                                    // hTemplateFile
    if (fd->hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateFile failed with error %d\n", GetLastError());
        return nullptr;
    }

    if (!GetFileSizeEx(fd->hFile, &liFileSize)) {
        fprintf(stderr, "GetFileSize failed with error %d\n", GetLastError());
        CloseHandle(fd->hFile);
        return nullptr;
    }

    if (liFileSize.QuadPart == 0) {
        fprintf(stderr, "File is empty\n");
        CloseHandle(fd->hFile);
        return nullptr;
    }

    fd->hMap = CreateFileMapping(
            fd->hFile,
            NULL,                          // Mapping attributes
            PAGE_READONLY,                 // Protection flags
            0,                             // MaximumSizeHigh
            0,                             // MaximumSizeLow
            NULL);                         // Name
    if (fd->hMap == 0) {
        fprintf(stderr, "CreateFileMapping failed with error %d\n", GetLastError());
        CloseHandle(fd->hFile);
        return nullptr;
    }

    lpBasePtr = MapViewOfFile(
            fd->hMap,
            FILE_MAP_READ|FILE_MAP_WRITE,         // dwDesiredAccess
            0,                     // dwFileOffsetHigh
            0,                     // dwFileOffsetLow
            0);                    // dwNumberOfBytesToMap
    if (lpBasePtr == NULL) {
        fprintf(stderr, "MapViewOfFile failed with error %d\n", GetLastError());
        CloseHandle(fd->hMap);
        CloseHandle(fd->hFile);
        return nullptr;
    }
    char *ptr = (char *)lpBasePtr;
    fd->len = *size = liFileSize.QuadPart;
    return ptr;
#else
    struct stat filestatus;
    stat( file.c_str(), &filestatus );
    *size = filestatus.st_size;
    fd->len = *size;
    fd->fd = open(file.c_str(),O_RDWR);
    void* addr = mmap(NULL,*size, PROT_READ | PROT_WRITE, MAP_SHARED, fd->fd, 0 );
    if (addr == MAP_FAILED) {
        std::cout << file << ": " << strerror(errno) << std::endl;
        return nullptr;
    }
    return addr;
#endif
}


void mmapClose(void* ptr, mmap_file* fd) {
#ifdef _MSC_VER
    UnmapViewOfFile(ptr);
    CloseHandle(fd->hMap);
    CloseHandle(fd->hFile);
#else
    munmap(ptr, fd->len);
    close(fd->fd);
    fd->clear();
#endif
}

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
