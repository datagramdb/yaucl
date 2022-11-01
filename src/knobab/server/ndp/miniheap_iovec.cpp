//
// Created by giacomo on 30/04/2022.
//

#include "knobab/server/ndp/miniheap_iovec.h"

miniheap_iovec::miniheap_iovec(void *memory, uint_fast64_t size, int fileno) : iovec{memory, size}, fileid{fileno} {}
