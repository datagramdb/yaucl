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

        /// Returns a version of 'str' where every occurrence of
/// 'find' is substituted by 'replace'.
/// - Inspired by James Kanze.
/// - http://stackoverflow.com/questions/20406744/
        static inline std::string replace_all(
                const std::string & str ,   // where to work
                const std::string & find ,  // substitute 'find'
                const std::string & replace //      by 'replace'
        ) {
            using namespace std;
            string result;
            size_t find_len = find.size();
            size_t pos,from=0;
            while ( string::npos != ( pos=str.find(find,from) ) ) {
                result.append( str, from, pos-from );
                result.append( replace );
                from = pos + find_len;
            }
            result.append( str, from , string::npos );
            return result;
/*
    This code might be an improvement to James Kanze's
    because it uses std::string methods instead of
    general algorithms [as 'std::search()'].
*/
        }
    }
}

#endif //KNOBAB_STRING_UTILS_H
