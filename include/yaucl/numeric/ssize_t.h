//
// Created by giacomo on 02/03/2022.
//

#ifndef KNOBAB_SSIZE_T_H
#define KNOBAB_SSIZE_T_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <BaseTsd.h>
#define ssize_t SSIZE_T
#else
#include <sys/types.h>
#endif

#endif //KNOBAB_SSIZE_T_H
