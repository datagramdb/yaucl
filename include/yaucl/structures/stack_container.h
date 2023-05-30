//
// Created by giacomo on 06/02/23.
//

#ifndef GSM_GSQL_STACK_CONTAINER_H
#define GSM_GSQL_STACK_CONTAINER_H

#include <stack>
// https://qr.ae/pruwyr

template<typename T, class C>
struct OpenStack : std::stack<T, C>
{
    using Stack = std::stack<T, C>;
    using Stack::c; // open c to public
};

template<typename T, class C>
C& getContainer(std::stack<T, C>& s)
{
    return static_cast<OpenStack<T,C>&>(s).c; // extract the container
}

template<typename T, class C>
C& getProtectedContainer(std::stack<T, C>& s)
{
    struct OpenStack : std::stack<T, C> {
        using std::stack<T, C>::c;
    };
    return static_cast<OpenStack&>(s).c;
}

#endif //GSM_GSQL_STACK_CONTAINER_H
