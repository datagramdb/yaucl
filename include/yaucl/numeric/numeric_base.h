//
// Created by giacomo on 25/10/21.
//

#ifndef YAUCL_NUMERIC_BASE_H
#define YAUCL_NUMERIC_BASE_H

#include <vector>
#include <string>

namespace yaucl {
    namespace numeric {
        std::vector<size_t> numberToBase(size_t n, size_t b);
        long long int parse8601(const std::string &save);
    }
}

#endif //YAUCL_NUMERIC_BASE_H
