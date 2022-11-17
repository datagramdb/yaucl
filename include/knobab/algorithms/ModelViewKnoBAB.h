//
// Created by giacomo on 16/11/22.
//

#ifndef KNOBAB_SERVER_MODELVIEWKNOBAB_H
#define KNOBAB_SERVER_MODELVIEWKNOBAB_H

#include <vector>
#include <knobab/server/declare/DeclareDataAware.h>
#include <yaucl/functional/simplistic_view.h>

using ConjunctiveModel = std::vector<DeclareDataAware>;
using ConjunctiveModelView = yaucl::functional::simplistic_view<DeclareDataAware>;

#endif //KNOBAB_SERVER_MODELVIEWKNOBAB_H
