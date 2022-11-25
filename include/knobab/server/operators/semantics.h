//
// Created by giacomo on 09/03/2022.
//

#ifndef KNOBAB_SEMANTICS_H
#define KNOBAB_SEMANTICS_H

#include <optional>
#include <cstdint>
#include <limits>
#include <vector>

#include <knobab/server/dataStructures/oid.h>
#include <yaucl/structures/set_operations.h>
#include <knobab/server/dataStructures/TraceData.h>
#include <knobab/server/dataStructures/marked_event.h>

#ifdef max
#undef max
#endif 

constexpr in_memory_event_id_t max = std::numeric_limits<in_memory_event_id_t>::max();
static const MarkedEventsVector maxVec(max,MAX_MARKED_EVENT());

#include <map>
#include <functional>


enum LeafType {
    ActivationLeaf = 1,
    TargetLeaf = 2,
    NoneLeaf = 0,
    NotALeaf = 4,
    MatchActivationTarget = 3
};



#include <vector>
#include <utility>


/**
 * @author Samuel 'Sam' Appleby
 * @tparam T
 * @param it1
 * @param it2
 * @param vec
 * @param sort
 */
template <typename T>
MarkedEventsVector populateAndReturnEvents(T it1, T it2){
    MarkedEventsVector vec;

    for(auto itr = it1; itr != it2; ++itr){
        for(const auto& r2 : itr->second.second){
            vec.push_back(r2);
        }
    }

    std::sort( vec.begin(), vec.end() );
    vec.erase( std::unique( vec.begin(), vec.end() ), vec.end());

    return vec;
}

/**
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 * @tparam T
 * @param it1
 * @param it2
 * @param vec
 * @param sort
 */
template <typename T> void populateAndReturnEvents(T it1, T it2, MarkedEventsVector& vec, bool sort = true){
    //vec.clear();

    for(auto itr = it1; itr != it2; ++itr){
        std::copy(itr->second.second.begin(), itr->second.second.end(), std::back_inserter(vec));
    }

    if (sort) {
        std::sort( vec.begin(), vec.end() );
        vec.erase( std::unique( vec.begin(), vec.end() ), vec.end());
    }
}

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#endif //KNOBAB_SEMANTICS_H
