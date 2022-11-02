//
// Created by giacomo on 30/04/2022.
//

#ifndef KNOBAB_SERVER_COUNT_TABLE_RCX_H
#define KNOBAB_SERVER_COUNT_TABLE_RCX_H

#include <string>

struct count_table_rcx {
    size_t act_id, trace_id, count;

    count_table_rcx();
    count_table_rcx(const count_table_rcx&) = default;
    count_table_rcx(count_table_rcx&&) = default;
    count_table_rcx& operator=(const count_table_rcx&) = default;
    count_table_rcx& operator=(count_table_rcx&&) = default;
    bool operator<(const count_table_rcx &rhs) const ;
    bool operator>(const count_table_rcx &rhs) const ;
    bool operator<=(const count_table_rcx &rhs) const ;
    bool operator>=(const count_table_rcx &rhs) const ;
    bool operator==(const count_table_rcx &rhs) const ;
    bool operator!=(const count_table_rcx &rhs) const ;
};


#endif //KNOBAB_SERVER_COUNT_TABLE_RCX_H
