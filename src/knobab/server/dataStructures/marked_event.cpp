//
// Created by giacomo on 16/04/2022.
//

#include "knobab/server/dataStructures/marked_event.h"

std::ostream &operator<<(std::ostream &os, const marked_event &event) {
    if (IS_MARKED_EVENT_ACTIVATION(event))
        return os << "Activation{" << GET_ACTIVATION_EVENT(event) << "}";
    else if (IS_MARKED_EVENT_TARGET(event))
        return os << "Target{" << GET_TARGET_EVENT(event) << "}";
    else if (IS_MARKED_EVENT_MATCH(event))
        return os << "Match{" << GET_ACTIVATION_EVENT(event) << ',' << GET_TARGET_EVENT(event) << "}";
    else
        return os <<"??";
}