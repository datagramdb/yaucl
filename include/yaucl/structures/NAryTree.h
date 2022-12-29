//
// Created by giacomo on 29/12/22.
//

#ifndef AIRLINE_NARYTREE_H
#define AIRLINE_NARYTREE_H

#include <unordered_map>
#include "RoaringBitmapWrapper.h"

namespace yaucl::structures {
    struct NAryTree {
        DEFAULT_CONSTRUCTORS(NAryTree)
        std::unordered_map<size_t, RoaringBitmapWrapper> children;
        NAryTree(const std::vector<ssize_t>& parents, const RoaringBitmapWrapper& roots);
    };
}



#endif //AIRLINE_NARYTREE_H
