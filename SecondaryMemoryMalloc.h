//
// Created by giacomo on 02/02/23.
//

#ifndef KNOBAB_SERVER_SECONDARYMEMORYMALLOC_H
#define KNOBAB_SERVER_SECONDARYMEMORYMALLOC_H

#include <string>
#include "yaucl/data/linked_list.h"
#include "yaucl/data/malloc_freelist.h"

#include <yaucl/data/MemoryMappingFile.h>

class SecondaryMemoryMalloc {
    std::string path;
    yaucl::data::MemoryMappedFile mallocer;
    char* data;
    struct ll_head free_list;
public:
    SecondaryMemoryMalloc(const std::string& path, size_t size);
    inline void* malloc(size_t s) {
        return fl_malloc(s, &free_list);
    }
    inline void free(void* ptr) {
        fl_free(ptr, &free_list);
    }
    ~SecondaryMemoryMalloc();
};

// After creating an object of this type, then you can use secondary memory and
// simulate data allocation with fl_malloc

#endif //KNOBAB_SERVER_SECONDARYMEMORYMALLOC_H
