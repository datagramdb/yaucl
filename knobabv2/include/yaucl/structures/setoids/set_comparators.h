//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_SET_COMPARATORS_H
#define KNOBAB_SERVER_SET_COMPARATORS_H

#include <set>
#include <algorithm>

template <typename T> struct set_lesseq {
    bool operator()(std::set<T> const &a, std::set<T> const &b) const {
        return (a!=b) && std::includes(b.begin(), b.end(), a.begin(), a.end());
    }
};

template <typename T> struct set_less {
    bool operator()(std::set<T> const & a, std::set<T> const &b) const {
        return (a!=b) && std::includes(b.begin(), b.end(), a.begin(), a.end());
    }
};

#endif //KNOBAB_SERVER_SET_COMPARATORS_H
