//
// Created by giacomo on 31/01/2020.
//

#ifndef TUTORIALS_RULE_H
#define TUTORIALS_RULE_H

#include <vector>
#include <string>
//#include <fptree.hpp>
#include <ostream>
#include <set>
/**
 * Subset relation among two vectors
 *
 * @tparam T    Types of the values associated to the vector
 * @param A     Left vector, the supposed superset
 * @param B     Right vector, the supposed subset
 * @return      Whether B is a subset of or equal to A
 */
template <typename T> bool IsSupsetOf(std::vector<T> A, std::vector<T> B) {
    std::sort(A.begin(), A.end());
    std::sort(B.begin(), B.end());
    return std::includes(A.begin(), A.end(), B.begin(), B.end());
}


/**
 * Represents a simple mined rule
 */
template <typename T> struct Rule {
    std::vector<T> head; ///< Represents the head of the rule
    std::vector<T> tail; ///< Represents the tail of the rule

    Rule() = default;
    Rule(const Rule<T>& rule) = default;
    Rule& operator=(const Rule& rule) = default;

    /**
     * Creating a rule from a vector having the head filled and the tail empty
     * @param itemset
     */
    Rule(const std::vector<T>& itemset) : head{itemset}, tail{} {}
    Rule(const std::set<T>& itemset) : tail{} {
        for (const auto& x : itemset) head.emplace_back(x);
        std::sort(head.begin(), head.end());
    }

    /**
     * Printing some rules in output
     * @param os
     * @param rule
     * @return
     */
    friend std::ostream &operator<<(std::ostream &os, const Rule &rule) {
        os << "{";
        for (size_t i = 0, N = rule.head.size(); i < N; i++) {
            os << rule.head[i];
            if (i != (N-1)) os << ", ";
        }
        os << "} => {";
        for (size_t i = 0, N = rule.tail.size(); i < N; i++) {
            os << rule.tail[i];
            if (i != (N-1)) os << ", ";
        }
        os << "}";
        return os;
    }

    /**
     * Comparison operator: using the formal definition of precedence!
     */
    bool operator<(const Rule &rhs) const {
        return (IsSupsetOf(tail, rhs.tail) && IsSupsetOf(rhs.head, head));
    }

    bool operator>(const Rule &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Rule &rhs) const {
        return (*this < rhs) || (*this == rhs);
    }

    bool operator>=(const Rule &rhs) const {
        return (*this > rhs) || (*this == rhs);
    }

    bool operator==(const Rule &rhs) const {
        return IsSupsetOf(head,rhs.head) && IsSupsetOf(rhs.head,head) &&
               IsSupsetOf(tail,rhs.tail) && IsSupsetOf(rhs.tail,tail);
    }

    bool operator!=(const Rule &rhs) const {
        return !(rhs == *this);
    }
};

namespace std {
    template<typename T>
    class hash<Rule<T>> {
    public:
        size_t operator()(const Rule<T> &s) const
        {
            size_t i = 31;
            size_t j = 7;
            for (const T& x : s.head) {
                i = std::hash<T>()(x) * 31 + i;
            }
            for (const T& x : s.tail) {
                j = std::hash<T>()(x) * 7 + i;
            }
            return i ^ ( j << 1 );
        }
    };
}



#endif //TUTORIALS_RULE_H
