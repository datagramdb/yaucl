//
// Created by giacomo on 16/11/22.
//

#ifndef KNOBAB_SERVER_MODELVIEWKNOBAB_H
#define KNOBAB_SERVER_MODELVIEWKNOBAB_H

#include <vector>
#include <knobab/server/declare/DeclareDataAware.h>
#include <yaucl/functional/simplistic_view.h>
#include "knobab/mining/structures/fast_model_search.h"

//using ConjunctiveModel = std::vector<DeclareDataAware>;
using ConjunctiveModelViewForMining = yaucl::functional::simplistic_view<pattern_mining_result<DeclareDataAware>,
                                                                         fast_model_search>;
using ConjunctiveModelView = ConjunctiveModelViewForMining;

#endif //KNOBAB_SERVER_MODELVIEWKNOBAB_H
