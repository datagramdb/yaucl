//
// Created by giacomo on 01/05/2022.
//

#include "yaucl/data/VariadicSizeArrayElements.h"

using namespace yaucl::data;

VariadicSizeArrayElements::VariadicSizeArrayElements(const std::filesystem::path &path) : file{path} {
    _size = file.at<size_t>(0);
    offsets = (size_t*)(file.data()+sizeof(size_t));
    payload = file.data()+sizeof(size_t)*(_size+1);
}

size_t VariadicSizeArrayElements::size() const { return file.at<size_t>(0); }

const char *VariadicSizeArrayElements::operator[](size_t i) const { return  (i >= _size) ? nullptr : payload+offsets[i]; }

size_t VariadicSizeArrayElements::representation_size(size_t i) const {
    if (i >= _size) return 0;
    else if (i == (_size-1)) {
        return (file.data()+file.file_size())-(payload+offsets[i]);
    } else {
        return offsets[i+1]-offsets[i];
    }
}
