//
// Created by giacomo on 23/01/23.
//

#include "yaucl/bpm/SimpleXESSerializer.h"

void begin_log(std::ostream & os) {
    os << "<log xes.version=\"1.0\" xes.features=\"nested-attributes\" openxes.version=\"1.0RC7\">\n"
          "\t<extension name=\"Lifecycle\" prefix=\"lifecycle\" uri=\"http://www.xes-standard.org/lifecycle.xesext\"/>\n"
          "\t<extension name=\"Semantic\" prefix=\"semantic\" uri=\"http://www.xes-standard.org/semantic.xesext\"/>\n"
          "\t<extension name=\"Organizational\" prefix=\"org\" uri=\"http://www.xes-standard.org/org.xesext\"/>\n"
          "\t<extension name=\"Time\" prefix=\"time\" uri=\"http://www.xes-standard.org/time.xesext\"/>\n"
          "\t<extension name=\"Concept\" prefix=\"concept\" uri=\"http://www.xes-standard.org/concept.xesext\"/>\n"
          "\t<string key=\"concept:name\" value=\"Artificial Log\"/>\n"
          "\t<string key=\"lifecycle:model\" value=\"standard\"/>\n"
          "\t<string key=\"source\" value=\"jackbergus's-BPM21\"/>";
}

void begin_trace_serialize(std::ostream& os, const std::string& trace_name) {
    os << "\t<trace>" << std::endl;
    os << "\t\t<string key=\"concept:name\" value=" << std::quoted(trace_name) << "/>" << std::endl;
}

void begin_event_serialize(std::ostream& os) {
    os << "\t\t<event>" << std::endl;
}

void serialize_event_label(std::ostream& os, const std::string& act,  long long milliseconds) {
    os << "\t\t\t<string key=\"concept:name\" value=" << std::quoted(act) << "/>" << std::endl;
    os << "\t\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>" << std::endl;
    if (milliseconds<0) {
        os << "\t\t\t<string key=\"time:timestamp\" value=\"" << date::format("%FT%TZ", std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now())) << "\"/>" << std::endl;
    } else {
        std::chrono::milliseconds dur(milliseconds);
        std::chrono::time_point<std::chrono::system_clock> dt(dur);
        os << "\t\t\t<string key=\"time:timestamp\" value=\"" << date::format("%FT%TZ", dt) << "\"/>" << std::endl;
    }
}

void serialize_event_attribute(std::ostream& os, const std::string& key, const std::string& value) {
    os << "\t\t\t<string key=" << std::quoted(key) << " value=" << std::quoted(value) << "/>" << std::endl;
}

void serialize_event_attribute(std::ostream& os, const std::string& key, long long value) {
    os << "\t\t\t<int key=" << std::quoted(key) << " value=" << (value) << "/>" << std::endl;
}

void serialize_event_attribute(std::ostream& os, const std::string& key, bool value) {
    os << "\t\t\t<bool key=" << std::quoted(key) << " value=" << (value ? "true" : "false") << "/>" << std::endl;
}

void serialize_event_attribute(std::ostream& os, const std::string& key, double value) {
    os << "\t\t\t<float key=" << std::quoted(key) << " value=\"" << value << "\"/>" << std::endl;
}

void end_event_serialize(std::ostream& os) {
    os << "\t\t</event>" << std::endl;
}

void end_trace_serialize(std::ostream& os) {
    os << "\t</trace>" << std::endl;
}

void end_log(std::ostream& os) {
    os << "</log>";
}

#include <fstream>

void serialize_non_data_log(const std::vector<std::vector<std::string>> &simple_log, const std::string &xes_file) {
    std::ofstream file{xes_file};
    begin_log(file);
    size_t i = 0;
    for (const auto& trace : simple_log) {
        begin_trace_serialize(file, std::to_string(i));
        for (const auto& event : trace) {
            begin_event_serialize(file);
            serialize_event_label(file, event);
            end_event_serialize(file);
        }
        end_trace_serialize(file);
        i++;
    }
    end_log(file);
}

void serialize_data_log(
        const std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> & data_log,
        const std::string &xes_file) {
    std::ofstream file{xes_file};
    begin_log(file);
    size_t i = 0;
    for (const auto& trace : data_log) {
        begin_trace_serialize(file, std::to_string(i));
        for (const auto& event : trace) {
            begin_event_serialize(file);
            serialize_event_label(file, event.first);
            for (const auto& key_value : event.second) {
                if (std::holds_alternative<std::string>(key_value.second)) {
                    serialize_event_attribute(file, key_value.first, std::get<std::string>(key_value.second));
                } else {
                    serialize_event_attribute(file, key_value.first, std::get<double>(key_value.second));
                }
            }
            end_event_serialize(file);
        }
        end_trace_serialize(file);
        i++;
    }
    end_log(file);
}