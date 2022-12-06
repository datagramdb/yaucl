//
// Created by giacomo on 15/04/19.
//

#include "yaucl/data/new_iovec.h"

new_iovec::new_iovec(void *memory, size_t size) : iov_base(memory), iov_len(size) {}

std::istringstream new_iovec::stream() {
    std::string elem2{(char*)iov_base,iov_len};
    return std::istringstream(elem2);
}

new_iovec::new_iovec() : iov_base{nullptr}, iov_len{0} {}

new_iovec::new_iovec(int *singleNum) : new_iovec{singleNum, sizeof(int)} {}

new_iovec::new_iovec(std::string &str) : new_iovec{(void*)str.c_str(), str.length()} {}

new_iovec::new_iovec(uint_fast64_t *singleNum) : new_iovec{singleNum, sizeof(singleNum)} {}
