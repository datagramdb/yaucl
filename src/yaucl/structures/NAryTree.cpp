//
// Created by giacomo on 29/12/22.
//

#include <yaucl/structures/NAryTree.h>
#include <yaucl/structures/setoids/basics.h>

yaucl::structures::NAryTree::NAryTree(const std::vector<ssize_t>& parents, const RoaringBitmapWrapper& roots) {
    RoaringBitmapWrapper emptyset{};
    for (size_t i = 0, N = parents.size(); i<N; i++) {
            auto val = parents.at(i);
            if ((val < 0) && (roots.contains(i))) {
                children.emplace(i, emptyset);
            } else if (val >= 0) {
                children[i].add(val);
                children.emplace(val, emptyset);
            }
        }
    }

std::unordered_map<size_t, std::vector<size_t>> yaucl::structures::NAryTree::serializable() {
    std::unordered_map<size_t, std::vector<size_t>> result;
    for (const auto& ref : children) {
        result.emplace(ref.first, ref.second.asVector());
    }
    return result;
}

std::vector<size_t> yaucl::structures::NAryTree::nodeSet() const {
    std::vector<size_t> result;
    for (const auto& ref : children) {
        result.emplace_back(ref.first);
        for (const auto& child : ref.second)
            result.emplace_back(child);
    }
    remove_duplicates(result);
    return result;
}
