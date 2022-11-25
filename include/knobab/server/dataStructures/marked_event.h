//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_MARKED_EVENT_H
#define KNOBAB_SERVER_MARKED_EVENT_H


#include <knobab/server/dataStructures/oid.h>

#define MARKED_EVENT_LEFT           (1)
#define MARKED_EVENT_ACTIVATION     MARKED_EVENT_LEFT
#define MARKED_EVENT_RIGHT          (2)
#define MARKED_EVENT_TARGET         MARKED_EVENT_RIGHT
#define MARKED_EVENT_MATCH          (3)

using marked_event_type = uint16_t;
using marked_event_future_t = uint16_t;

TAGGED_UNION_WITH_ENCAPSULATION_BEGIN(uint64_t, marked_event, 0, 4, marked_event_type type:16, in_memory_event_id_t left:16, in_memory_event_id_t right:16, marked_event_future_t future:16)
(marked_event_type type, in_memory_event_id_t left, in_memory_event_id_t right = 0, marked_event_future_t future = 0) {
id.parts.type = type;
id.parts.left = left;
id.parts.right = right;
id.parts.future = future;
}

static marked_event left(in_memory_event_id_t left) {
return {MARKED_EVENT_LEFT, left};
}
static marked_event activation(in_memory_event_id_t left) {
return {MARKED_EVENT_ACTIVATION, left};
}
static marked_event right(in_memory_event_id_t right) {
return {MARKED_EVENT_RIGHT, 0, right};
}
static marked_event target(in_memory_event_id_t right) {
return {MARKED_EVENT_TARGET, 0, right};
}
static marked_event join(in_memory_event_id_t left, in_memory_event_id_t right) {
return {MARKED_EVENT_MATCH, left, right};
}
TAGGED_UNION_ENCAPSULATOR_END

        std::ostream &operator<<(std::ostream &os, const marked_event &event);

#define IS_MARKED_EVENT_LEFT(x)  (((marked_event)(x)).id.parts.type == MARKED_EVENT_LEFT)
#define IS_MARKED_EVENT_ACTIVATION(x)  (((marked_event)(x)).id.parts.type == MARKED_EVENT_ACTIVATION)
#define IS_MARKED_EVENT_RIGHT(x)  (((marked_event)(x)).id.parts.type == MARKED_EVENT_RIGHT)
#define IS_MARKED_EVENT_TARGET(x)  (((marked_event)(x)).id.parts.type == MARKED_EVENT_TARGET)
#define IS_MARKED_EVENT_MATCH(x)  (((marked_event)(x)).id.parts.type == MARKED_EVENT_MATCH)
#define GET_ACTIVATION_EVENT(x)    ((IS_MARKED_EVENT_ACTIVATION(x) || IS_MARKED_EVENT_MATCH(x)) ? (x).id.parts.left : 0)
#define GET_TARGET_EVENT(x)    ((IS_MARKED_EVENT_TARGET(x) || IS_MARKED_EVENT_MATCH(x)) ? (x).id.parts.right : 0)
#define GET_LEFT_EVENT(x)    (IS_MARKED_EVENT_LEFT(x) ? (x).id.parts.left : 0)
#define GET_RIGHT_EVENT(x)    (IS_MARKED_EVENT_RIGHT(x) ? (x).id.parts.right : 0)
#define SET_EVENT(x,val)   if (IS_MARKED_EVENT_ACTIVATION(x))  (x).id.parts.left = (val); else if (IS_MARKED_EVENT_TARGET(x))(x).id.parts.right = (val);
#define SET_JOIN_EVENT(x,L,R)   if (IS_MARKED_EVENT_MATCH(x)) { (x).id.parts.left = (L); (x).id.parts.right = (R); }

#include <limits>
#include <ostream>

#define MAX_MARKED_EVENT()      (marked_event{std::numeric_limits<uint64_t>::max()})


#endif //KNOBAB_SERVER_MARKED_EVENT_H
