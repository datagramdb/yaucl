//
// Created by giacomo on 02/01/2022.
//

#ifndef KNOBAB_CNFDECLAREDATAAWARE_H
#define KNOBAB_CNFDECLAREDATAAWARE_H

#include <knobab/server/declare/DisjunctiveDeclareDataAware.h>
#include <ostream>

struct CNFDeclareDataAware {
    std::vector<DisjunctiveDeclareDataAware> singleElementOfConjunction;
    DEFAULT_CONSTRUCTORS(CNFDeclareDataAware)

    CNFDeclareDataAware(const DeclareDataAware& singleton);
    CNFDeclareDataAware(const std::vector<DeclareDataAware>& orig);

    friend std::ostream &operator<<(std::ostream &os, const CNFDeclareDataAware &aware);
};


#endif //KNOBAB_CNFDECLAREDATAAWARE_H
