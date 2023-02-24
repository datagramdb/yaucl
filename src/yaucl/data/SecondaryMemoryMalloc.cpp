//
// Created by giacomo on 02/02/23.
//

#include "yaucl/data/SecondaryMemoryMalloc.h"
#include "yaucl/data/malloc_freelist.h"
#include <fstream>
#include <filesystem>

SecondaryMemoryMalloc::SecondaryMemoryMalloc(const std::string& path, size_t size) : path{path} {
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    ofs.seekp(size);
    ofs.write("", 1);
    mallocer.open(path);
    data = mallocer.data();
    free_list.next = &free_list;
    free_list.prev = &free_list;
    // Initialize our simple freelist with the space.
    // On our embedded platform, this will be the RAM address for your pool and the size you desire.
    malloc_addblock(data, size, &free_list);
}

SecondaryMemoryMalloc::~SecondaryMemoryMalloc() {
    data = nullptr;
    mallocer.close();
    std::filesystem::remove(path);
}