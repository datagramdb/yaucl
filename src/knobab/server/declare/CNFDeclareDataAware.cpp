//
// Created by giacomo on 02/01/2022.
//

#include "knobab/server/declare/CNFDeclareDataAware.h"

CNFDeclareDataAware::CNFDeclareDataAware(const DeclareDataAware &singleton) : singleElementOfConjunction{singleton} {}

CNFDeclareDataAware::CNFDeclareDataAware(const std::vector<DeclareDataAware> &orig) : singleElementOfConjunction() {
    singleElementOfConjunction.reserve(orig.size());
    for (const auto& ref : orig)
        singleElementOfConjunction.emplace_back(ref);
}

std::ostream &operator<<(std::ostream &os, const CNFDeclareDataAware &aware) {
        static std::string CONJ{" ⋀ "};
        size_t N = aware.singleElementOfConjunction.size();
        if (N == 0)
            os << "false";
        else {
            for (size_t i = 0; i<N; i++) {
                os << aware.singleElementOfConjunction.at(i);
                if (i != (N-1))
                    os << CONJ;
            }
        }
        return os;
}
