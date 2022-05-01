//
// Created by giacomo on 15/04/19.
//

#ifndef VARSORTER_NEW_IOVEC_H
#define VARSORTER_NEW_IOVEC_H

#include <string>
#include <sstream>

/**
 * Providing a modern C++ definition of the iovec structure. In particular, this is give with respect to the implementation
 * of the constructor
 */
struct new_iovec {
    /**
     * Pointer to the data to be serialized
     */
    void* iov_base;
    /**
     * Sizoe of the pointed data.
     */
    uint_fast64_t iov_len;
    new_iovec();
    new_iovec(void *memory, uint_fast64_t size);
    new_iovec(int* singleNum);
    new_iovec(std::string& str);
    new_iovec(uint_fast64_t* singleNum);

    // Utility method to use when the data needs to be written in secondary memory
    std::istringstream stream();
};


#endif //VARSORTER_NEW_IOVEC_H
