//
// Created by giacomo on 01/11/2021.
//

#ifndef KNOBAB_ANY_TO_UINT_BIMAP_H
#define KNOBAB_ANY_TO_UINT_BIMAP_H

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <yaucl/functional/assert.h>

namespace yaucl {
    namespace structures {

        template<typename T>  struct any_to_uint_bimap {

            std::vector<T> int_to_T;
            std::unordered_map<T, size_t> T_to_int;


            any_to_uint_bimap() = default;
            any_to_uint_bimap(const any_to_uint_bimap&) = default;
            any_to_uint_bimap(any_to_uint_bimap&&) = default;
            any_to_uint_bimap&operator=(const any_to_uint_bimap&) = default;
            any_to_uint_bimap&operator=(any_to_uint_bimap&&) = default;

            void clear() {
                int_to_T.clear();
                T_to_int.clear();
            }

            std::pair<size_t, bool> put(const T& elem) {
                auto it = T_to_int.find(elem);
                if (it != T_to_int.end())
                    return { it->second, false };
                else {
                    size_t toRet = int_to_T.size();
                    T_to_int.emplace(elem, toRet);
                    int_to_T.emplace_back(elem);
                    return { toRet, true };
                }
            }
            size_t get(const T& elem) const {
                auto it = T_to_int.find(elem);
                if (it != T_to_int.end())
                    return it->second;
                else {
                    throw std::runtime_error("Unkown key");
                }
            }
            const T&  get(size_t elem) const {
                DEBUG_ASSERT(int_to_T.size() > elem);
                return int_to_T.at(elem);
            }
        };
    }
}



#endif //KNOBAB_ANY_TO_UINT_BIMAP_H
