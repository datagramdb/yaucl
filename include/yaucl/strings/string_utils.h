//
// Created by giacomo on 01/11/2021.
//

#ifndef KNOBAB_STRING_UTILS_H
#define KNOBAB_STRING_UTILS_H

#include <string>

namespace yaucl {
    namespace strings {
        /**
 * Converts to uppercase a string that might be in unicode
 * @param src   Unicode raw string
 * @return
 */
        std::string utf8_tolower(const std::string &src);
        wchar_t  lowcase(wchar_t ch);

        static inline bool is_number(const std::string& s)
        {
            return !s.empty() && std::find_if(s.begin(),
                                              s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
        }
    }
}

#endif //KNOBAB_STRING_UTILS_H
