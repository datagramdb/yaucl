//
// Created by giacomo on 31/01/2020.
//

#ifndef TUTORIALS_LEXICOGRAPHICALORDER_H
#define TUTORIALS_LEXICOGRAPHICALORDER_H

#include <functional>

#define ForAll2(T,U)       template <typename T, typename U>

/**
 * Defining an ordering over vectors following the lexicographical order
 * @tparam T    Type associated to the container
 * @tparam U    Type associated to the content
 */
ForAll2(T,U) struct LexicographicalOrder {
    std::less<U> lesscmp;   // Default comparator for the content
    bool operator()(const T& lhs, const T& rhs) const {
        return compare(lhs, rhs, 0);
    }
private:
    bool compare(const T& lhs, const T& rhs, size_t idx) const {
        if (idx == lhs.size()) {
            return !(idx == rhs.size());
        } else {
            if (idx == rhs.size()) {
                return false;
            } else {
                if (lesscmp(lhs[idx], rhs[idx])) {
                    return true;
                } else if (lesscmp(rhs[idx], lhs[idx])) {
                    return false;
                } else {
                    return compare(lhs, rhs, idx+1);
                }
            }
        }
    }
};


#endif //TUTORIALS_LEXICOGRAPHICALORDER_H
