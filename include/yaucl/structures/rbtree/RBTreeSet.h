//
// Created by Giacomo Bergami on 18/08/16.
//

#pragma once

#include "rbtree.h"

template <typename T> class RBTreeSet {
    RBTree<T,void*> t;
public:

    RBTreeSet(void(*ptr)(T,T,int*)) : t{ptr} {}

    void add(T val) {
        t.insertKey(val);
    }

    bool contains(T val) {
        return t.lookupNode(val)!=nullptr;
    }

};

