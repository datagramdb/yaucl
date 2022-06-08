//
// Created by giacomo on 01/05/2022.
//

#include "yaucl/data/VariadicSizeArrayElements.h"

using namespace yaucl::data;

VariadicSizeArrayElementsReader::VariadicSizeArrayElementsReader(const std::filesystem::path &path) : file{path} {
    _size = file.at<size_t>(0);
    offsets = (size_t*)(file.data()+sizeof(size_t));
    payload = file.data()+sizeof(size_t)*(_size+1);
    opened = true;
}

size_t VariadicSizeArrayElementsReader::size() const { return (!opened) ? 0 : file.at<size_t>(0); }

const char *VariadicSizeArrayElementsReader::operator[](size_t i) const {
    return (!opened) || (i >= _size) ? nullptr : payload + (offsets[i]);
}

size_t VariadicSizeArrayElementsReader::representation_size(size_t i) const {
    if ((!opened) || (i >= _size)) return 0;
    else if (i == (_size-1)) {
        return (file.data()+file.file_size())-(payload+(offsets[i]));
    } else {
        return (offsets[i+1])-(offsets[i]);
    }
}

VariadicSizeArrayElementsReader::VariadicSizeArrayElementsReader() {
    opened = false;
    _size = 0;
    offsets = nullptr;
    payload = nullptr;
}


void VariadicSizeArrayElementsReader::open(const std::filesystem::path &p) {
    close();
    file.open(p);
    _size = file.at<size_t>(0);
    offsets = (size_t*)(file.data()+sizeof(size_t));
    payload = file.data()+sizeof(size_t)*(_size+1);
    opened = true;
}

void VariadicSizeArrayElementsReader::close() {
    if (opened) {
        file.close();
        opened = false;
    }
}

void VariadicSizeArrayElementsWriter::put(const new_iovec &mem) {
    if (isOpen) {
        idxTmpFile.write((char*)&offset, sizeof(offset));
        dataTmpFile.write((char*)mem.iov_base, mem.iov_len);
        ssize++;
        offset+=mem.iov_len;
    }

}

#include <fstream>

void VariadicSizeArrayElementsWriter::open(const std::filesystem::path &p) {
    close();
    path = p;
    isOpen = true;
    idxTmpFile = std::fstream(path.string()+"_idx", std::ios::out | std::ios::binary);
    dataTmpFile = std::fstream(path.string()+"_dat", std::ios::out | std::ios::binary);
}

void VariadicSizeArrayElementsWriter::close() {
    isOpen = false;
    idxTmpFile.close();
    dataTmpFile.close();
    auto sol = std::fstream(path, std::ios_base::out | std::ios_base::binary);
    sol.write((char*)&ssize, sizeof(size_t));

    {
        auto f = std::fstream(path.string()+"_idx", std::ios_base::in | std::ios::binary);
        sol << f.rdbuf();
    }
    {
        auto f = std::fstream(path.string()+"_dat", std::ios_base::in | std::ios::binary);
        sol << f.rdbuf();
    }

    std::filesystem::remove(path.string()+"_dat");
    std::filesystem::remove(path.string()+"_idx");

    offset = 0;
    ssize = 0;
}
