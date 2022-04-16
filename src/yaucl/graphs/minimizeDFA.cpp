//
// Created by giacomo on 08/01/2022.
//

#include <yaucl/graphs/algorithms/minimizeDFA.h>

void marcaRec(std::pair<size_t, size_t> cp,
              std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> &M) {
    if (cp.second < cp.first)
        std::swap(cp.first, cp.second);
    auto it = M.find(cp);
    std::unordered_set<std::pair<size_t,size_t>> ls;
    if (std::holds_alternative<std::unordered_set<std::pair<size_t,size_t>>>(it->second)) {
        ls = std::get<std::unordered_set<std::pair<size_t,size_t>>>(it->second);
    }
    //std::cout << "\t\t<" << graph.getNodeLabel(it->first.first) <<',' << graph.getNodeLabel(it->first.second) << "> marked!" << std::endl;
    it->second = {false};
    for (std::pair<size_t,size_t> x : ls) {
        if (x.second < x.first)
            std::swap(x.first, x.second);
        marcaRec(x, M);
    }
}