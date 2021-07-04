/*
 * utils.cpp
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */
//
// Created by giacomo on 17/10/18.
//
//
// Created by giacomo on 26/08/19.
//

#include <yaucl/strings/stringutils.h>
#include <string>
#include <locale> //wstring_convert
#include <algorithm>
#include <codecvt> //codecvt_utf8
#include <streambuf>
#include <regex>

using namespace yaucl::strings;

int yaucl::strings::strnmcmp(char *value, size_t len1, char *other, size_t len2) {
    int lim = len1 < len2 ? len1 : len2;
    for (int k = 0; k < lim; k++) {
        char c1 = value[k];
        char c2 = other[k];
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return len1 - len2;
}



wchar_t yaucl::strings::lowcase(wchar_t ch) {
    static auto& t = std::use_facet<std::ctype<wchar_t>>(std::locale());
    return t.tolower( ch );
}

std::string yaucl::strings::utf8_tolower(const std::string &src) {
    static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
    std::wstring wsTmp(src.begin(), src.end());
    std::wstring result;
    std::transform( src.begin(), src.end(), std::back_inserter( result ), lowcase );
    return converterX.to_bytes(result);
}









/**
 * Provides the expected representation for the string in Concepts
 *
 * @param x
 * @return
 */
std::string yaucl::strings::rectify(std::string& x) {
    static std::regex spaceRegex("\\s");
    std::string s{x};
    std::regex_replace (s, spaceRegex, "_");
    std::replace( s.begin(), s.end(), '-', '_');
    return s;
}

/**
 * This function allows to traverse a string and get the i-th substring separated by a given delimiter
 * without the need of actually splitting the string. In this way we perform at most a linear scan
 * of the string and we save primary memory space.
 *
 * @param s             String to be tokenized according to the delimiter
 * @param delimiter     Delimiter separating the tokens
 * @param ith           Selecting the i-th token that is separated from the delimiter
 * @return              Returns the token if it has been found, or an empty string otherwise. Important:
 *                      the semantic assumes that the to-be-splitted string shall never contain empty tokens
 */
std::string yaucl::strings::splitOnce(std::string& s, std::string& delimiter, int ith) {
    size_t next = 0, pos = 0, prev = 0, count = 0, c = 0;
    size_t len = delimiter.length();
    while ((next = s.find(delimiter, pos)) != std::string::npos && count < ith) {
        count++;
        prev = pos;
        pos = s.find(delimiter, next+len);
    }
    c = (s.find(delimiter, pos+1));
    bool finalIsNotEnd = c != std::string::npos;
    bool begIsNotEnd = pos != std::string::npos;
    std::string toret = next == s.length() ? "" :
                        s.substr((begIsNotEnd ? pos+1 : prev+len),(finalIsNotEnd && begIsNotEnd ? c-pos-1 : s.length()));
    return toret;
}

std::string yaucl::strings::unrectify(std::string& x) {
    static std::string slash{"/"};
    std::string s{x};
    if (s.rfind("/c/", 0) == 0) {
        std::string split = splitOnce(x, slash, 2);
        return unrectify(split);
    } else {
        std::replace( s.begin(), s.end(), '_', ' ');
        return s;
    }
}

std::string yaucl::strings::extract_basic_id(std::string &generic_id) {
    static std::string slash{"/"};
    long pos = strpos((char*)generic_id.c_str(), (char*)slash.c_str(),4);
    if (pos >=  0) {
        return generic_id.substr(0, pos);
    } else {
        return generic_id;
    }
}


/*
 * trimmers.cpp
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 11/12/18.
//

std::string yaucl::strings::ltrim(const std::string &s) {
    static std::string WHITESPACE = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string yaucl::strings::rtrim(const std::string &s) {
    static std::string WHITESPACE = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string yaucl::strings::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

int yaucl::strings::strpos(char *haystack, char *needle, int nth) {
    char *res = haystack;
    for(int i = 1; i <= nth; i++)
    {
        res = strstr(res, needle);
        if (!res)
            return -1;
        else if(i != nth)
            res++;
    }
    return res - haystack;
}

std::string yaucl::strings::to_string(const std::string &value) {
    return value;
}


//
// Created by giacomo on 29/12/19.
//


std::vector<std::string> yaucl::strings::string_split_to_stringvector(const std::string &str, const std::string &delim) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

std::vector<size_t> yaucl::strings::string_split_to_sizetvector(const std::string &str, const std::string &delim) {
    std::vector<size_t> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(std::stoull(token));
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

#include <string>
#include <yaucl/strings/StringBuilder.h>

std::string yaucl::strings::size_vector_to_string(const std::vector<size_t> &vector) {
    yaucl::strings::StringBuilder<char> sb;
    static const std::string underscore{"_"};
    for (const size_t& x : vector)
        sb.Append(std::to_string(x));
    return sb.Join(underscore);
}

std::vector<std::vector<size_t>> yaucl::strings::generateAllPossibleSubpaths(const std::vector<size_t> &x) {
    std::vector<std::vector<size_t>> toReturn;
    toReturn.emplace_back(std::vector<size_t>{});
    for (size_t i = 1, n = x.size(); i<=n; i++) {
        toReturn.emplace_back(std::vector<size_t>(x.begin(), x.begin()+i));
    }
    return toReturn;
}
