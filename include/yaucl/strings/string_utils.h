//
// Created by giacomo on 01/11/2021.
//

#ifndef KNOBAB_STRING_UTILS_H
#define KNOBAB_STRING_UTILS_H

#include <string>

#include <algorithm>
#include <cctype>
#include <locale>
namespace yaucl {
    namespace strings {
        /** Compares two strings having different lengths
        *
        * @param value
        * @param len1
        * @param other
        * @param len2
        * @return
        */
        int strnmcmp(char* value, size_t len1, char * other, size_t len2);



        /**
 * Converts to uppercase a string that might be in unicode
 * @param src   Unicode raw string
 * @return
 */
        std::string utf8_tolower(const std::string &src);

        std::wstring utf8_towlower(const std::string &src);
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

        // trim from start (in place)
        //https://stackoverflow.com/a/217605
        static inline void ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
        }

// trim from end (in place)
        static inline void rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

// trim from both ends (in place)
        static inline void trim(std::string &s) {
            rtrim(s);
            ltrim(s);
        }

// trim from start (copying)
        static inline std::string ltrim_copy(std::string s) {
            ltrim(s);
            return s;
        }

// trim from end (copying)
        static inline std::string rtrim_copy(std::string s) {
            rtrim(s);
            return s;
        }

// trim from both ends (copying)
        static inline std::string trim_copy(std::string s) {
            trim(s);
            return s;
        }
    }
}

#endif //KNOBAB_STRING_UTILS_H
