//
// Created by giacomo on 29/12/22.
//

#include <yaucl/structures/NAryTree.h>
#include <yaucl/structures/setoids/basics.h>
//#include <iostream>

yaucl::structures::NAryTree::NAryTree(const std::vector<ssize_t>& parents, const RoaringBitmapWrapper& roots) {
    RoaringBitmapWrapper emptyset{};
//    std::cout << "new tree" << std::endl;
    for (size_t i = 0, N = parents.size(); i<N; i++) {
        auto val = parents.at(i);
        if ((val < 0) && (roots.contains(i))) {
            this->roots.emplace_back(i);
            children.emplace(i, emptyset);
        } else if (val >= 0) {
//            std::cout << val << "-->" << i << std::endl;
            children[val].add(i);
            children.emplace(i, emptyset);
        }
    }
//    for (const auto& cp : children)
//        for (size_t dst : cp.second)
//            std::cout << "[[" << cp.first << "-->" << dst << "]" << std::endl;
//    if (children.size() > 1)
//        std::cout << "DEBUG" << std::endl;
    compile_vertex();
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

#include <stack>

void yaucl::structures::NAryTree::compile_vertex() {

    // Compiling the vector representation for common ancestor calculation
    roaring::Roaring64Map visited;
    std::stack<size_t> stack;
    for (size_t i = 0, N = roots.size(); i<N; i++){
        auto it = node_to_vector.emplace(roots.at(i), std::vector<size_t>{i});
        assert(it.second);
        auto it2 = vector_to_node.emplace(std::vector<size_t>{i}, roots.at(i));
        assert(it2.second);
        stack.emplace(roots.at(i));
    }
    while (!stack.empty()) {
        auto src = stack.top();
        auto src_vec = node_to_vector.at(src);
        stack.pop();
        src_vec.emplace_back(0);
        visited.add(src);
        auto it = children.find(src);
        if (it != children.end()) {
            size_t outNo = 0;
            for (size_t dst : it->second) {
                if (!visited.contains(dst)) {
                    src_vec.back() = outNo;
                    auto it3 = node_to_vector.emplace(dst, src_vec);
                    assert(it3.second);
                    auto it2 = vector_to_node.emplace(src_vec, dst);
                    assert(it2.second);
                    stack.emplace(dst);
                }
                outNo++;
            }
        }
    }

    // Generating the layer information
    size_t layer_id = 0;
    RoaringBitmapWrapper tmp;
    RoaringBitmapWrapper layer{roots};
    layerMap.emplace_back(roots);
    visited = layer.get();
    layer_id++;
    while (!layer.empty()) {
        for (size_t idp : layer) {
            tmp |= children.at(idp);
        }
        tmp -= visited;
        visited |= tmp.get();
        if (tmp.empty())
            layerMap.emplace_back(); // Empty insertion
        else
            layerMap.emplace_back(tmp.asVector());
        std::swap(layer, tmp);
        tmp.clear();
    }
}
