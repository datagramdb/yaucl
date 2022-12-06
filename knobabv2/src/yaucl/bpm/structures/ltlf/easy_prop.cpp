//
// Created by giacomo on 07/01/2022.
//

#include "yaucl/bpm/structures/commons/easy_prop.h"

easy_prop::easy_prop() : casusu{E_P_TRUE}, isAtomNegated{false}, args{}, single_atom_if_any{} {}


easy_prop::easy_prop(t casusu) : casusu{casusu} {}