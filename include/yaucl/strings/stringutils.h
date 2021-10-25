//
// Created by giacomo on 26/08/19.
//

#ifndef INCONSISTENCY_STRINGUITLS_H
#define INCONSISTENCY_STRINGUITLS_H

#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
extern "C" {
#include <string.h>
};
#include <sstream>

#include <nlohmann/json.hpp>
#define UNESCAPE(x)                  nlohmann::json::parse(x).get<std::string>()

namespace yaucl {
    namespace strings {

/**
 * Compares two strings having different lengths
 *
 * @param value
 * @param len1
 * @param other
 * @param len2
 * @return
 */
        int strnmcmp(char* value, size_t len1, char * other, size_t len2);


/**
 * Convert single character into lowercase
 * @param ch    UTF8 character
 * @return
 */
        wchar_t lowcase(wchar_t ch);

/**
 * Converts to uppercase a string that might be in unicode
 * @param src   Unicode raw string
 * @return
 */
        std::string utf8_tolower(const std::string &src);



        std::string ltrim(const std::string& s);
        std::string rtrim(const std::string& s);
        std::string trim(const std::string& s);
        int strpos(char *haystack, char *needle, int nth);

        std::string to_string(const std::string& value);

        extern  std::string slash;
        std::string rectify(std::string& x);
        std::string splitOnce(std::string& s, std::string& delimiter, int ith);
        std::string unrectify(std::string& x);
        std::string extract_basic_id(std::string& generic_id);

        std::vector<std::string> string_split_to_stringvector(const std::string& str, const std::string& delim);
        std::vector<size_t> string_split_to_sizetvector(const std::string &str, const std::string &delim = "_");
        std::string size_vector_to_string(const std::vector<size_t>& vector);
        std::vector<std::vector<size_t>> generateAllPossibleSubpaths(const std::vector<size_t>& x);;

        /* https://stackoverflow.com/a/24315631/1376095 */
        std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

        /* http://www.cplusplus.com/articles/D9j2Nwbp/ */
        template <typename T> std::string NumberToString (T Number ) {
            std::ostringstream ss;
            ss << Number;
            return ss.str();
        }

        template <typename T> T StringToNumber ( const std::string &Text )  {
            std::istringstream ss(Text);
            T result;
            return ss >> result ? result : 0;
        }

    }
}

#endif //INCONSISTENCY_STRINGUITLS_H
