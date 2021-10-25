//
// Created by giacomo on 25/10/21.
//

#ifndef YAUCL_BPM_LOG_H
#define YAUCL_BPM_LOG_H

#include <string>
#include <unordered_map>
#include <variant>
#include <ostream>
#include <vector>

std::ostream &operator<<(std::ostream &os, const std::variant<std::string, double> &event);

namespace yaucl {
    namespace bpm {
        struct event {
            std::pair<size_t, size_t>                                          epoch_time_and_event_id;
            std::string                                                        event_label_or_name;
            std::unordered_map<std::string, std::variant<std::string, double>> payload;

            event() : event({0,0}, "", {}) {}
            event(const std::string& label, size_t epoch, size_t id) : event({epoch, id}, label, {}) {}
            event(const event& ) = default;
            event(event&& ) = default;
            event& operator=(const event& ) = default;
            event& operator=(event&& ) = default;
            event(const std::pair<size_t, size_t> &epochTimeAndEventId, const std::string &eventLabelOrName,
                  const std::unordered_map<std::string, std::variant<std::string, double>> &payload);
            bool operator==(const event &rhs) const;
            bool operator!=(const event &rhs) const;
        };

        struct trace {
            std::string                                                        event_label_or_name;
            std::unordered_map<std::string, std::variant<std::string, double>> payload;
            std::vector<event>                                                 trace_events;

            trace() : trace("", {}, {}) {}
            trace(const trace& ) = default;
            trace(trace&& ) = default;
            trace& operator=(const trace& ) = default;
            trace& operator=(trace&& ) = default;
            trace(const std::string &eventLabelOrName,
                  const std::unordered_map<std::string, std::variant<std::string, double>> &payload,
                  const std::vector<event> &traceEvents);
            bool operator==(const trace &rhs) const;
            bool operator!=(const trace &rhs) const;
        };

        struct log {
            std::string name;
            std::string data_source;
            std::vector<trace>  traces;

            log() : log("", "", {}) {}
            log(const log& ) = default;
            log(log&& ) = default;
            log& operator=(const log& ) = default;
            log& operator=(log&& ) = default;
            log(const std::string &name, const std::string &dataSource, const std::vector<trace> &traces);
            bool operator==(const log &rhs) const;
            bool operator!=(const log &rhs) const;
        };
    }
}

std::ostream &operator<<(std::ostream &os, const yaucl::bpm::event &event);
std::ostream &operator<<(std::ostream &os, const yaucl::bpm::trace &trace);

#endif //YAUCL_BPM_LOG_H
