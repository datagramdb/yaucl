//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_TRACEDATA_H
#define KNOBAB_SERVER_TRACEDATA_H


#include <knobab/server/dataStructures/marked_event.h>
#include <vector>
#include <utility>

using MarkedEventsVector = std::vector<marked_event>;
using ResultIndex = std::pair<in_memory_trace_id_t, in_memory_event_id_t>;
using ResultRecordSemantics = std::pair<double, MarkedEventsVector>;
using ResultRecord = std::pair<ResultIndex, ResultRecordSemantics>;
using Result = std::vector<ResultRecord>;
using PartialResultRecord = std::pair<ResultIndex, double>;
using PartialResult = std::vector<PartialResultRecord>;

#define RESULT_RECORD_MIN(name, tid, eid)         ResultRecord name{{tid, eid}, {0.0, {}}}
#define RESULT_RECORD_MAX(name, tid, eid)         ResultRecord name{{tid, eid}, {1.0, maxVec}}

#endif //KNOBAB_SERVER_TRACEDATA_H
