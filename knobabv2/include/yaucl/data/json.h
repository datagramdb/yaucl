//
// Created by giacomo on 14/11/2021.
//

#ifndef KNOBAB_JSON_H
#define KNOBAB_JSON_H

#include <nlohmann/json.hpp>
#define UNESCAPE(x)                  nlohmann::json::parse(x).get<std::string>()

#endif //KNOBAB_JSON_H
