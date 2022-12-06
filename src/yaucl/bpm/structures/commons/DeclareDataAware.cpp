//
// Created by giacomo on 06/12/22.
//

#include <yaucl/bpm/structures/commons/DeclareDataAware.h>

bool SimpleDeclareDataAware::operator==(const SimpleDeclareDataAware &rhs) const {
    return template_name == rhs.template_name &&
           n == rhs.n &&
           left_act == rhs.left_act &&
           right_act == rhs.right_act &&
           dnf_left_map == rhs.dnf_left_map &&
           dnf_right_map == rhs.dnf_right_map &&
           conjunctive_map == rhs.conjunctive_map;
}

bool SimpleDeclareDataAware::operator!=(const SimpleDeclareDataAware &rhs) const {
    return !(rhs == *this);
}
