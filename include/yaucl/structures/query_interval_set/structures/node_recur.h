/*
 * node_recur.h
 * This file is part of bpm21
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 03/01/21.
//

#ifndef CUCCIOLO_NODE_RECUR_H
#define CUCCIOLO_NODE_RECUR_H

#include <yaucl/strings/serializers.h>
#include <yaucl/hashing/uset_hash.h>

template <typename T> struct node_recur {
    T min, max;         //<@ Storing the interval information
    std::vector<node_recur<T>> children, onlyForPointed;
    std::vector<size_t>        toErase; //<@ Indices to be removed from the children after moving those to onlyForPointed
    size_t isPointed;   //<@ This fields counts how many query intervals are requiring this step. This will define the interval priority

    node_recur(T min, T max) : min(min), max(max), isPointed(0) {}
    node_recur(): isPointed(0) {}
    DEFAULT_COPY_ASSGN(node_recur)

    bool operator<(const node_recur &rhs) const {
        if (min < rhs.min) return true;
        if (rhs.min < min) return false;
        return max < rhs.max;
    }
    bool operator==(const node_recur &rhs) const {
        return min == rhs.min && max == rhs.max;
    }
    bool operator>(const node_recur &rhs)  const { return rhs < *this;     }
    bool operator<=(const node_recur &rhs) const { return !(rhs < *this);  }
    bool operator>=(const node_recur &rhs) const { return !(*this < rhs);  }
    bool operator!=(const node_recur &rhs) const { return !(rhs == *this); }

    void collect_intervals(std::unordered_set<std::pair<T,T>>& S) const {
        if ((children.empty()) || isPointed) {
            if ((!onlyForPointed.empty())) {
                for (const node_recur<T>& ref : onlyForPointed)
                    ref.collect_intervals(S);
            } else {
                S.emplace(min, max);
            }
        }

        for (const node_recur<T>& ref : children)
            ref.collect_intervals(S);
    }

    template <typename PrevNext>
    void collect_intervals2(std::unordered_set<std::pair<T,T>>& S, const PrevNext& i) const {
        std::unordered_set<std::pair<T,T>> tmp;
        if ((children.empty()) || isPointed) {
            if ((!onlyForPointed.empty())) {
                for (const node_recur<T>& ref : onlyForPointed)
                    ref.collect_intervals2(tmp, i);
            } else {
                //S.emplace(min, max);
            }
        }

        for (const node_recur<T>& ref : children)
            ref.collect_intervals2(tmp, i);

        if (tmp.empty()) {
            S.emplace(min, max);
        } else {
            T mio_min = min, prevo = min;
            std::vector<std::pair<T,T>> S2;
            S2.insert(S2.end(), tmp.begin(), tmp.end());
            std::sort(S2.begin(), S2.end());
            for (const std::pair<T,T>& cp : S2) {
                auto prev = i.getPrev(cp.first);
                if ((mio_min < (cp.first)) && (mio_min < prev))
                    S.emplace(mio_min, prev);
                S.emplace(cp.first, cp.second);
                prevo = cp.second;
                mio_min = i.getNext(cp.second);
            }
            if ((mio_min <= max) && (prevo != mio_min))
                S.emplace(mio_min, max);
        }
    }

    void print(std::ostream &os, unsigned int pad = 0) const {
        for (size_t i = 0; i<pad; i++) os << "..";
        os << '[' << min << ',' << max << ']' << /*(children.empty() ? " **" : "") << */(isPointed ? ("<" + std::to_string(isPointed) +"<") : "");
        if (!onlyForPointed.empty()) os << ':' << onlyForPointed;
        os << std::endl;
        for (size_t i = 0, N = children.size(); i<N; i++)
            children.at(i).print(os, pad+1);
    }

    friend std::ostream &operator<<(std::ostream &os, const node_recur &recur) {
        os << "{" << recur.min << "," << recur.max << "}";
        return os;
    }
};

template<typename T> std::ostream& printVector(std::ostream &os, const std::vector<std::pair<struct node_recur<T>*,size_t>> &insertion) {
    os << '{';
    for (size_t i = 0, N = insertion.size(); i<N; i++) {
        os << insertion.at(i).first->children.at(insertion.at(i).second);
        if (i < (N-1)) os << ", ";
    }
    return os << '}';
}


#endif //CUCCIOLO_NODE_RECUR_H
