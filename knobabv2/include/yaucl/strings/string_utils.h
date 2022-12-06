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
    }
}

#endif //KNOBAB_STRING_UTILS_H
