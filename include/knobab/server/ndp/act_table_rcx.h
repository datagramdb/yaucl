//
// Created by giacomo on 01/05/2022.
//

#ifndef KNOBAB_SERVER_ACT_TABLE_RCX_H
#define KNOBAB_SERVER_ACT_TABLE_RCX_H

#include <string>

struct act_table_rcx {
    size_t act_id, trace_id, event_id, absolute_sequence;

    act_table_rcx() : act_id(0), trace_id(0), event_id(0), absolute_sequence(0) {};
    act_table_rcx(const act_table_rcx&) = default;
    act_table_rcx(act_table_rcx&& ) = default;
    act_table_rcx& operator=(const act_table_rcx&) = default;
    act_table_rcx& operator=(act_table_rcx&& ) = default;

    bool operator<(const act_table_rcx &rhs) const;
    bool operator>(const act_table_rcx &rhs) const;
    bool operator<=(const act_table_rcx &rhs) const;
    bool operator>=(const act_table_rcx &rhs) const;
    bool operator==(const act_table_rcx &rhs) const ;
    bool operator!=(const act_table_rcx &rhs) const ;
};


#endif //KNOBAB_SERVER_ACT_TABLE_RCX_H
