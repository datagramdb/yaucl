/*
 * Node.h
 * This file is part of graphSampler
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * Created by Giacomo Bergami on 16/08/16.
 *
 * graphSampler is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * graphSampler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with graphSampler. If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once

#include <set>
#include "enums.h"
#include <memory>
#include <vector>
#include <algorithm>

namespace yaucl {
    namespace structures  {
        namespace rbtree {

/**
 * Defines the node of a RBtree. Each node contains a list of elements
 */
            template<typename Key, typename Value> class Node {
            public:
                Color color;
                PositionIteratorEnum visit = PositionIteratorEnum::None;
                std::vector<Value> overflowList;
                Node<Key,Value> *parent, *left, *right;
                Key key;
                bool isSampled;
                unsigned long sampledEdgesSize;

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
                    isSampled = false;
                    sampledEdgesSize = 0;
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

                /**
                 * If you want to insert a new element by calling its constructor, then this method instantiates the new
                 * object using such constructor
                 * @param __args    Parameters for the object to be inserted
                 * @return          true constant
                 */
                template <class... _Args> bool add(_Args&&... __args) {
                    overflowList.emplace_back(std::forward<_Args>(__args)...);
                    return true;
                }

                /**
                 * Adds one single value to the node
                 * @param value     Value to be inserted
                 * @return
                 */
                bool add(Value& value) {
                    overflowList.push_back(value);
                    return true;
                }
/*
    bool remove(Value value) {
        return overflowList.erase(value)==1;
    }*/

                /**
                 * The size of the node is the size of the elements it contains
                 * @return
                 */
                int getSize() {
                    return overflowList.size();
                }

                /**
                 * Checks if a specific value is contained within the node
                 * @param value
                 * @return
                 */
                bool contains(Value value) {
                    return std::find(overflowList.begin(), overflowList.end(),value) != overflowList.end();
                }

                /**
                 * Checking if there is an element next of the current one within the tree
                 * @return  If there actually is an element
                 */
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
        }
    }
}


