//
// Created by giacomo on 30/04/2022.
//

#include "knobab/server/ndp/count_table_rcx.h"

bool count_table_rcx::operator<(const count_table_rcx &rhs) const {
    if (act_id < rhs.act_id)
        return true;
    if (rhs.act_id < act_id)
        return false;
    if (trace_id < rhs.trace_id)
        return true;
    if (rhs.trace_id < trace_id)
        return false;
    return count < rhs.count;
}
bool count_table_rcx::operator>(const count_table_rcx &rhs) const {
    return rhs < *this;
}
bool count_table_rcx::operator<=(const count_table_rcx &rhs) const {
    return !(rhs < *this);
}
bool count_table_rcx::operator>=(const count_table_rcx &rhs) const {
    return !(*this < rhs);
}
bool count_table_rcx::operator==(const count_table_rcx &rhs) const {
    return act_id == rhs.act_id &&
           trace_id == rhs.trace_id &&
           count == rhs.count;
}
bool count_table_rcx::operator!=(const count_table_rcx &rhs) const {
    return !(rhs == *this);
}

count_table_rcx::count_table_rcx() : act_id(0), trace_id(0), count(0) {}
