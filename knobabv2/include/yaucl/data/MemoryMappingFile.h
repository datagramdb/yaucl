//
// Created by giacomo on 30/04/2022.
//

#ifndef VARSORTER_MEMORYMAPPINGFILE_H
#define VARSORTER_MEMORYMAPPINGFILE_H

#include <filesystem>
#include "mmapFile.h"

namespace yaucl {
    namespace data {
        class MemoryMappedFile {
            std::filesystem::path file;
            unsigned long size;
            mmap_file file_descriptor;
            void* memory;
            bool doClose;

        public:
            MemoryMappedFile(const std::filesystem::path& file);
            MemoryMappedFile();
            ~MemoryMappedFile();
            MemoryMappedFile(const MemoryMappedFile& x);
            MemoryMappedFile(MemoryMappedFile&& x);
            MemoryMappedFile& operator=(const MemoryMappedFile& x);
            MemoryMappedFile& operator=(MemoryMappedFile&& x);

            void open(const std::filesystem::path& file);
            void close();

            char* data() const { return (char*)memory; }
            size_t file_size() const { return size; }
            template <typename T> size_t cast_size() const { return size / (sizeof (T)); }

            template <typename T> const T& at(size_t pos) const {
                return *&((const T*)memory)[pos];
            }
            template <typename T> T& get(size_t pos) {
                return *&((T *) memory)[pos];
            }
        };

    }
}


#endif //VARSORTER_MEMORYMAPPINGFILE_H
