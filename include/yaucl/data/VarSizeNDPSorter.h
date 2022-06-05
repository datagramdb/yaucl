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

public:
    VarSizeNDPWriter(const std::filesystem::path& file, const std::function<void(const T&,new_iovec&)>& f) :
    p{file}, idx(file.string()+"_idx"), w{f}, id{0}, offset{0} {}

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
    VarSizeNDPReader(const std::filesystem::path& p);

    size_t size() const { return idx.file_size()/(sizeof(size_t)*2); }
    new_iovec get(size_t i) const {
        if (i >= size()) {
            return {NULL, 0};
        } else {
            const auto& var = idx.at<idx_record>(i);
            return {file.data()+var.offset, var.len};
        }
    }
};

#endif //KNOBAB_SERVER_VARSIZENDPSORTER_H
