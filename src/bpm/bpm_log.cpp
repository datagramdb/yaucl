//
// Created by giacomo on 25/10/21.
//

#include "yaucl/bpm/bpm_log.h"

using namespace yaucl::bpm;

event::event(const std::pair<size_t, size_t> &epochTimeAndEventId, const std::string &eventLabelOrName,
             const std::unordered_map<std::string, std::variant<std::string, double>> &payload)
        : epoch_time_and_event_id(epochTimeAndEventId), event_label_or_name(eventLabelOrName), payload(payload) {}

bool event::operator==(const event &rhs) const {
    return epoch_time_and_event_id == rhs.epoch_time_and_event_id &&
           event_label_or_name == rhs.event_label_or_name &&
           payload == rhs.payload;
}

bool event::operator!=(const event &rhs) const {
    return !(rhs == *this);
}

#include <yaucl/strings/serializer.h>

std::ostream &operator<<(std::ostream &os, const event &event) {
    os << "epoch_time_and_event_id: " << event.epoch_time_and_event_id << " event_label_or_name: "
       << event.event_label_or_name << " payload: " << event.payload;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::variant<std::string, double> &event) {
    if (std::holds_alternative<double>(event))
        return os << std::get<double>(event);
    else
        return os << std::get<std::string>(event);
}

trace::trace(const std::string &eventLabelOrName,
             const std::unordered_map<std::string, std::variant<std::string, double>> &payload,
             const std::vector<event> &traceEvents) : event_label_or_name(eventLabelOrName), payload(payload),
                                                      trace_events(traceEvents) {}

bool trace::operator==(const trace &rhs) const {
    return event_label_or_name == rhs.event_label_or_name &&
           payload == rhs.payload &&
           trace_events == rhs.trace_events;
}

bool trace::operator!=(const trace &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const trace &trace) {
    return os << "event_label_or_name: " << trace.event_label_or_name << " payload: " << trace.payload << " trace_events: "
       << trace.trace_events;
}

log::log(const std::string &name, const std::string &dataSource, const std::vector<trace> &traces) : name(name),
                                                                                                     data_source(
                                                                                                             dataSource),
                                                                                                     traces(traces) {}

bool log::operator==(const log &rhs) const {
    return name == rhs.name &&
           data_source == rhs.data_source &&
           traces == rhs.traces;
}

bool log::operator!=(const log &rhs) const {
    return !(rhs == *this);
}
