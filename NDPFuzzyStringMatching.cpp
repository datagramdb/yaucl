//
// Created by giacomo on 08/06/2022.
//

#include "knobab/server/query_manager/NDPFuzzyStringMatching.h"
#include <sstream>
#include <map>
#include <cstring>
#include <yaucl/data/mmapFile.h>
#include <SimplifiedFuzzyStringMatching.h>


static inline void compareStringHashmap1_n(const std::string& str,
                                         std::map<std::string, size_t> &map,
                                         std::vector<size_t> &vec) {
    size_t numPairs = str.length() - 1;
    if (numPairs == 0) {
        map[str] = 0;
        vec.emplace_back(1);
    } else {
        if (numPairs < 0) numPairs = 0;
        int singleGrams = 0;
        for (int i = 0; i < numPairs; i++) {
            std::string s{(str.substr(i, 2))};

            if (!s.empty()) {
                auto pos = map.find(s);
                if (pos == map.end()) {
                    map[s] = singleGrams++;
                    vec.emplace_back(1);
                } else {
                    vec[pos->second]++;
                }
            }
        }
    }
}

static inline void compareStringHashmap2_n(const std::string &string,
                                         std::map<std::string, size_t> &map,

                                         std::vector<size_t> &vec) {
    {
        std::string str;
        std::stringstream ss(string);
        std::vector<std::string> pairs;

        // Getting the grams within each word, thus avoiding to create 2grams with a space
        while (ss >> str) {
            compareStringHashmap1_n(str, map, vec);
        }
    }
    // Updating the gram count with each elements' output
    for (auto & kp : map) {
        kp.second = vec[kp.second];
    }
}

static inline void compareStringHashMap_n(const std::string& str, std::map<std::string, size_t> & retMap) {
    std::vector<size_t> retList{};
    compareStringHashmap2_n(str, retMap, retList);
}

std::pair<size_t, bool> NDPFuzzyStringMatching::put(const std::string &str) {
    static std::pair<size_t, size_t> objectGramSize;
    static gram_to_string_id pair1;
    static string_gram_frequency freq;
    static new_iovec memo;
    if (this->closed) open();
    auto it = unique_string.put(str);
    if (it.second) {
        newlyInputtedData = true;
        memo.iov_base = (void*)str.c_str();
        memo.iov_len = str.size()+1;
        strings.put(memo);
        objectGramSize.first = freq.string_id = pair1.string_id = it.first;
        // Pre-filling and ordering in primary memory, so this is not required to be done in secondary
        std::map<std::string, size_t> m;
        std::vector<size_t> vec;
        compareStringHashmap2_n(str, m, vec);

        for (const auto& cp : m) {
            auto it = gramsInStringCount.emplace(cp.first, 1);
            if (!it.second)
                it.first->second++;
            if (cp.first.empty()) {
                memset((void*)pair1.two_gram, 0, 2);
            } else {
                size_t N = cp.first.size();
                pair1.two_gram[0] = cp.first.at(0);
                if (N == 2)
                    pair1.two_gram[1] = cp.first.at(1);
                else
                    pair1.two_gram[1] = '\0';
            }
            freq.frequency = cp.second;
            memcpy(freq.two_gram, pair1.two_gram, 2);
            gram_to_string_id_multimap.put(pair1);
            string_id_to_gram_frequency.put(freq);
        }

        string_id_to_gram_frequency_primary.put(incrElement);
        incrElement += m.size();

        objectGramSize.second  = 0;
        for (size_t& j : vec) {
            objectGramSize.second  += j;
        }
        string_id_to_number_2gram.put(objectGramSize);
    }
    return it;
}

void NDPFuzzyStringMatching::close() {
    static gram_to_string_id pair1;
    if (newlyInputtedData) {
        gram_to_string_id_multimap.sort(availableMemory() / 4);
        pair1.string_id = 0;
        auto gram_to_string_idmultimap_primary = path / "gram_to_string_idmultimap.bin_primary";
        gram_to_string_id_multimap_primary.open(gram_to_string_idmultimap_primary, std::filesystem::temp_directory_path());
        for (const auto& cp : this->gramsInStringCount) {
            if (cp.first.empty()) {
                memset((void*)pair1.two_gram, 0, 2);
            } else {
                size_t N = cp.first.size();
                pair1.two_gram[0] = cp.first.at(0);
                if (N == 2)
                    pair1.two_gram[1] = cp.first.at(1);
                else
                    pair1.two_gram[1] = '\0';
            }
            gram_to_string_id_multimap_primary.put(pair1);
            pair1.string_id += cp.second;
        }
        // gram_to_string_id_multimap_primary already sorted in primary memory!
        gram_to_string_id_multimap_primary.close();
    }
    strings.close();
    string_id_to_number_2gram.close();
    string_id_to_gram_frequency.close();
    string_id_to_gram_frequency_primary.close();
    gram_to_string_id_multimap.close();
    gram_to_string_id_multimap_primary.close();
    closed = true;
    incrElement = 0;
}

void NDPFuzzyStringMatching::open() {
    close();
    auto strings_path = path / "strings.bin";
    if (std::filesystem::exists(strings_path)) {
        strings.open(strings_path);
        for (size_t i = 0, N = strings.size(); i<N; i++) {
            std::string local{strings.get<char>(i), strings.representation_size(i)-1};
            unique_string.put(local);
        }
    } else {
        strings.open(strings_path);
    }
    auto strings_to_n_2grams = path / "strings_to_n_2grams.bin";
    string_id_to_number_2gram.open(strings_to_n_2grams, std::filesystem::temp_directory_path());
    auto strings_to_n_2gramsF = path / "strings_to_n_2gramsF.bin";
    string_id_to_gram_frequency.open(strings_to_n_2gramsF , std::filesystem::temp_directory_path());
    auto strings_to_n_2gramsF_primary = path / "strings_to_n_2gramsF.bin_primary";
    string_id_to_gram_frequency_primary.open(strings_to_n_2gramsF_primary, std::filesystem::temp_directory_path());
    auto gram_to_string_idmultimap = path / "gram_to_string_idmultimap.bin";
    auto gram_to_string_idmultimap_primary = path / "gram_to_string_idmultimap.bin_primary";
    if (std::filesystem::exists(gram_to_string_idmultimap_primary) && std::filesystem::exists(gram_to_string_idmultimap)) {
        gram_to_string_id_multimap_primary.open(gram_to_string_idmultimap_primary, std::filesystem::temp_directory_path());
        std::string gram;
        size_t prev = 0;
        gram_to_string_id_multimap.open(gram_to_string_idmultimap, std::filesystem::temp_directory_path());
        incrElement = gram_to_string_id_multimap.size();
        for (size_t i = 0, N = gram_to_string_id_multimap_primary.size(); i<N; i++) {
            auto& ref = gram_to_string_id_multimap_primary.get(i);
            if (i > 0) {
                gramsInStringCount[gram] = ref.string_id - prev;
            }
            gram = std::string(ref.two_gram, 2);
            prev = ref.string_id;
        }
        gramsInStringCount[gram] = incrElement - prev;
    } else {
        gram_to_string_id_multimap.open(gram_to_string_idmultimap, std::filesystem::temp_directory_path());
        gram_to_string_id_multimap_primary.open(gram_to_string_idmultimap_primary, std::filesystem::temp_directory_path());
    }
    closed = false;
}

void NDPFuzzyStringMatching::rankCollectionOf(std::unordered_set<size_t> &k,
                                                     std::map<std::string, size_t> &m1, unsigned long size,
                                                     double threshold,
                                                     yaucl::structures::PollMap<double, std::string> &pollMap) {
    for (size_t itemId : k) {
        std::string associatedToElement = get(itemId);
        std::map<std::string , size_t> m2;
        getTwoGramAndString(itemId, m2);

        // Set intersection of the keys. Intersection while iterating
        double score = 0;
        double e = 0;
        std::map<std::string , size_t>* lesser = m1.size() < m2.size() ? &m1 : &m2;
        std::map<std::string , size_t>* greater = m1.size() < m2.size() ? &m2 : &m1;
        for (auto it = lesser->begin(); it != lesser->cend(); it++) {
            auto it2 = greater->find(it->first);
            if (it2 != greater->cend()) {
                // key is in both elements
                e += std::min(it->second, it2->second)*1.0;
            }
        }
        // no memoization
        ////void *ls = objectGramSize.find(associatedToElement);
        double leftCount = 0;
        //if (ls == nullptr) {
        std::map<std::string , size_t> retMap;
        std::vector<size_t> retList;
        compareStringHashmap2_n(associatedToElement, retMap, retList);
        for (size_t& x : retList) leftCount+=x;
        score = (e * 2.0) / ((leftCount + size)*1.0);

        if (threshold >= 0 && score >= threshold) {
            pollMap.add(score, associatedToElement);
        }
    }
}


void NDPFuzzyStringMatching::fuzzyMatch(double threshold, size_t topk, const std::string &objectString,
                                        std::multimap<double, std::string> &result) {
    close();
    open();
    yaucl::structures::PollMap<double, std::string> toReturnTop{topk};
    std::vector<std::string> objectGrams = compareString_wordLetterPairs(objectString);
    std::unordered_set<size_t> candidates{};
    std::map<std::string , size_t> m1;
    gram_to_string_id cpFind;
    cpFind.string_id = 0;

    // obtain all the objects that are assoicated to the gram that are within the current objectString in objectStrings
    for (const std::string& gram : objectGrams) {
        if (gram.empty()) {
            memset((void*)cpFind.two_gram, 0, 2);
        } else {
            size_t N = gram.size();
            cpFind.two_gram[0] = gram.at(0);
            if (N == 2)
                cpFind.two_gram[1] = gram.at(1);
            else
                cpFind.two_gram[1] = '\0';
        }
        auto beg = gram_to_string_id_multimap.begin();
        auto end = gram_to_string_id_multimap.end();
        auto it = std::lower_bound(beg,
                                   end,
                           cpFind);
        while ((it != end) && (it->two_gram[0] == cpFind.two_gram[0]) && (it->two_gram[1] == cpFind.two_gram[1])) {
            candidates.insert(it->string_id);
            it++;
        }
    }

    // return the number of the occurences of the twoGrams within the current string
    compareStringHashMap_n(objectString, m1);
    size_t ogSize = 0;
    for (auto it = m1.begin(); it != m1.cend(); it++) ogSize += it->second;

    // Rank the similarity between all the candidates
    rankCollectionOf(candidates, m1, ogSize, threshold, toReturnTop);

    // providing the final result
    toReturnTop.getPoll(result);
}

void NDPFuzzyStringMatching::getTwoGramAndString(size_t string_id,
                                                 std::map<std::string, size_t> &map)  {
    size_t offsetBegin = string_id_to_gram_frequency_primary.get(string_id);
    size_t offsetEnd;
    if (string_id_to_gram_frequency_primary.size() -1 == string_id) {
        offsetEnd = string_id_to_gram_frequency.size();
    } else {
        offsetEnd = string_id_to_gram_frequency_primary.get(string_id+1);
    }
    for (; offsetBegin < offsetEnd; offsetBegin++) {
        auto& ref = string_id_to_gram_frequency.get(offsetBegin);
        DEBUG_ASSERT(ref.string_id == string_id);
        map[std::string(ref.two_gram,2)] = ref.frequency;
    }
}

std::string NDPFuzzyStringMatching::get(size_t i) const {
    return unique_string.get(i);
}

gram_to_string_id::gram_to_string_id() : two_gram{'\0','\0'}, string_id{0} {}

bool gram_to_string_id::operator<(const gram_to_string_id &rhs) const {
    if (two_gram[0] < rhs.two_gram[0])
        return true;
    if (two_gram[0] != rhs.two_gram[0])
        return false;
    if (two_gram[1] < rhs.two_gram[1])
        return true;
    if (two_gram[1] != rhs.two_gram[1])
        return false;
    return string_id < rhs.string_id;
}

bool gram_to_string_id::operator>(const gram_to_string_id &rhs) const {
    return rhs < *this;
}

bool gram_to_string_id::operator<=(const gram_to_string_id &rhs) const {
    return !(rhs < *this);
}

bool gram_to_string_id::operator>=(const gram_to_string_id &rhs) const {
    return !(*this < rhs);
}

string_gram_frequency::string_gram_frequency() : two_gram{'\0','\0'}, string_id{0}, frequency{0} {}

bool string_gram_frequency::operator<(const string_gram_frequency &rhs) const {
    if (two_gram[0] < rhs.two_gram[0])
        return true;
    if (two_gram[0] != rhs.two_gram[0])
        return false;
    if (two_gram[1] < rhs.two_gram[1])
        return true;
    if (two_gram[1] != rhs.two_gram[1])
        return false;
    if (string_id < rhs.string_id)
        return true;
    if (rhs.string_id < string_id)
        return false;
    return frequency < rhs.frequency;
}

bool string_gram_frequency::operator>(const string_gram_frequency &rhs) const {
    return rhs < *this;
}

bool string_gram_frequency::operator<=(const string_gram_frequency &rhs) const {
    return !(rhs < *this);
}

bool string_gram_frequency::operator>=(const string_gram_frequency &rhs) const {
    return !(*this < rhs);
}
