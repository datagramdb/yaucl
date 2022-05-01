//
// Created by giacomo on 30/04/2022.
//

#ifndef KNOBAB_SERVER_MINIHEAP_IOVEC_H
#define KNOBAB_SERVER_MINIHEAP_IOVEC_H

#include <knobab/server/ndp/new_iovec.h>
#include <cstdint>

/**
 * Data structure used by the min-Heap.
 * This structure is relevant not only because it stores each file value per time, but also because it stores which
 * file is associated to the given file
 */
struct miniheap_iovec {
    struct new_iovec iovec;
    int fileid;
    miniheap_iovec(void *memory, uint_fast64_t size, int fileno);
};


#endif //KNOBAB_SERVER_MINIHEAP_IOVEC_H
