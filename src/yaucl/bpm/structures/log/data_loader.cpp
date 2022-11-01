//
// Created by giacomo on 31/10/2021.
//

#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
#include "yaucl/bpm/structures/log/data_loader.h"
#include "yaucl/data/xml.h"
#include "yaucl/numeric/numeric_base.h"
#include <yaucl/functional/assert.h>

struct payload {
    std::unordered_map<std::string, std::string> strings;
    std::unordered_map<std::string, double> floats;
    std::unordered_map<std::string, size_t> ints;
    std::unordered_map<std::string, double> dates;
    std::unordered_map<std::string, bool> booleans;
};

void parse_payload(bool isTrace, rapidxml::xml_node<>* payloadNode, struct payload& pay) {

    std::string tag_name  = payloadNode->name();
    std::string attribute = GET_ATTRIBUTE(payloadNode, "key");
    std::string value     = GET_ATTRIBUTE(payloadNode, "value");
    constexpr std::basic_string_view concept_name{"concept:name"};
    constexpr std::basic_string_view timeTimestamp{"time:timestamp"};
    if (tag_name == "string") {
        //assert(attribute != concept_name);
        pay.strings[attribute] = value;
    } else if (tag_name == "float") {
        pay.floats[attribute] = (double)std::stod(value);
    } else if (tag_name == "int") {
        pay.ints[attribute] = (size_t)std::stoull(value);
    }else if (tag_name == "boolean") {
        pay.booleans[attribute] = (value == "true");
    }  else if (tag_name == "date") {
        if (!isTrace) {
            DEBUG_ASSERT(attribute != timeTimestamp);
        }
        pay.dates[attribute] = (double)yaucl::numeric::parse8601(value);
    }
}


enum XML_SCAN_STEPS {
    LABELS_TRACE_INFO = 0,
    TRACE_PAYLOAD = 1,
    EVENTS = 2
};

#include <chrono>


void parse_event(rapidxml::xml_node<>* event, trace_visitor* tv, bool load_data) {
    constexpr std::basic_string_view concept_name{"concept:name"};
    constexpr std::basic_string_view timeTimestamp{"time:timestamp"};
    DEBUG_ASSERT(tv);
    static size_t event_id = 0;
    bool hasEventOpened = false;
    bool isNewEvent = true;
    struct payload pay;


    std::string trace_name = std::to_string(event_id);
    XML_SCAN_STEPS event_start = XML_SCAN_STEPS::LABELS_TRACE_INFO;
    const auto p1 = std::chrono::system_clock::now();
    unsigned long long int timestamp = std::chrono::duration_cast<std::chrono::hours>(p1.time_since_epoch()).count();

    SIBLING_ITERATE2(t, event) {
        std::string tag_name  = t->name();
        std::string attribute = GET_ATTRIBUTE(t, "key");
        std::string value     = GET_ATTRIBUTE(t, "value");
        hasEventOpened = true;

        if (tag_name == "string") {
            if (attribute == concept_name) {
                trace_name = value;
                isNewEvent = false;
            } else {
                if (load_data) parse_payload(true, t, pay);
            }
        } else if (tag_name == "date") {
            if (attribute == timeTimestamp) {
                timestamp = yaucl::numeric::parse8601(value);
            } else {
                if (load_data) parse_payload(true, t, pay);
            }
        } else {
            if (load_data) parse_payload(true, t, pay);
        }
    }

    if (hasEventOpened) {
        event_id = tv->enterEvent(timestamp, trace_name);
        for (auto &str: pay.strings) {
            tv->visitField(str.first, str.second);
        }
        for (auto &str: pay.booleans) {
            tv->visitField(str.first, str.second);
        }
        for (auto &str: pay.ints) {
            tv->visitField(str.first, str.second);
        }
        for (auto &str: pay.floats) {
            tv->visitField(str.first, str.second);
        }
        for (auto &str: pay.dates) {
            tv->visitField(str.first, str.second);
        }
        tv->exitEvent(event_id);
    }
}

void load_xes_with_data(const std::string &filename, std::istream &file, bool load_data, trace_visitor *tv) {
    DEBUG_ASSERT(tv);
    rapidxml::xml_document<> doc;
    std::vector<char> buffer;
    rapidxml::xml_node<> *root_node =  yaucl::data::init_XML_parse(file, "log", doc, buffer);
    constexpr std::basic_string_view concept_name{"concept:name"};
    constexpr std::basic_string_view timeTimestamp{"time:timestamp"};
    ///yaucl::bpm::log log;
    size_t trace_id = 0;
    tv->enterLog(filename, filename);

    SIBLING_ITERATE(trace, root_node, "trace", false) {
        ///yaucl::bpm::trace& transaction = log.traces.emplace_back();
        long long int previous = 0;
        XML_SCAN_STEPS event_start = XML_SCAN_STEPS::LABELS_TRACE_INFO;
        bool hasTraceOpened = false;
        struct payload pay;

        SIBLING_ITERATE2(t, trace) {
            std::string tag_name  = t->name();
            //std::string trace_name = std::to_string(trace_id++);
            if (tag_name == "event") {
                if (!hasTraceOpened) {
                    hasTraceOpened = true;
                    trace_id = tv->enterTrace(std::to_string(trace_id));
                }
                if ((event_start == XML_SCAN_STEPS::TRACE_PAYLOAD) && (load_data)) {
                    // Before loading the event, I create a dummy event,
                    // which will contain the trace payload information
                    auto event_id = tv->enterEvent(0, "__trace__payload");
                    for (auto &str: pay.strings) {
                        tv->visitField(str.first, str.second);
                    }
                    for (auto &str: pay.booleans) {
                        tv->visitField(str.first, str.second);
                    }
                    for (auto &str: pay.ints) {
                        tv->visitField(str.first, str.second);
                    }
                    for (auto &str: pay.floats) {
                        tv->visitField(str.first, str.second);
                    }
                    for (auto &str: pay.dates) {
                        tv->visitField(str.first, str.second);
                    }
                    tv->exitEvent(event_id);
                }
                event_start = XML_SCAN_STEPS::EVENTS;
                parse_event(t, tv, load_data);
            } else if (tag_name == "string") {
                std::string attribute = GET_ATTRIBUTE(t, "key");
                std::string value     = GET_ATTRIBUTE(t, "value");
                if (attribute == concept_name) {
                    //DEBUG_ASSERT(event_start == XML_SCAN_STEPS::LABELS_TRACE_INFO);
                    //trace_name = value;
                } else {
                    DEBUG_ASSERT(event_start != XML_SCAN_STEPS::EVENTS);
                    if (event_start == XML_SCAN_STEPS::LABELS_TRACE_INFO) {
                        //trace_id = tv->enterTrace(trace_name);
                        event_start = XML_SCAN_STEPS::TRACE_PAYLOAD;
                    }
                    if (load_data) parse_payload(true, t, pay);
                }
            } else {
                DEBUG_ASSERT(event_start != XML_SCAN_STEPS::EVENTS);
                if (event_start == XML_SCAN_STEPS::LABELS_TRACE_INFO) {
                    //trace_id = tv->enterTrace(trace_name);
                    event_start = XML_SCAN_STEPS::TRACE_PAYLOAD;
                }
                if (load_data) parse_payload(true, t, pay);
            }
        }

        if (hasTraceOpened) {
            tv->exitTrace(trace_id);
        }
    }

    tv->exitLog(filename, filename);
}

#include "yaucl/bpm/structures/log/DataTraceParse.h"

void load_act_stream_to_knowledge_base(std::istream &stream, const std::string &filename, KnowledgeBase &tv) {
    std::string line;
    tv.enterLog(filename, filename);
    size_t trace_count = 0;
    size_t timestamp = 0;
    while (std::getline(stream, line)) {
        size_t traceId = tv.enterTrace(std::to_string(trace_count));
        std::istringstream iss(line);
        std::string token;
        while(std::getline(iss, token, '\t')) {
            tv.exitEvent(tv.enterEvent(timestamp++, token));
        }
        tv.exitTrace(traceId);
        trace_count++;
    }
    tv.exitLog(filename, filename);
}

void load_into_knowledge_base(log_data_format format, bool loadData, std::istream &stream, KnowledgeBase &output,
                              const std::string &filename) {
    trace_visitor* tv = (trace_visitor*)&output;
    switch (format) {
        case HUMAN_READABLE_YAUCL: {
            yaucl::bpm::DataTraceParse dp;
            auto tmp = dp.load_also_data;
            dp.load_also_data = loadData;
            dp.load(filename, stream, tv);
            dp.load_also_data = tmp;
            break;
        }
        case XES1:
            load_xes_with_data(filename, stream, loadData, tv); break;
        case TAB_SEPARATED_EVENTS: // This never comes with data
            load_act_stream_to_knowledge_base(stream, filename, output); break;
    }
}

