//
// Created by giacomo on 03/01/2022.
//

#ifndef KNOBAB_GROUNDING_H
#define KNOBAB_GROUNDING_H

#include <string>
#include <unordered_map>
#include <optional>
#include <yaucl/bpm/structures/commons/DataPredicate.h>
#include <yaucl/functional/optional_binary.h>
#include <knobab/server/declare/DeclareDataAware.h>

/*
yaucl::functional::optional_binary<DataPredicate> instantiateWithValues(
                                                   const DataPredicate& toGround,
                                                   const std::unordered_map<std::string, union_minimal> &mapL,
                                                   const std::unordered_map<std::string, union_minimal> &mapR,
                                                   std::unordered_map<std::string, std::vector<DataPredicate>>& toIntersectAtAnUpperLevelLeft,
                                                   std::unordered_map<std::string, std::vector<DataPredicate>>& toIntersectAtAnUpperLevelRight,
                                                   bool isAlwaysFromTheSameEventAndIntersectMap
                                                   );


std::optional<std::unordered_map<std::string, DataPredicate>> instantiateWithValues(
                                                    const std::unordered_map<std::string, DataPredicate>& toGround,
                                                    const std::unordered_map<std::string, union_minimal> &mapL,
                                                    const std::unordered_map<std::string, union_minimal> &mapR,
                                                    bool isAlwaysFromTheSameEventAndIntersectMap
                                                    );

std::optional<std::vector<std::unordered_map<std::string, DataPredicate>>> instantiateWithValues(
                                                    const std::vector<std::unordered_map<std::string, DataPredicate>>& toGround,
                                                    const std::unordered_map<std::string, union_minimal> &mapL,
                                                    const std::unordered_map<std::string, union_minimal> &mapR,
                                                    bool isAlwaysFromTheSameEventAndIntersectMap
                                                    );*/

std::optional<DeclareDataAware> instantiateWithValues(
                                                    const DeclareDataAware& toGround,
                                                    const std::unordered_map<std::string, union_minimal>& mapLeft,
                                                    const std::unordered_map<std::string, union_minimal>& mapRight
                                                    );

#endif //KNOBAB_GROUNDING_H
