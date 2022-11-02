//
// Created by giacomo on 08/06/2022.
//

#ifndef KNOBAB_SERVER_NDPFUZZYSTRINGMATCHING_H
#define KNOBAB_SERVER_NDPFUZZYSTRINGMATCHING_H

#include <yaucl/data/VariadicSizeArrayElements.h>
#include <yaucl/structures/any_to_uint_bimap.h>
#include <yaucl/data/FixedSizeNDPSorter.h>
#include <yaucl/structures/PollMap.h>
#include <map>
#include <yaucl/structures/bit_tagged_unions.h>

PACK(struct string_gram_frequency {
    char two_gram[2];
    size_t string_id;
    size_t frequency;

    string_gram_frequency();
    string_gram_frequency(const string_gram_frequency& ) = default;
    string_gram_frequency(string_gram_frequency&& ) = default;
    string_gram_frequency& operator=(const string_gram_frequency& ) = default;
    string_gram_frequency& operator=(string_gram_frequency&& ) = default;
    bool operator<(const string_gram_frequency &rhs) const;
    bool operator>(const string_gram_frequency &rhs) const;
    bool operator<=(const string_gram_frequency &rhs) const;
    bool operator>=(const string_gram_frequency &rhs) const;
});

PACK(struct gram_to_string_id {
    char two_gram[2];
    size_t string_id;

    gram_to_string_id();
    gram_to_string_id(const gram_to_string_id& ) = default;
    gram_to_string_id(gram_to_string_id&& ) = default;
    gram_to_string_id& operator=(const gram_to_string_id& ) = default;
    gram_to_string_id& operator=(gram_to_string_id&& ) = default;
    bool operator<(const gram_to_string_id &rhs) const;
    bool operator>(const gram_to_string_id &rhs) const;
    bool operator<=(const gram_to_string_id &rhs) const;
    bool operator>=(const gram_to_string_id &rhs) const;
});

/***
 * Warning! This class does not support to add strings at runtime, after something has been already written
 * in memory This should allow to open the files in append mode, and guarantee that indices are re-written
 * as expected
 */
class NDPFuzzyStringMatching {
    yaucl::data::VariadicSizeArrayElementsReaderWriter strings;
    yaucl::structures::any_to_uint_bimap<std::string> unique_string;
    FixedSizeReaderWriter<std::pair<size_t, size_t>> string_id_to_number_2gram;
    FixedSizeReaderWriter<string_gram_frequency> string_id_to_gram_frequency;
    FixedSizeReaderWriter<size_t> string_id_to_gram_frequency_primary;
    FixedSizeReaderWriter<gram_to_string_id> gram_to_string_id_multimap, gram_to_string_id_multimap_primary;
    std::filesystem::path path;
    std::map<std::string, size_t> gramsInStringCount;
    bool closed;
    size_t incrElement;
    bool newlyInputtedData;

    void rankCollectionOf(std::unordered_set<size_t> &k, std::map<std::string , size_t> &m1,
                          unsigned long size, double threshold,
                          yaucl::structures::PollMap<double, std::string> &pollMap);
    void getTwoGramAndString(size_t string_id,
                             std::map<std::string, size_t> &map) ;

public:
    NDPFuzzyStringMatching(const std::filesystem::path& path): path{path} {
        newlyInputtedData = false;
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);
        open();
    }

    NDPFuzzyStringMatching() { newlyInputtedData = false; }
    NDPFuzzyStringMatching(const NDPFuzzyStringMatching&) = default;
    NDPFuzzyStringMatching(NDPFuzzyStringMatching&&) = default;
    NDPFuzzyStringMatching& operator=(const NDPFuzzyStringMatching&) = default;
    NDPFuzzyStringMatching& operator=(NDPFuzzyStringMatching&&) = default;

    /**
     * Opens a string repository contained within a given folder
     * @param p
     */
    void open(const std::filesystem::path& p) {
        close();
        if (!std::filesystem::exists(p))
            std::filesystem::create_directories(p);
        this->path = p;
        open();
    }

    /**
     * Opens the path that was either previously provided or given
     * in the constructor
     */
    void open();

    /**
     * Returns the i-th string contained in such a folder, under the
     * assumption that all of the possible strings fit in primary memory
     * @param i
     * @return
     */
    std::string get(size_t i) const;

    /**
     * Adds a string to the repository
     * @param str   The string to be added
     * @return Returns a pair, where the first element returns the unique
     *         id associated to the string, and the second one returns whether
     *         the element already existed or not
     */
    std::pair<size_t, bool> put(const std::string &str);
    /**
     * Closes the files, if opened
     */
    void close();
    /**
     * Performs the fuzzy string maching
     * @param threshold     Minimum threshold for the match
     * @param topk          Maximum number of unique scores associated to the string that we tolerate
     * @param objectString  String to be approximately matched
     * @param result        Association between the score and the string of interest
     */
    void fuzzyMatch(double threshold, size_t topk, const std::string &objectString,
                                                   std::multimap<double, std::string> &result);
};


#endif //KNOBAB_SERVER_NDPFUZZYSTRINGMATCHING_H
