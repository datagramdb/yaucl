//
// Created by giacomo on 25/11/22.
//

#ifndef KNOBAB_SERVER_SET_COMPARE_H
#define KNOBAB_SERVER_SET_COMPARE_H

#include <set>
#include <algorithm>

enum QueryRelation {
    QR_LE,
    QR_LEQ,
    QR_EQ,
    QR_GEQ,
    QR_GE
};

template <typename T> bool set_compare(const std::set<T>& l, const std::set<T>& r, QueryRelation casus) {
    switch (casus) {
        case QR_LE:
            return (l!=r) && (std::includes(r.begin(), r.end(), l.begin(), l.end()));

        case QR_LEQ:
            return (std::includes(r.begin(), r.end(), l.begin(), l.end()));

        case QR_EQ:
            return l == r;

        case QR_GEQ:
            return (std::includes(l.begin(), l.end(), r.begin(), r.end()));

        case QR_GE:
            return (l!=r) && (std::includes(l.begin(), l.end(), r.begin(), r.end()));
    }
}

static inline bool int_compare(size_t l, size_t r, QueryRelation casus) {
    switch (casus) {
        case QR_LE:
            return l<r;

        case QR_LEQ:
            return l<=r;

        case QR_EQ:
            return l == r;

        case QR_GEQ:
            return l>=r;

        case QR_GE:
            return l>r;
    }
}

template <typename Iterator> std::pair<Iterator,Iterator> iterator_resolve(Iterator beg,
                                                                           Iterator end,
                                                                           Iterator eqIter,
                                                                           QueryRelation casus){
    switch (casus) {
        case QR_LE:
            return {beg, eqIter};

        case QR_LEQ:
            eqIter++;
            return {beg, eqIter};

        case QR_EQ: {
            auto it = eqIter;
            it++;
            return {eqIter, it};
        }

        case QR_GEQ:
            return {eqIter, end};

        case QR_GE: {
            auto it = eqIter;
            it++;
            return {eqIter, end};
        }
    }
}

#endif //KNOBAB_SERVER_SET_COMPARE_H
