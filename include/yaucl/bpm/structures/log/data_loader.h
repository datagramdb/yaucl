//
// Created by giacomo on 31/10/2021.
//

#ifndef BZDB_DATA_LOADER_H
#define BZDB_DATA_LOADER_H

#include <istream>
#include "trace_visitor.h"

enum log_data_format {
    HUMAN_READABLE_YAUCL = 0,
    XES1 = 1,
    TAB_SEPARATED_EVENTS = 2
};

void scan_through_trace_visitor(log_data_format format, bool loadData, std::istream &stream,trace_visitor* tv,
                              const std::string &filename);
//void load_into_knowledge_base(log_data_format format, bool loadData, std::istream &stream, KnowledgeBase &output,
//                              const std::string &filename);

#endif //BZDB_DATA_LOADER_H
