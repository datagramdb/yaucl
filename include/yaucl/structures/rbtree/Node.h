//
// Created by Giacomo Bergami on 16/08/16.
//

#pragma once

#include <set>
#include "enums.h"
#include <memory>
#include <vector>


template<typename Key, typename Value> class Node {
public:
    Color color;
    bool isStack;
    PositionIteratorEnum visit = PositionIteratorEnum::None;
    std::vector<Value> overflowList;
    Node<Key,Value> *parent, *left, *right;
    Key key;

    //constructor
    Node(Key k, Value value) : Node(k,value,Color::RED,nullptr,nullptr) { }

    Node(Key k, Value value, Color nodeColor, Node<Key,Value>* l, Node<Key,Value>* r) :
            key{k},
            color{nodeColor},
            left((l)),
            right((r)),
            parent{nullptr} {
        overflowList.emplace_back(value);
        if (left) left->parent = this;
        if (right) right->parent = this;
    }

    Node(Key k, Color nodeColor, Node<Key,Value>* l, Node<Key,Value>* r) :
            key{k},
            color{nodeColor},
            left((l)),
            right((r)),
            parent{nullptr} {
        if (left) left->parent = this;
        if (right) right->parent = this;
    }

    //methods

    Node<Key,Value>* grandparent() {
        return parent->parent;
    }
    Node<Key,Value>*  sibling() {
        if (!parent) return nullptr;
        return  (this == parent->left ?  parent->right : parent->left);
    }
    Node<Key,Value>*  uncle() {
        return parent->sibling();
    }

    template <class... _Args> bool add(_Args&&... __args) {
        overflowList.emplace_back(std::forward<_Args>(__args)...);
        return true;
    }

    bool add(Value& value) {
        overflowList.push_back(value);
        return true;
    }
/*
    bool remove(Value value) {
        return overflowList.erase(value)==1;
    }*/

    int getSize() {
        return overflowList.size();
    }

    bool contains(Value value) {
        return overflowList.find(value) != overflowList.end();
    }

    bool hasNext() {
        return ((parent && parent->left==this)  // if I am a left child, I still have to visit the parent
                || (parent && parent->right==this &&
                    (    visit== PositionIteratorEnum::Left
                         ||  visit== PositionIteratorEnum::Center
                         || (visit== PositionIteratorEnum::Right && (right ||
                                                                     (((this->visit= PositionIteratorEnum::None)==PositionIteratorEnum::None) && parent->hasNext())))
                         || (this->visit== PositionIteratorEnum::None && parent->hasNext())))  //If I am a right child, either the siblings or the parent has still to be visited

                || (parent== nullptr && (visit== PositionIteratorEnum::Left || visit== PositionIteratorEnum::Center ||
                                         (visit== PositionIteratorEnum::Right && right)))
        ); // If the recursion meets the root, then I have to check if the root has been visited yet
    }


};

