//
// Created by giacomo on 29/12/22.
//

#ifndef AIRLINE_NARYTREE_H
#define AIRLINE_NARYTREE_H

#include <yaucl/hashing/vector_hash.h>
#include <unordered_map>
#include "RoaringBitmapWrapper.h"

namespace yaucl::structures {
    struct NAryTree {
        DEFAULT_CONSTRUCTORS(NAryTree)
        std::vector<size_t>                              roots;
        std::vector<RoaringBitmapWrapper>                layerMap;
        std::unordered_map<size_t, RoaringBitmapWrapper> children;
        std::unordered_map<size_t, std::vector<size_t>>  node_to_vector;
        std::unordered_map<std::vector<size_t>, size_t>  vector_to_node;
        std::vector<size_t>                              node_to_layer;
        std::vector<ssize_t>                             parents;
        RoaringBitmapWrapper                             allNodes;

        NAryTree(const std::vector<ssize_t>& parents, const RoaringBitmapWrapper& roots);
        std::unordered_map<size_t, std::vector<size_t>> serializable();
        std::vector<size_t> nodeSet() const;

    private:
        void compile_vertex();
    };
}



#endif //AIRLINE_NARYTREE_H
