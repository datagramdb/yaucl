//
// Created by giacomo on 01/05/2022.
//

#include "knobab/server/ndp/act_table_rcx.h"

bool act_table_rcx::operator<(const act_table_rcx &rhs) const {
    if (act_id < rhs.act_id)
        return true;
    if (rhs.act_id < act_id)
        return false;
    if (trace_id < rhs.trace_id)
        return true;
    if (rhs.trace_id < trace_id)
        return false;
    if (event_id < rhs.event_id)
        return true;
    if (rhs.event_id < event_id)
        return false;
    else
        return true; // ???
}

bool act_table_rcx::operator>(const act_table_rcx &rhs) const {
    return rhs < *this;
}

bool act_table_rcx::operator<=(const act_table_rcx &rhs) const {
    return !(rhs < *this);
}

bool act_table_rcx::operator>=(const act_table_rcx &rhs) const {
    return !(*this < rhs);
}

bool act_table_rcx::operator==(const act_table_rcx &rhs) const {
    return act_id == rhs.act_id &&
           trace_id == rhs.trace_id &&
           event_id == rhs.event_id;
}
bool act_table_rcx::operator!=(const act_table_rcx &rhs) const {
    return !(rhs == *this);
}