/*
 * mmapFile.h
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * 
 * Created on 10/08/17
 *
 * varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with varsorter. If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef VARSORTER_MMAPFILE_H
#define VARSORTER_MMAPFILE_H


#include <string>
#include <fstream>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

}

/**
 * Opens a file using the memory-mapping technique, and also getting the file size information. 
 *
 * @param file	File name to be opened (in read mode)
 * @param size	Non-null pointer that will be set with the actual size -- TODO: replace with &
 * @param fd    Non-null pointer that will be set to the filedescriptor
 * @return	The virtual memory containing the memory-mapped file
 */
void* mmapFile(std::string file, unsigned long* size, int* fd);

size_t availableMemory();

#endif //VARSORTER_MMAPFILE_H
