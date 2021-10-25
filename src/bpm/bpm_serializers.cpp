//
// Created by giacomo on 25/10/21.
//

#include <yaucl/bpm/bpm_serializers.h>
#include <iomanip>
#include <date/date.h>
#include <chrono>

void begin_log(std::ostream & os, const std::string& source, const std::string& name) {
    os << "<log xes.version=\"1.0\" xes.features=\"nested-attributes\" openxes.version=\"1.0RC7\">\n"
          "\t<extension name=\"Lifecycle\" prefix=\"lifecycle\" uri=\"http://www.xes-standard.org/lifecycle.xesext\"/>\n"
          "\t<extension name=\"Semantic\" prefix=\"semantic\" uri=\"http://www.xes-standard.org/semantic.xesext\"/>\n"
          "\t<extension name=\"Organizational\" prefix=\"org\" uri=\"http://www.xes-standard.org/org.xesext\"/>\n"
          "\t<extension name=\"Time\" prefix=\"time\" uri=\"http://www.xes-standard.org/time.xesext\"/>\n"
          "\t<extension name=\"Concept\" prefix=\"concept\" uri=\"http://www.xes-standard.org/concept.xesext\"/>\n"
          "\t<string key=\"concept:name\" value=\"" << name << "\"/>\n"
          "\t<string key=\"lifecycle:model\" value=\"standard\"/>\n"
          "\t<string key=\"source\" value=\"" << source << "\"/>";
}

void begin_trace_serialize(std::ostream& os, const std::string& trace_name) {
    os << "\t<trace>" << std::endl;
    os << "\t\t<string key=\"concept:name\" value=" << std::quoted(trace_name) << "/>" << std::endl;
}

void begin_event_serialize(std::ostream& os) {
    os << "\t\t<event>" << std::endl;
}

void serialize_event_label(std::ostream& os, const std::string& act, size_t epoch) {
    std::chrono::milliseconds dur(epoch);
    std::chrono::time_point<std::chrono::system_clock> dt(dur);
    os << "\t\t\t<string key=\"concept:name\" value=" << std::quoted(act) << "/>" << std::endl;
    os << "\t\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>" << std::endl;
    os << "\t\t\t<string key=\"time:timestamp\" value=\"" << date::format("%FT%TZ", dt) << "\"/>" << std::endl;
}

void serialize_attribute(std::ostream& os, const std::string& key, const std::string& value) {
    os << "\t\t\t<string key=" << std::quoted(key) << " value=" << std::quoted(value) << "/>" << std::endl;
}

void serialize_attribute(std::ostream& os, const std::string& key, double value) {
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


void yaucl::bpm::serialize_non_data_log(const yaucl::bpm::log &simple_log, std::ostream &file) {
    begin_log(file, simple_log.data_source, simple_log.name);
    size_t i = 0;
    for (const yaucl::bpm::trace& trace : simple_log.traces) {
        begin_trace_serialize(file, std::to_string(i));
        for (const yaucl::bpm::event& event : trace.trace_events) {
            begin_event_serialize(file);
            serialize_event_label(file, event.event_label_or_name, event.epoch_time_and_event_id.first);
            end_event_serialize(file);
        }
        end_trace_serialize(file);
        i++;
    }
    end_log(file);
}

void yaucl::bpm::serialize_data_log(const yaucl::bpm::log &payload_log, std::ostream &file) {
    begin_log(file, payload_log.data_source, payload_log.name);
    size_t i = 0;
    for (const yaucl::bpm::trace& trace : payload_log.traces) {
        begin_trace_serialize(file, std::to_string(i));
        for (const auto& key_value : trace.payload) {
            if (std::holds_alternative<std::string>(key_value.second)) {
                serialize_attribute(file, key_value.first, std::get<std::string>(key_value.second));
            } else {
                serialize_attribute(file, key_value.first, std::get<double>(key_value.second));
            }
        }
        for (const yaucl::bpm::event& event : trace.trace_events) {
            begin_event_serialize(file);
            serialize_event_label(file, event.event_label_or_name, event.epoch_time_and_event_id.first);
            for (const auto& key_value : event.payload) {
                if (std::holds_alternative<std::string>(key_value.second)) {
                    serialize_attribute(file, key_value.first, std::get<std::string>(key_value.second));
                } else {
                    serialize_attribute(file, key_value.first, std::get<double>(key_value.second));
                }
            }
            end_event_serialize(file);
        }
        end_trace_serialize(file);
        i++;
    }
    end_log(file);
}

#include <yaucl/data/xml.h>
#include <boost/regex.hpp>
using namespace boost;
#include <yaucl/numeric/numeric_base.h>

inline std::string generateStringFromNumber(size_t n) {
    const static std::string defaults = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    auto v = yaucl::numeric::numberToBase(n, defaults.size());
    for (const auto& x : v)
        result += defaults[x];
    return result;
}

inline std::string istr(const smatch &m) {
    return generateStringFromNumber(std::stoul(m[0].str()));
}

yaucl::bpm::log yaucl::bpm::load_xes_ignore_data(const std::string &filename, bool conservative_for_pddl) {
    rapidxml::xml_document<> doc;
    std::vector<char> buffer;
    rapidxml::xml_node<> *root_node = yaucl::data::init_XML_parse(filename, "log", doc, buffer);
    std::string concept_name{"concept:name"}, timeTimestamp{"time:timestamp"};
    yaucl::bpm::log log;

    SIBLING_ITERATE(trace, root_node, "trace", false) {
        yaucl::bpm::trace& transaction = log.traces.emplace_back();
        long long int previous = 0;
        /// TODO: log name and data source

        SIBLING_ITERATE(event, trace, "event", false) {

            std::string eventName = ".";
            long long int timestamp = 0;

            SIBLING_ITERATE(t, event, "string", false) {
                if (GET_ATTRIBUTE(t, "key") == concept_name) {
                    eventName = GET_ATTRIBUTE(t, "value");
                    if (conservative_for_pddl)
                        STRIP_ALL_SPACES(eventName);
                    break;
                }
            }

            SIBLING_ITERATE(t, event, "date", false) {
                if (GET_ATTRIBUTE(t, "key") == timeTimestamp) {
                    timestamp = yaucl::numeric::parse8601(GET_ATTRIBUTE(t, "value"));
                    break;
                }
            }
            //if (timestamp == 0) timestamp += (previous+1);
            if (conservative_for_pddl) {
                eventName = regex_replace(eventName, regex("[[:digit:]]+"), istr);
                STRIP_ALL_SPACES(eventName);
                STRIP_NOT_ALPHAS(eventName);
                TO_LOWER(eventName);
                eventName.erase(remove_if(eventName.begin(), eventName.end(), isspace), eventName.end());
            }
            transaction.trace_events.emplace_back(eventName, timestamp, previous);
            previous++;
        }
    }

    return log;
}

yaucl::bpm::log yaucl::bpm::load_xes_with_data(const std::string &filename, bool conservative_for_pddl) {
    rapidxml::xml_document<> doc;
    std::vector<char> buffer;
    rapidxml::xml_node<> *root_node =  yaucl::data::init_XML_parse(filename, "log", doc, buffer);
    std::string concept_name{"concept:name"}, timeTimestamp{"time:timestamp"};
    yaucl::bpm::log log;

    SIBLING_ITERATE(trace, root_node, "trace", false) {
        yaucl::bpm::trace& transaction = log.traces.emplace_back();
        long long int previous = 0;

        SIBLING_ITERATE(event, trace, "event", false) {
            std::unordered_map<std::string, std::variant<std::string, double>> payload;
            std::string eventName = ".";
            long long int timestamp = 0, counting = 0;

            SIBLING_ITERATE2(t, event) {
                std::string attribute = GET_ATTRIBUTE(t, "key");
                std::string value     = GET_ATTRIBUTE(t, "value");
                std::string tag_name  = t->name();

                if (tag_name == "string") {
                    if (attribute == concept_name) {
                        eventName = value;
                        if (conservative_for_pddl)
                            STRIP_ALL_SPACES(eventName);
                    } else {
                        if (conservative_for_pddl) {
                            STRIP_NOT_ALPHAS(attribute);
                            TO_LOWER(attribute);
                        }
                        payload[attribute] = value;
                    }
                } else if (tag_name == "float") {
                    if (conservative_for_pddl) {
                        STRIP_NOT_ALPHAS(attribute);
                        TO_LOWER(attribute);
                    }
                    payload[attribute] = std::stod(value);
                } else if (tag_name == "int") {
                    if (conservative_for_pddl) {
                        STRIP_NOT_ALPHAS(attribute);
                        TO_LOWER(attribute);
                    }
                    payload[attribute] = std::stod(value);
                } else if (tag_name == "date") {
                    bool doInsert = true;
                    if (conservative_for_pddl) {
                        if (attribute == timeTimestamp) {
                            timestamp = yaucl::numeric::parse8601(GET_ATTRIBUTE(t, "value"));
                            doInsert = false;
                        } else {
                            STRIP_NOT_ALPHAS(attribute);
                            TO_LOWER(attribute);
                        }
                    }
                    if (doInsert)
                        payload["time"] = (float)yaucl::numeric::parse8601(value);
                }
            }

            if (conservative_for_pddl) {
                eventName = regex_replace(eventName, regex("[[:digit:]]+"), istr);
                STRIP_ALL_SPACES(eventName);
                STRIP_NOT_ALPHAS(eventName);
                TO_LOWER(eventName);
                eventName.erase(remove_if(eventName.begin(), eventName.end(), isspace), eventName.end());
            }
            transaction.trace_events.emplace_back(std::pair<size_t, size_t>{timestamp, counting}, eventName, payload);
            counting++;
        }
    }

}
