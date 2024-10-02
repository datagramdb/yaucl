//
// Created by giacomo on 02/10/24.
//

#ifndef DT_N_VECTOR_H
#define DT_N_VECTOR_H

#include <vector>

struct n_vector {

    inline void init(size_t nclasses) {
        nclazzes = nclasses+1;
        n.resize(nclazzes, 0);
    }
    inline void reset() {
        std::fill(n.begin(), n.end(), 0);
    }
    inline void increment(size_t clazzNo) {
        if (clazzNo <= nclazzes)
            n[clazzNo]++;
    }
    inline size_t count(size_t clazzNo) const {
        return n.at(clazzNo);
    }
private:
    std::vector<size_t> n;
    size_t nclazzes;
};

#endif //DT_N_VECTOR_H
