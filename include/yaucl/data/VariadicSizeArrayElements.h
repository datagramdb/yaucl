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
            bool opened;

        public:
            FixedSizeArrayElements() {
                _size = 0;
                payload = nullptr;
                opened = false;
            }
            FixedSizeArrayElements(const std::filesystem::path& path) : file{path} {
                _size = file.file_size() / sizeof(T);
                payload = (T*)file.data();
                opened = true;
            }

            void open(const std::filesystem::path& path) {
                close();
                file.open(path);
                opened = true;
                _size = file.file_size() / sizeof(T);
                payload = (T*)file.data();
            }
            void close() {
                if (opened) {
                    file.close();
                    opened = false;
                    _size = 0;
                    payload = nullptr;
                }
            }
            const char* end() const { return opened ? file.data()+file.file_size() : nullptr; }
            size_t size() const { return opened ? _size : 0; }
            T& operator[](size_t i) const { return *&payload[i]; }
            T* update(size_t i) { return opened ? &payload[i] : nullptr; }
            size_t representation_size(size_t i) const { return sizeof(T); }
        };
    }
}



#endif //KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H
