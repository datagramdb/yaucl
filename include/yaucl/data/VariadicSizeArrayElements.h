//
// Created by giacomo on 01/05/2022.
//

#ifndef KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H
#define KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H

#include <yaucl/data/MemoryMappingFile.h>
#include "new_iovec.h"
#include <fstream>
#include <unordered_set>
#include <vector>
#include <unordered_map>

namespace yaucl {
    namespace data {
        class VariadicSizeArrayElementsReader {
            yaucl::data::MemoryMappedFile file;
            size_t _size;
            size_t* offsets;
            const char* payload;
            bool opened;

        public:
            VariadicSizeArrayElementsReader(const std::filesystem::path& path);
            VariadicSizeArrayElementsReader();
            VariadicSizeArrayElementsReader(const VariadicSizeArrayElementsReader&) = default;
            VariadicSizeArrayElementsReader(VariadicSizeArrayElementsReader&&) = default;
            VariadicSizeArrayElementsReader& operator=(const VariadicSizeArrayElementsReader&) = default;
            VariadicSizeArrayElementsReader& operator=(VariadicSizeArrayElementsReader&&) = default;

            void open(const std::filesystem::path& p);
            void close();
            size_t size() const;
            const char* operator[](size_t i) const;
            template<class T>
            const T *get(size_t i) const { return  ((!opened) || (T*)((i >= _size)) ? nullptr : payload + offsets[i]); }
            size_t representation_size(size_t i) const;
            const size_t* getOffsets() const { return offsets; }
        };

        class VariadicSizeArrayElementsWriter {
            std::fstream idxTmpFile, dataTmpFile;
            bool isOpen;
            std::filesystem::path path;
            size_t offset;
            size_t ssize;
        public:
            VariadicSizeArrayElementsWriter() : isOpen{false}, offset{0} {}
            VariadicSizeArrayElementsWriter(const VariadicSizeArrayElementsWriter&) = default;
            VariadicSizeArrayElementsWriter(VariadicSizeArrayElementsWriter&&) = default;
            VariadicSizeArrayElementsWriter& operator=(const VariadicSizeArrayElementsWriter&) = default;
            VariadicSizeArrayElementsWriter& operator=(VariadicSizeArrayElementsWriter&&) = default;
            void put(const new_iovec& mem);
            void open(const std::filesystem::path& p);
            void close();
            size_t size() const { return ssize; }
        };

        class VariadicSizeArrayElementsReaderWriter {
            std::filesystem::path filename; // Final file name containing the updated setting
            bool isRead, isWrite;           // Opening mode
            VariadicSizeArrayElementsWriter writer;     // Updates to the reader file
            VariadicSizeArrayElementsReader reader;     // Previously written data, if any

            size_t removed;                             // How many items were removed
            std::unordered_set<size_t> toRemove;        // Ids to be removed

            std::vector<size_t> updates;
            std::unordered_map<size_t, size_t> updatedAt;
            std::unordered_map<size_t, size_t> settedAt;
            std::vector<bool> doIgnore;
            std::vector<bool> actuallyAdded;
            size_t actually_added;
            size_t reader_size;
            bool hasMovedSomeData;

            void prepareWrite() {
                if (isRead) {
                    reader.close();
                    isRead = false;
                }
                if (!isWrite) {
                    writer.open(filename.string()+"_w");
                    isWrite = true;
                }
            }

            void finalizeWritingToFile() {
                size_t size = actually_added - removed;
                if (!isRead) {
                    if (std::filesystem::exists(filename)) {
                        reader.open(filename);
                        isRead = true;
                        size += reader.size();
                        reader_size = reader.size();
                    }
                }
                VariadicSizeArrayElementsReader readingWriter;
                readingWriter.open(filename.string() + "_w");

                auto sol = std::fstream(filename.string() + "_tmp", std::ios_base::binary | std::ios::out);
                sol.write((char*)&size, sizeof(size_t));

                // Writing the offsets
                size_t offsetCounting = 0;
                size_t i = 0, N;
                size_t beforeReading = 0;
                for (N = reader.size(); i < N; i++) {
                    if (!toRemove.contains(i)) {
                        beforeReading++;
                        sol.write((char*)&offsetCounting, sizeof(size_t));
                        auto it = updatedAt.find(i);
                        if (it != updatedAt.end()) {
                            offsetCounting += readingWriter.representation_size(it->second);
                        } else {
                            offsetCounting += reader.representation_size(i);
                        }
                    }
                }
                for (size_t j = beforeReading; j<size; j++) {
                    sol.write((char*)&offsetCounting, sizeof(size_t));
                    offsetCounting += readingWriter.representation_size(settedAt[j]);
                }
//                for (size_t i = 0, M = readingWriter.size(); i<M; i++) {
//                    if ((!doIgnore[i]) && ((N == 0) || (actuallyAdded[i]))) {
//                        sol.write((char*)&offsetCounting, sizeof(size_t));
//                        offsetCounting += readingWriter.representation_size(i);
//                    }
//                }

                N = reader.size();
                for (size_t i =0; i<N; i++) {
                    if (!toRemove.contains(i)) {
                        auto it = updatedAt.find(i);
                        if (it != updatedAt.end()) {
                            auto ref = readingWriter[it->second];
                            sol.write(readingWriter[it->second], readingWriter.representation_size(it->second));
                        } else {
                            auto ref = reader[i];
                            sol.write(reader[i], reader.representation_size(i));
                        }
                    }
                }
                for (size_t j = beforeReading; j<size; j++) {
                        auto ref = readingWriter[settedAt[j]];
                        sol.write(readingWriter[settedAt[j]], readingWriter.representation_size(settedAt[j]));
                }
//                for (size_t i = 0, M = readingWriter.size(); i<M; i++) {
//                    if ((!doIgnore[i]) && ((N == 0) || (actuallyAdded[i]))) {
//                        auto ref = readingWriter[i];
//                        sol.write(readingWriter[i], readingWriter.representation_size(i));
//                    }
//                }

                sol.close();
                readingWriter.close();
                std::filesystem::remove(filename.string() + "_w");
                std::filesystem::remove(filename);
                std::filesystem::rename(filename.string() + "_tmp", filename);

                doIgnore.clear();
                updates.clear();
                toRemove.clear();
                updatedAt.clear();
                actuallyAdded.clear();
                actually_added = 0;
                removed = 0;

                reader.close();
                reader.open(filename);
                reader_size = reader.size();
                isRead = true;
                hasMovedSomeData = false;
            }

            void prepareRead() {
                if (isWrite) {
                    writer.close();
                    isWrite = false;
                }
                if (hasMovedSomeData)
                    finalizeWritingToFile();
                else if (!isRead) {
                    reader.open(filename);
                    reader_size = reader.size();
                    isRead = true;
                }
                isRead = true;
            }

        public:
            VariadicSizeArrayElementsReaderWriter(const std::filesystem::path& path) : filename{path} {
                isRead = false;
                isWrite = false;
                removed = 0;
                actually_added = 0;
                hasMovedSomeData = false;
                if (std::filesystem::exists(filename)) {
                    reader.open(filename);
                    reader_size = reader.size();
                    reader.close();
                } else
                    reader_size = 0;
            }
            VariadicSizeArrayElementsReaderWriter() {
                isRead = false;
                isWrite = false;
                removed = 0;
                actually_added = 0;
                hasMovedSomeData = false;
                reader_size = 0;
                actually_added = 0;
                removed = 0;
            }
            ~VariadicSizeArrayElementsReaderWriter() { close(); }

            void open(const std::filesystem::path& path) {
                close();
                isRead = false;
                isWrite = false;
                removed = 0;
                actually_added = 0;
                filename = path;
                if (std::filesystem::exists(filename)) {
                    reader.open(filename);
                    reader_size = reader.size();
                    reader.close();
                } else
                    reader_size = 0;
            }
            void close() {
                if (isWrite) {
                    writer.close();
                    isWrite = false;
                }
                if (hasMovedSomeData) {
                    finalizeWritingToFile();
                }
                if (isRead) {
                    reader.close();
                    isRead = false;
                }
                isRead = false;
            }
            size_t size() {
                return actually_added+reader_size-removed;
            }
            const char* operator[](size_t i) {
                if ((writer.size() > 0) || (i >= reader.size())) prepareRead();
                return reader[i];
            }
            template<class T> const T* get(size_t i) {
                if ((removed > 0) || (i >= reader.size())) prepareRead();
                return reader.get<T>(i);
            }
            size_t representation_size(size_t i) {
                if ((removed > 0) || (i >= reader.size())) prepareRead();
                return reader.representation_size(i);
            }

            void put(const new_iovec& mem) {
                hasMovedSomeData = true;
                prepareWrite();
                writer.put(mem);
                settedAt[size()] = actuallyAdded.size();
                actuallyAdded.emplace_back(true);
                doIgnore.emplace_back(false);        // I cannot ignore a newly inserted value
                updates.push_back(actually_added++); //
            }
            bool update(size_t i, const new_iovec& mem) {
                toRemove.erase(i); // If I want to update i, then I do not want to remove it anymore!
                if (i < size()) {  // I can only update an existing value
                    hasMovedSomeData = true;
                    if (i > reader_size) {
                        actuallyAdded[i - reader_size] = false;
                    }
                    actuallyAdded.emplace_back(false);
                    auto it = updatedAt.find(i);
                    if (it != updatedAt.end()) {
                        prepareWrite();
                        if (i > reader_size)
                            settedAt[i] = writer.size();
                        doIgnore[it->second] = true;  // Ignoring the old value
                        doIgnore.emplace_back(false); // ...and remembering the new
                        it->second = writer.size();   // Now, I know the pointer to the value to be updated
                        writer.put(mem);
                        updates.push_back(i);         // Remembering which position I am referring to
                    } else {
                        prepareWrite();
                        if (i > reader_size)
                            settedAt[i] = writer.size();
                        updatedAt[i] = writer.size(); // Updating the object for the first time.
                        writer.put(mem);              // Writing the update to file

                        doIgnore[i] = true;  // Ignoring the old value
                        updates.push_back(i);         // Remembering which position I am referring to
                        doIgnore.emplace_back(false); // Remembering the new value
                    }
                } else {
                    return false;
                }
            }
            void removeById(const std::unordered_set<size_t>& ids) {
                for (const auto& id : ids) {
                    hasMovedSomeData = true;
                    if (reader_size <= id) { // If the element has been newly inserted
                        doIgnore[id] = true; // Simply ignore the newly-inserted data
                        actually_added--;
                    } else {
                        toRemove.insert(id);
                    }
                }
                removed = toRemove.size();
            }
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
            FixedSizeArrayElements(const FixedSizeArrayElements&) = default;
            FixedSizeArrayElements(FixedSizeArrayElements&&) = default;
            FixedSizeArrayElements& operator=(const FixedSizeArrayElements&) = default;
            FixedSizeArrayElements& operator=(FixedSizeArrayElements&&) = default;

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
            T* begin() const { return opened ? (T*)file.data() : nullptr; }
            T* end() const { return opened ? (T*)(file.data()+file.file_size()) : nullptr; }
            size_t size() const { return opened ? _size : 0; }
            T& operator[](size_t i) const { return *&payload[i]; }
            T* update(size_t i) { return opened ? &payload[i] : nullptr; }
            size_t representation_size(size_t i) const { return sizeof(T); }
        };
    }
}



#endif //KNOBAB_SERVER_VARIADICSIZEARRAYELEMENTS_H
