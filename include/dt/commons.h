//
// Created by giacomo on 01/10/24.
//

#ifndef DT_COMMONS_H
#define DT_COMMONS_H

#include <variant>
#include <string>
#include <utility>
#include <vector>

using union_minimal = std::variant<std::string,double>;
using record = std::vector<std::pair<std::string,union_minimal>>;
using data_record = std::vector<record>;
using data_clazzes = std::vector<int>;

#include <dt/structures/find_argument.h>
#include <dt/structures/n_vector.h>
#include <dt/structures/Nodes.h>

#endif //DT_COMMONS_H
