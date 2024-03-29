//
// Created by giacomo on 01/11/2021.
//

#include <locale>
#include <codecvt>
#include "yaucl/strings/string_utils.h"
#include <algorithm>

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

