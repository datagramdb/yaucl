//
// Created by giacomo on 07/01/2022.
//

#ifndef KNOBAB_EASY_PROP_H
#define KNOBAB_EASY_PROP_H

#include <ostream>
#include <string>
#include <vector>
#include <yaucl/structures/default_constructors.h>

struct easy_prop {
    enum t {
        E_P_AND,
        E_P_OR,
        E_P_ATOM,
        E_P_TRUE,
        E_P_FALSE
    };
    t casusu;
    bool isAtomNegated;
    std::vector<easy_prop> args;
    std::string single_atom_if_any;

    easy_prop(t casusu);
    easy_prop();
    DEFAULT_COPY_ASSGN(easy_prop)

    friend std::ostream &operator<<(std::ostream &os, const easy_prop &pipeline) {
        if (pipeline.isAtomNegated) os << "!";
        switch(pipeline.casusu) {
            case E_P_ATOM:
                os << pipeline.single_atom_if_any;
                break;
            case E_P_FALSE:
                os << "F";
                break;
            case E_P_TRUE:
                os << "T";
                break;
            case E_P_AND:
                os << "And(";
                for (const auto& child : pipeline.args) {
                    os << child << " ";
                }
                os << ")";
                break;
            case E_P_OR:
                os << "Or(";
                for (const auto& child : pipeline.args) {
                    os << child << " ";
                }
                os << ")";
                break;
        }
        return os;
    }
};


#endif //KNOBAB_EASY_PROP_H
