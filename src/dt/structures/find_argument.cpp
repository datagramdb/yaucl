//
// Created by giacomo on 02/10/24.
//

#include <dt/structures/find_argument.h>

union_minimal find_argument::a{0.0};
std::string   find_argument::nostring{""};
union_minimal find_argument::b{find_argument::nostring};
union_minimal find_argument::min = std::min(find_argument::a, find_argument::b);