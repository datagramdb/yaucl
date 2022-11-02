//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_OID_H
#define KNOBAB_SERVER_OID_H

#include <yaucl/structures/bit_tagged_unions.h>
#include <ostream>

using trace_t = uint32_t;
using event_t = uint16_t;
using act_t    = uint16_t;
using monotone_hash_t = uint_least64_t;

TAGGED_UNION_WITH_ENCAPSULATION_BEGIN(monotone_hash_t, oid, 0, 3, act_t act:16, trace_t trace_id:32, event_t event_id:16)
    (act_t fact, trace_t trace_id, event_t event_pos) {
        id.parts.act = fact;
        id.parts.trace_id = trace_id;
        id.parts.event_id = event_pos;
    }
TAGGED_UNION_ENCAPSULATOR_END

#endif //KNOBAB_SERVER_OID_H
