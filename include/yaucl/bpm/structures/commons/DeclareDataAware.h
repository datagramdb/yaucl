//
// Created by giacomo on 06/12/22.
//

#ifndef AIRLINE_DECLAREDATAAWARE_H
#define AIRLINE_DECLAREDATAAWARE_H

#include <string>
#include "DataPredicate.h"
#include "roaring64map.hh"

using conjoint_data_condition = std::unordered_map<std::string, DataPredicate>;
using data_conditions = std::vector<conjoint_data_condition>; // Under disjunction

template <>
struct std::hash<std::unordered_map<std::string, DataPredicate>>
{
    std::size_t operator()(const std::unordered_map<std::string,DataPredicate>& k) const
    {
        std::hash<std::string> thash;
        std::hash<DataPredicate> khash;
        size_t init = 31;
        for (const auto& x : k) init += thash(x.first) ^ khash(x.second);
        return init;
    }
};


#include "yaucl/hashing/umap_hash.h"
#include "yaucl/hashing/vector_hash.h"


struct SimpleDeclareDataAware {
    std::string template_name;
    size_t n;
    std::string left_act;
    std::string right_act;
    data_conditions dnf_left_map, dnf_right_map, conjunctive_map;

    roaring::Roaring64Map left_decomposed_atoms, right_decomposed_atoms;
    DEFAULT_CONSTRUCTORS(SimpleDeclareDataAware)

    bool operator==(const SimpleDeclareDataAware &rhs) const;
    bool operator!=(const SimpleDeclareDataAware &rhs) const;
};

namespace std {
    template <>
    struct hash<SimpleDeclareDataAware> {
        std::size_t operator()(const SimpleDeclareDataAware& k) const {
            using yaucl::hashing::hash_combine;
            size_t seed = 13;
            seed = hash_combine(seed, k.template_name);
            seed = hash_combine(seed, k.n);
            seed = hash_combine(seed, k.left_act);
            seed = hash_combine(seed, k.right_act);
            seed = hash_combine(seed, k.dnf_left_map);
            seed = hash_combine(seed, k.dnf_right_map);
            return seed;
        }
    };

}


#endif //AIRLINE_DECLAREDATAAWARE_H
