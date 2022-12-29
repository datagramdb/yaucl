//
// Created by giacomo on 29/12/22.
//

#include "yaucl/structures/NAryTree.h"
yaucl::structures::NAryTree::NAryTree(const std::vector<ssize_t>& parents, const RoaringBitmapWrapper& roots) {
        for (size_t i = 0, N = parents.size(); i<N; i++) {
            auto val = parents.at(i);
            if ((val < 0) && (roots.contains(i))) {
                children.emplace(i, RoaringBitmapWrapper{});
            } else if (val >= 0) {
                children[i].add(val);
            }
        }
    }