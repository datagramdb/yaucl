//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_OID_H
#define KNOBAB_SERVER_OID_H

#include <yaucl/structures/bit_tagged_unions.h>
#include <ostream>

using in_memory_trace_id_t = uint32_t;
using in_memory_event_id_t = uint16_t;
using in_memory_act_id_t   = uint16_t;
//using monotone_hash_t = uint_least64_t;

TAGGED_UNION_WITH_ENCAPSULATION_BEGIN(uint_least64_t, in_memory_oid, 0, 3, in_memory_act_id_t act:16, in_memory_trace_id_t trace_id:32, in_memory_event_id_t event_id:16)
    (in_memory_act_id_t fact, in_memory_trace_id_t trace_id, in_memory_event_id_t event_pos) {
        id.parts.act = fact;
        id.parts.trace_id = trace_id;
        id.parts.event_id = event_pos;
    }
TAGGED_UNION_ENCAPSULATOR_END

#endif //KNOBAB_SERVER_OID_H
