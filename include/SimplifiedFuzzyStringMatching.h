//
// Created by giacomo on 01/11/2021.
//

#ifndef KNOBAB_SIMPLIFIEDFUZZYSTRINGMATCHING_H
#define KNOBAB_SIMPLIFIEDFUZZYSTRINGMATCHING_H

#include <string>
#include <yaucl/structures/any_to_uint_bimap.h>
#include <unordered_map>
#include <vector>
#include <yaucl/structures/PollMap.h>
#include <unordered_set>


class SimplifiedFuzzyStringMatching {
    static_assert(sizeof(short) == sizeof(char)*2, "Chars should be double the size than chars");
    yaucl::structures::any_to_uint_bimap<std::string> unique_string;
    std::unordered_map<std::string, std::vector<size_t>> shift0_2grams_containment;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> gram_multiplicity;
    std::unordered_map<std::string, size_t> objectGramSize;


public:
    std::pair<size_t, bool> put(const std::string& str);
    std::string get(size_t id) const;
    void clear() {
        shift0_2grams_containment.clear();
        gram_multiplicity.clear();
        objectGramSize.clear();
    }

    // TODO: import the code for the indexed approximate string matching
    void getTwoGramAndString(const std::string &argument, std::unordered_map<std::string , size_t> &map) const;

    void fuzzyMatch(double threshold,
                    size_t topk,
                    const std::string& objectString,
                    std::multimap<double, std::string> &result) const;

private:
    void rankCollectionOf(std::unordered_set<size_t> &k, std::unordered_map<std::string , size_t> &m1,
                          unsigned long size, double threshold,
                          yaucl::structures::PollMap<double, std::string> &pollMap) const;
};



#endif //KNOBAB_SIMPLIFIEDFUZZYSTRINGMATCHING_H
