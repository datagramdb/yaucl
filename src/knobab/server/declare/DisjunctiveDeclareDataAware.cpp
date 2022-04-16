//
// Created by giacomo on 02/01/2022.
//

#include "knobab/server/declare/DisjunctiveDeclareDataAware.h"

DisjunctiveDeclareDataAware::DisjunctiveDeclareDataAware(const DeclareDataAware &singleton) : elementsInDisjunction{singleton} {}

std::ostream &operator<<(std::ostream &os, const DisjunctiveDeclareDataAware &aware) {
    static std::string DISJ{" ⋁ "};
    size_t N = aware.elementsInDisjunction.size();
    if (N == 0)
        os << "true";
    else {
        if (N > 1) os << "( ";
        for (size_t i = 0; i<N; i++) {
            os << aware.elementsInDisjunction.at(i);
            if (i != (N-1))
                os << DISJ;
        }
        if (N > 1) os << " )";
    }
    return os;
}
