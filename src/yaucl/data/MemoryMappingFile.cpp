//
// Created by giacomo on 30/04/2022.
//

#include <yaucl/data/MemoryMappingFile.h>
#include <yaucl/data/mmapFile.h>

using namespace yaucl::data;

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &file) : file{file}, memory{nullptr}, file_descriptor{}, size{0}, doClose{true} {
#ifndef __MSC_VER
    this->memory = mmapFile(file.string(), &size, &file_descriptor);
#endif
    if (!memory) size = 0;
}

MemoryMappedFile::~MemoryMappedFile() {
    if (memory && doClose) {
#ifndef _MSC_VER
        munmap(memory, size);
        ::close(file_descriptor.fd);
        memory = nullptr;
#else
        mmapClose(this->memory, &file_descriptor);
        memory = nullptr;
        doClose = false;
#endif
    }
}

void MemoryMappedFile::close() {
    if (memory && doClose) {
#ifndef _MSC_VER
        munmap(memory, size);
        ::close(file_descriptor.fd);
        memory = nullptr;
#else
        mmapClose(this->memory, &file_descriptor);
        memory = nullptr;
        doClose = false;
#endif
        file.clear();
    }
}

void MemoryMappedFile::open(const std::filesystem::path &file) {
    doClose = true;
#ifndef __MSC_VER
    this->file = file;
    this->memory = mmapFile(file.string(), &size, &file_descriptor);
#endif
    if (!memory) size = 0;
}

MemoryMappedFile::MemoryMappedFile() : file{}, size{0}, file_descriptor{}, memory{nullptr}, doClose{false} {}

MemoryMappedFile &MemoryMappedFile::operator=(MemoryMappedFile &&x) {
    file = x.file;
    size = x.size;
    file_descriptor = x.file_descriptor;
    memory = x.memory;
    x.memory = nullptr;
    x.file.clear();
    x.size = 0;
    doClose = x.doClose;
    x.doClose = false;
    return *this;
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&x) {
    file = x.file;
    size = x.size;
    file_descriptor = x.file_descriptor;
    memory = x.memory;
    x.memory = nullptr;
    x.file.clear();
    x.size = 0;
    doClose = x.doClose;
    x.doClose = false;
}

MemoryMappedFile::MemoryMappedFile(const MemoryMappedFile &x) {
    file = x.file;
    size = x.size;
    file_descriptor = x.file_descriptor;
    memory = x.memory;
    doClose = false;
}

MemoryMappedFile &MemoryMappedFile::operator=(const MemoryMappedFile &x) {
    file = x.file;
    size = x.size;
    file_descriptor = x.file_descriptor;
    memory = x.memory;
    doClose = false;
    return *this;
}
