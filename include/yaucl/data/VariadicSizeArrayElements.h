//
// Created by giacomo on 01/05/2022.
//

#ifndef KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H
#define KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H

#include <yaucl/data/MemoryMappingFile.h>

namespace yaucl {
    namespace data {
        class VariadicSizeArrayElements {
            yaucl::data::MemoryMappedFile file;
            size_t _size;
            size_t* offsets;
            const char* payload;

        public:
            VariadicSizeArrayElements(const std::filesystem::path& path);


            size_t size() const;
            const char* operator[](size_t i) const;
            template<class T> const T* get(size_t i) const { return  (T*)((i >= _size) ? nullptr : payload+offsets[i]); }
            size_t representation_size(size_t i) const;
        };

        template <typename T> class FixedSizeArrayElements {
            yaucl::data::MemoryMappedFile file;
            size_t _size;
            T* payload;

        public:
            FixedSizeArrayElements(const std::filesystem::path& path) : file{path} {
                _size = file.file_size() / sizeof(T);
                payload = (T*)file.data();
            }

            const char* end() const { return file.data()+file.file_size(); }
            size_t size() const { return _size; }
            T& operator[](size_t i) const { return *&payload[i]; }
            T* update(size_t i) { return &payload[i]; }
            size_t representation_size(size_t i) const { return sizeof(T); }
        };
    }
}



#endif //KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H
