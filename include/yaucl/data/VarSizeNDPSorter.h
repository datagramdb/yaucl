//
// Created by giacomo on 05/06/2022.
//

#ifndef KNOBAB_SERVER_VARSIZENDPSORTER_H
#define KNOBAB_SERVER_VARSIZENDPSORTER_H

#include <filesystem>
#include <functional>
#include <yaucl/data/new_iovec.h>
#include "MemoryMappingFile.h"
#include <fstream>

template <typename T> class VarSizeNDPWriter {
    std::ofstream p, idx;
    new_iovec mem;
    std::function<void(const T&,new_iovec&)> w;
    size_t id;
    size_t offset;
    std::filesystem::path filename;

public:
    VarSizeNDPWriter(const std::filesystem::path& file, const std::function<void(const T&,new_iovec&)>& f) :
    p{file}, idx(file.string()+"_idx"), w{f}, id{0}, offset{0}, filename{file} {}
    VarSizeNDPWriter(const std::function<void(const T&,new_iovec&)>& f) :  w{f}, id{0}, offset{0} {}
    void put(const T& data) {
        w(data, mem);
        put(mem);
    }
    void put(const new_iovec& data) {
        idx.write((char*)&offset, sizeof(size_t));
        idx.write((char*)&data.iov_len, sizeof(size_t));
        id++;
        offset += data.iov_len;
        p.write((char*)data.iov_base, data.iov_len);
    }
    ~VarSizeNDPWriter() { p.close(); idx.close(); }
    void close() { p.close(); idx.close(); }
    void open(const std::filesystem::path& file) {
        p.open(file);
        idx.open(file.string()+"_idx");
        filename = file;
    }
};

struct idx_record {
    size_t offset;
    size_t len;

    idx_record(size_t offset, size_t len) : offset(offset), len(len) {}

    bool operator==(const idx_record &rhs) const {
        return offset == rhs.offset && len == rhs.len;
    }
    bool operator!=(const idx_record &rhs) const {
        return !(rhs == *this);
    }
    bool operator<(const idx_record &rhs) const {
        if (offset < rhs.offset)
            return true;
        if (rhs.offset < offset)
            return false;
        return len < rhs.len;
    }
    bool operator>(const idx_record &rhs) const {
        return rhs < *this;
    }
    bool operator<=(const idx_record &rhs) const {
        return !(rhs < *this);
    }
    bool operator>=(const idx_record &rhs) const {
        return !(*this < rhs);
    }
};

class VarSizeNDPSorter {
    size_t runs_size;
    std::function<bool(const new_iovec&, const new_iovec&, char*)> pred;
    int partition(std::vector<new_iovec>& array, const int low, const int high, char* );
    void do_quicksort(std::vector<new_iovec>& array, const int low, const int high, char*);
    void do_quicksort(std::vector<new_iovec>& arr, char* );
public:

    VarSizeNDPSorter(size_t size_runs, const std::function<bool(const new_iovec&, const new_iovec&, char*)>& p);
    void sort(const std::filesystem::path& f, const std::filesystem::path& tmp_path);
};

class VarSizeNDPReader {
    yaucl::data::MemoryMappedFile file, idx;

public:
    VarSizeNDPReader() {}
    VarSizeNDPReader(const std::filesystem::path& p);
    size_t size() const { return idx.file_size()/(sizeof(size_t)*2); }
    char* data() const {
        return file.data();
    }
    new_iovec get(size_t i) const {
        if (i >= size()) {
            return {NULL, 0};
        } else {
            const auto& var = idx.at<idx_record>(i);
            return {file.data()+var.offset, var.len};
        }
    }
    void close() {
        file.close();
        idx.close();
    }
    void open(const std::filesystem::path& p) {
        file.open(p);
        idx.open(p.string()+"_idx");
    }
};


template <typename T> class VarSizeNDPReaderWriter {
    std::function<T(const new_iovec&)> u;
    new_iovec mem;
    std::filesystem::path filename;
    bool isWrite, isRead;
    VarSizeNDPReader reader;
    VarSizeNDPWriter<T> writer;

    void prepareWrite() {
        if (isRead) {
            reader.close();
            isRead = false;
        }
        if (!isWrite) {
            writer.open(filename);
            isWrite = true;
        }
    }
    void prepareRead() {
        if (isWrite) {
            writer.close();
            isWrite = false;
        }
        if (!isRead) {
            reader.open(filename);
            isRead = true;
        }
    }


public:
    VarSizeNDPReaderWriter(const std::filesystem::path& file,
                           std::function<void(const T&,new_iovec&)> w,
                           std::function<T(const new_iovec&)> u) : writer{w}, u{u}, filename{file}, isWrite{false}, isRead{false}{}

    void put(const T& data) {
        prepareWrite();
        writer.put(data);
    }
    void put(const new_iovec& data) {
        prepareWrite();
        writer.put(data);
    }
    size_t size() {
        prepareRead();
        return reader.size();
    }
    T get(size_t i) {
        prepareRead();
        return u(reader.get(i));
    }
    void sort(size_t size_runs, const std::filesystem::path& tmp_path) {
        close();
        std::function<bool(const new_iovec&, const new_iovec&, char*)> f =
                [this](const new_iovec& l, const new_iovec& r, char* data) {
            new_iovec iov;
            iov.iov_base = data+(size_t)l.iov_base;
            iov.iov_len = l.iov_len;
            T left = u(iov);
            iov.iov_base = data+(size_t)r.iov_base;
            iov.iov_len = r.iov_len;
            return left < u(iov);
        };
        VarSizeNDPSorter sorter(size_runs, f);
        sorter.sort(filename, tmp_path);
    }
    void close() {
        if (isWrite) {
            writer.close();
            isWrite = false;
        }
        if (isRead) {
            reader.close();
            isRead = false;
        }
    }
};

#endif //KNOBAB_SERVER_VARSIZENDPSORTER_H
