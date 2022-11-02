//
// Created by giacomo on 15/03/2022.
//

#ifndef KNOBAB_ASSERT_H
#define KNOBAB_ASSERT_H

#ifdef DEBUG
#include <cassert>
#define DEBUG_ASSERT(x)     assert(x)
#else
#define DEBUG_ASSERT(x)     do {} while(0)
#endif

#endif //KNOBAB_ASSERT_H
