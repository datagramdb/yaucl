//
// Created by giacomo on 30/04/2022.
//

#include <yaucl/data/MemoryMappingFile.h>
#include <yaucl/data/mmapFile.h>

using namespace yaucl::data;

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &file) : file{file}, memory{nullptr}, file_descriptor{0}, size{0} {
#ifndef __MSC_VER
    this->memory = mmapFile(file.string(), &size, &file_descriptor);
#endif
}

MemoryMappedFile::~MemoryMappedFile() {
#ifndef __MSC_VER
    if (memory) {
        munmap(memory, size);
        ::close(file_descriptor);
        memory = nullptr;
    }
#endif
}

void MemoryMappedFile::close() {
#ifndef __MSC_VER
    if (memory) {
        munmap(memory, size);
        ::close(file_descriptor);
        memory = nullptr;
        file.clear();
    }
#endif
}

void MemoryMappedFile::open(const std::filesystem::path &file) {
#ifndef __MSC_VER
    this->file = file;
    this->memory = mmapFile(file.string(), &size, &file_descriptor);
#endif
}

MemoryMappedFile::MemoryMappedFile() : file{}, size{0}, file_descriptor{-1}, memory{nullptr} {}
