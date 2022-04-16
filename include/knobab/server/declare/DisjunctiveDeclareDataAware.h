//
// Created by giacomo on 02/01/2022.
//

#ifndef KNOBAB_DISJUNCTIVEDECLAREDATAAWARE_H
#define KNOBAB_DISJUNCTIVEDECLAREDATAAWARE_H

#include <vector>
#include <knobab/server/declare/DeclareDataAware.h>
#include <ostream>

class DisjunctiveDeclareDataAware {
public:
    std::vector<DeclareDataAware> elementsInDisjunction;

    DEFAULT_CONSTRUCTORS(DisjunctiveDeclareDataAware)
    DisjunctiveDeclareDataAware(const DeclareDataAware& singleton);

    friend std::ostream &operator<<(std::ostream &os, const DisjunctiveDeclareDataAware &aware);
};


#endif //KNOBAB_DISJUNCTIVEDECLAREDATAAWARE_H
