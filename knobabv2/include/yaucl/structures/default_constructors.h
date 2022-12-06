//
// Created by giacomo on 02/01/2022.
//

#ifndef KNOBAB_DEFAULT_CONSTRUCTORS_H
#define KNOBAB_DEFAULT_CONSTRUCTORS_H

#define DEFAULT_ASSIGNMENT(name)   name& operator=(const name&) = default; name& operator=(name && ) = default;
#define DEFAULT_COPY_CONSTRS(name) name (const name& ) = default; name (name && ) = default;
#define DEFAULT_COPY_ASSGN(name)   DEFAULT_COPY_CONSTRS(name) DEFAULT_ASSIGNMENT(name)
#define DEFAULT_CONSTRUCTORS(name) name () = default; DEFAULT_COPY_ASSGN(name)

#endif //KNOBAB_DEFAULT_CONSTRUCTORS_H
