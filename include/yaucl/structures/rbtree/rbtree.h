//
// THe source code is courtesy of
// http://cplusplus.kurttest.com/notes/llrb.html
//

#pragma once

#include "Node.h"

namespace yaucl {
    namespace structures {
        namespace rbtree {

#define NODECOLOR(n) ((!(n)) ? Color::BLACK : ((n)->color));

            template <typename K, typename V> class RBTree {



                //// INSERTION
                void insertCase1(Node<K,V>* n) {
                    if (!(n->parent))
                        n->color = Color::BLACK;
                    else
                        insertCase2(n);
                }
                void insertCase2(Node<K,V>* n) {
                    Color c = NODECOLOR(n->parent);
                    if (c == Color::BLACK)
                        return; // Tree is still valid
                    else insertCase3(n);
                }
                void insertCase3(Node<K,V>* n) {
                    Color c = NODECOLOR(n->uncle());
                    if (c == Color::RED) {
                        n->parent->color = Color::BLACK;
                        n->uncle()->color = Color::BLACK;
                        n->grandparent()->color = Color::RED;
                        insertCase1(n->grandparent());
                    } else {
                        insertCase4(n);
                    }
                }
                void insertCase4(Node<K,V>* n) {
                    if ((n == n->parent->right) &&
                        (n->parent == n->grandparent()->left)) {
                        rotateLeft(n->parent);
                        n = n->left;
                    } else if (n == n->parent->left && n->parent == n->grandparent()->right) {
                        rotateRight(n->parent);
                        n = n->right;
                    }
                    insertCase5(n);
                }
                void insertCase5(Node<K,V>* n) {
                    n->parent->color = Color::BLACK;
                    n->grandparent()->color = Color::RED;
                    if (n == n->parent->left && n->parent == n->grandparent()->left) {
                        rotateRight(n->grandparent());
                    } else {
                        rotateLeft(n->grandparent());
                    }
                }


                /// DELETION
                void deleteCase1(Node<K, V>* n) {
                    if (!(n->parent))
                        return;
                    else deleteCase2(n);
                }
                void deleteCase2( Node<K,V>* n) {
                    if ((n->sibling()->color)->color == Color::RED) {
                        n->parent->color = Color::RED;
                        n->sibling()->color = Color::BLACK;
                        if (n == n->parent->left)
                            rotateLeft(n->parent);
                        else
                            rotateRight(n->parent);
                    }
                    deleteCase3(n);
                }
                void deleteCase3(Node<K,V> n) {
                    if ((n->parent)->color == Color::BLACK &&
                        (n->sibling())->color == Color::BLACK &&
                        (n->sibling()->left)->color == Color::BLACK &&
                        (n->sibling()->right)->color == Color::BLACK)
                    {
                        n->sibling()->color = Color::RED;
                        deleteCase1(n->parent);
                    }
                    else
                        deleteCase4(n);
                }
                void deleteCase4(Node<K,V>* n) {
                    if ((n->parent)->color == Color::RED &&
                        (n->sibling())->color == Color::BLACK &&
                        (n->sibling()->left)->color == Color::BLACK &&
                        (n->sibling()->right)->color == Color::BLACK)
                    {
                        n->sibling()->color = Color::RED;
                        n->parent.color = Color::BLACK;
                    }
                    else
                        deleteCase5(n);
                }
                void deleteCase5(Node<K,V>* n) {
                    if (n == n->parent.left &&
                        (n->sibling())->color == Color::BLACK &&
                        (n->sibling()->left)->color == Color::RED &&
                        (n->sibling()->right)->color == Color::BLACK)
                    {
                        n->sibling()->color = Color::RED;
                        n->sibling()->left.color = Color::BLACK;
                        rotateRight(n->sibling());
                    }
                    else if (n == n->parent.right &&
                             (n->sibling())->color == Color::BLACK &&
                             (n->sibling()->right)->color == Color::RED &&
                             (n->sibling()->left)->color == Color::BLACK)
                    {
                        n->sibling()->color = Color::RED;
                        n->sibling()->right.color = Color::BLACK;
                        rotateLeft(n->sibling());
                    }
                    deleteCase6(n);
                }
                void  deleteCase6(Node<K,V>* n) {
                    n->sibling()->color = (n->parent);
                    n->parent.color = Color::BLACK;
                    if (n == n->parent.left) {
                        n->sibling()->right.color = Color::BLACK;
                        rotateLeft(n->parent);
                    } else {
                        n->sibling()->left.color = Color::BLACK;
                        rotateRight(n->parent);
                    }
                }

                void rotateLeft(Node<K,V>* n) {
                    Node<K,V>* r = n->right;
                    replaceNode(n, r);
                    n->right = r->left;
                    if (r->left) {
                        r->left->parent = n;
                    }
                    r->left = n;
                    n->parent = r;
                }

                void rotateRight(Node<K,V>* n) {
                    Node<K,V>* l = n->left;
                    replaceNode(n, l);
                    n->left = l->right;
                    if (l->right) {
                        l->right->parent = n;
                    }
                    l->right = n;
                    n->parent = l;
                }


            public:
                /**
                 * Avoid to perform sanity checks. I rely on the algorithm and that it behaves
                 */
                bool VERIFY_RBTREE = false;
                /**
                 * Number of elements
                 */
                unsigned long count;
                /**
                 * Main root
                 */
                Node<K,V>* root;
                /**
                 * The comparator function, taking two element values ad returning via the third parameter the result of the
                 * comparison
                 */
                void (*cmp)(K,K,int*);

                /**
                 *
                 * @param comp
                 * @return
                 */
                RBTree(void (*comp)(K,K,int*)): root{nullptr}, count{0}, cmp{comp} {}

                void replaceNode( Node<K, V>* oldn, Node<K, V>* newn) {
                    if (!(oldn->parent)) {
                        root = newn;
                    } else {
                        if (oldn == oldn->parent->left)
                            oldn->parent->left = newn;
                        else
                            oldn->parent->right = newn;
                    }
                    if (newn) {
                        newn->parent = oldn->parent;
                    }
                }

                /**
                 * Returns the pointer to the element contained within the tree, using binary search over the ordered tree
                 * @param key	Key to be searched
                 * @return		The actual element node if it exists. Otherwise, null is returned
                 */
                Node<K,V>* lookupNode(K key) {
                    Node<K,V>* n = root;
                    while (n) {
                        int compResult = 0;
                        (*cmp)(key,n->key,&compResult);
                        if (compResult == 0) {
                            return n;
                        } else if (compResult < 0) {
                            n = n->left;
                        } else {
                            n = n->right;
                        }
                    }
                    return n;
                }

                /**
                 * Removes an element from a given key, if it exists. It doesn't deallocate the
                 * memory
                 * @param key
                 * @return
                 */
                Node<K,V>* remove(K key) {
                    Node<K,V>* n = lookupNode(key);
                    if (n)
                        deleteCase1(n);
                    return n;
                };

                /**
                 * Creates a new entry to the tree, or returns an already-existing one
                 * @param key	Node having this key is created
                 * @return		If the node already exists, then it returns the existing element, otherwise the new one is returned
                 */
                Node<K,V>* insertKey(K key) {
                    //Node<K,V>* insertedNode = new Node<K,V>(key, Color::RED, nullptr, nullptr);
                    bool toret = false;

                    Node<K,V>* toInsert = nullptr;
                    if (!root) {
                        root = toInsert = new Node<K,V>(key, Color::RED, nullptr, nullptr);
                    } else {
                        Node<K,V>* n = root;
                        while (true) {
                            int compResult = 0;
                            (*cmp)(key,n->key,&compResult);
                            if (compResult == 0) {
                                //n.add(value);
                                return n;
                            } else if (compResult < 0) {
                                if (n->left == nullptr) {
                                    toInsert = new Node<K,V>(key, Color::RED, nullptr, nullptr);
                                    n->left = toInsert;
                                    toret = true;
                                    break;
                                } else {
                                    n = n->left;
                                }
                            } else {
                                if (n->right == nullptr) {
                                    toInsert = new Node<K,V>(key, Color::RED, nullptr, nullptr);
                                    n->right = toInsert;
                                    toret = true;
                                    break;
                                } else {
                                    n = n->right;
                                }
                            }
                        }
                        toInsert->parent = n;
                    }
                    insertCase1(toInsert);
                    //verifyProperties();
                    if (toret) count++;
                    return toInsert;
                }

                /**
                 * Implements a tree iterator in a Java fashon.
                 * Please note that the iteration works only once.
                 *
                 * TOXO: when a new iteration begins, reset all the position constants.
                 */
                class rbiterator {
                    Node<K,V>* current;
                    Node<K,V>* cprev;
                    RBTree<K, V>* root;
                    RBTree<K, V>* rprev;
                    bool justStarted;

                public:
                    rbiterator(Node<K,V>* current, RBTree<K, V>* root) {
                        this->current = current;
                        this->cprev = current;
                        this->root = root;
                        this->rprev = root;
                        if (this->current)
                            this->current->visit = PositionIteratorEnum::Left;
                        justStarted = true;
                    }

                    rbiterator(rbiterator& elem) {
                        current = elem.current;
                        root = elem.root;
                        justStarted = elem.justStarted;
                        cprev = elem.cprev;
                        rprev = elem.rprev;
                    }

                    rbiterator(rbiterator&& elem) {
                        current = elem.current;
                        root = elem.root;
                        justStarted = elem.justStarted;
                        cprev = elem.cprev;
                        rprev = elem.rprev;
                    }

                    /**
                     * XXX: rewinds the iteration
                     */
                    void rewind() {
                        current = cprev;
                        root = rprev;
                        justStarted = true;
                        if (this->current)
                            this->current->visit = PositionIteratorEnum::Left;
                    }

                    /**
                     * Returns the key belonging to the currently extracted element
                     * @return
                     */
                    K getCurrentK() {
                        if (current && (!justStarted))
                            return current->key;
                    }

                    bool hasNext() {
                        return current && current->hasNext();
                    }

                    /**
                     * Returns the next element
                     * @return
                     */
                    Node<K,V>* next() {
                        justStarted = false;

                        // If the tree is empty or the current element has already visited the root
                        if (!current) return nullptr;

                        // Until there is some node to visit
                        while (current) {
                            switch (current->visit) {
                                case PositionIteratorEnum::Left:{
                                    if (current->left) {
                                        current->visit = PositionIteratorEnum::Center;
                                        current = current->left;
                                        current->visit = PositionIteratorEnum::Left; //Initialization of the left node's visit
                                    } else {
                                        current->visit=PositionIteratorEnum::Center;
                                    }
                                } break;

                                case PositionIteratorEnum::Center: {
                                    //Visiting the element
                                    current->visit=PositionIteratorEnum::Right;
                                    return current;
                                }

                                case PositionIteratorEnum::Right: {
                                    current->visit=PositionIteratorEnum::None;
                                    if (current->right) {
                                        current = current->right;
                                        current->visit = PositionIteratorEnum::Left;
                                    } else {
                                        // XXX: Re-initializing the iteration
                                        current->visit = PositionIteratorEnum::Left;
                                        if (current->right) {
                                            current->right->visit = PositionIteratorEnum::Left;
                                        }
                                        if (current->left) {
                                            current->left->visit = PositionIteratorEnum::Left;
                                        }
                                        // XXX
                                        current = current->parent;
                                    }
                                } break;

                                case PositionIteratorEnum::None:{
                                    // XXX: Re-initializing the iteration
                                    current->visit = PositionIteratorEnum::Left;
                                    if (current->right) {
                                        current->right->visit = PositionIteratorEnum::Left;
                                    }
                                    if (current->left) {
                                        current->left->visit = PositionIteratorEnum::Left;
                                    }
                                    // XXX
                                    current = current->parent;
                                } break;
                            }
                        }
                        return nullptr;
                    }

                    Node<K,V>* disposeWhileIterating() {
                        justStarted = false;

                        // If the tree is empty or the current element has already visited the root
                        if (!current) return nullptr;

                        // Until there is some node to visit
                        while (current) {
                            switch (current->visit) {
                                case PositionIteratorEnum::Left:{
                                    if (current->left) {
                                        current->visit = PositionIteratorEnum::Center;
                                        current = current->left;
                                        current->visit = PositionIteratorEnum::Left; //Initialization of the left node's visit
                                    } else {
                                        current->visit=PositionIteratorEnum::Center;
                                    }
                                } break;

                                case PositionIteratorEnum::Center: {
                                    //Visiting the element
                                    current->visit=PositionIteratorEnum::Right;
                                    return current;
                                }

                                case PositionIteratorEnum::Right: {
                                    current->visit=PositionIteratorEnum::None;
                                    if (current->right) {
                                        current = current->right;
                                        current->visit = PositionIteratorEnum::Left;
                                    } else {
                                        // XXX: Re-initializing the iteration
                                        current->visit = PositionIteratorEnum::Left;
                                        if (current->right) {
                                            current->right->visit = PositionIteratorEnum::Left;
                                        }
                                        if (current->left) {
                                            current->left->visit = PositionIteratorEnum::Left;
                                        }
                                        // XXX
                                        current = current->parent;
                                    }
                                } break;

                                case PositionIteratorEnum::None:{
                                    // XXX: Re-initializing the iteration
                                    //current->visit = PositionIteratorEnum::Left;
                                    if (current->right) {
                                        delete current->right;
                                        //current->right->visit = PositionIteratorEnum::Left;
                                    }
                                    if (current->left) {
                                        delete current->left;
                                        //current->left->visit = PositionIteratorEnum::Left;
                                    }
                                    current->right = nullptr;
                                    current->left = nullptr;
                                    // XXX
                                    current = current->parent;
                                } break;
                            }
                        }
                        return nullptr;
                    }

                    void clear() {
                        auto it = disposeWhileIterating();
                        while (it.hasNext()) {
                            it.disposeWhileIterating();
                        }
                    }

                };

                /**
                 * Returns…
                 * @return 	A tree iterator starting from the root
                 */
                rbiterator iterator() {
                    rbiterator toret{root,this};
                    return toret;
                }


            };

        }
    }
}
