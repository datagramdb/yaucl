//
// Created by giacomo on 01/05/2022.
//

#ifndef KNOBAB_SERVER_ATTRIBUTE_TABLE_RCX_H
#define KNOBAB_SERVER_ATTRIBUTE_TABLE_RCX_H

#include <string>

struct attribute_table_rcx {
    size_t act_id, trace_id, event_id, absolute_sequence;

    attribute_table_rcx() : act_id(0), trace_id(0), event_id(0), absolute_sequence(0) {};
    attribute_table_rcx(const attribute_table_rcx&) = default;
    attribute_table_rcx(attribute_table_rcx&& ) = default;
    attribute_table_rcx& operator=(const attribute_table_rcx&) = default;
    attribute_table_rcx& operator=(attribute_table_rcx&& ) = default;

    bool operator<(const attribute_table_rcx &rhs) const;
    bool operator>(const attribute_table_rcx &rhs) const;
    bool operator<=(const attribute_table_rcx &rhs) const;
    bool operator>=(const attribute_table_rcx &rhs) const;
    bool operator==(const attribute_table_rcx &rhs) const ;
    bool operator!=(const attribute_table_rcx &rhs) const ;
};


#endif //KNOBAB_SERVER_ATTRIBUTE_TABLE_RCX_H
