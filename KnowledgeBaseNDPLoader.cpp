//
// Created by giacomo on 23/04/2022.
//

#include "KnowledgeBaseNDPLoader.h"
#include <iostream>
#include <yaucl/functional/assert.h>


void KnowledgeBaseNDPLoader::enterLog(const std::string &source, const std::string &name) {
    std::ios_base::sync_with_stdio(false);
    maxActPerTrace.clear();
    DEBUG_ASSERT(!this->alreadySet);
    this->source = source;
    this->name = name;
    this->alreadySet = true;
    status = LogParsing;
    noTraces = 0;
//    count_table.table.clear();
}

void KnowledgeBaseNDPLoader::exitLog(const std::string &source, const std::string &name) {
    DEBUG_ASSERT(this->alreadySet);
    DEBUG_ASSERT(this->name == name);
    DEBUG_ASSERT(this->source == source);
    status = FinishParsing;
//    count_table.sort();
}

size_t KnowledgeBaseNDPLoader::enterTrace(const std::string &trace_label) {
    maxActPerTrace.emplace_back(0);
    currentEventId = 0;
//    counting_reference.clear();
    status = TraceParsing;
    actId = 0;
    return (noTraces++);
}

void KnowledgeBaseNDPLoader::exitTrace(size_t traceId) {
    DEBUG_ASSERT(noTraces == (traceId+1));
    status = LogParsing;
}

size_t KnowledgeBaseNDPLoader::enterEvent(size_t chronos_tick, const std::string &event_label) {

    currentEventLabel = event_label;
    actId = event_label_mapper.put(event_label).first;
    maxActPerTrace.back() = std::max(maxActPerTrace.back(), actId);
    auto it = counting_reference.emplace(actId, 1UL);
    if (!it.second) {
        // Existing key, increment the count val
        it.first->second++;
    }
    status = EventParsing;
    act_table_by_act_id.load_record(noTraces - 1, actId, currentEventId);
    enterData_part(true);
    size_t currentEventIdRet = currentEventId++;
    visitField("__time", chronos_tick);
    return currentEventIdRet;
}

void KnowledgeBaseNDPLoader::exitEvent(size_t event_id) {
    currentEventLabel.clear();
    DEBUG_ASSERT(currentEventId == (event_id+1));
    // using counting_reference to populate
    std::vector<std::pair<size_t, size_t>> cp;
    for (const auto& it : counting_reference)
        cp.emplace_back(it.first, it.second);
    std::sort(cp.begin(), cp.end());
    for (const auto& it : cp)
        count_table.load_record(it.first, noTraces-1, it.second);
    status = TraceParsing;
}

void KnowledgeBaseNDPLoader::enterData_part(bool isEvent) {
    if (isEvent)
        DEBUG_ASSERT(status == EventParsing);
    else
        DEBUG_ASSERT(status == TraceParsing);
}

void KnowledgeBaseNDPLoader::exitData_part(bool isEvent) {
    if (isEvent)
        DEBUG_ASSERT(status == EventParsing);
    else
        DEBUG_ASSERT(status == TraceParsing);
}

void KnowledgeBaseNDPLoader::visitField(const std::string &key, bool value) {
    constexpr AttributeTableType type = BoolAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
    if (isEventParsing) {
        ptr2 = &registerEventLabelSchema;
        ptr = &attribute_name_to_table;
    } else if (status == MissingDataParsing)
        ptr = &approximate_attribute_to_table;
    else
        return;
    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}



void KnowledgeBaseNDPLoader::visitField(const std::string &key, double value) {
    constexpr AttributeTableType type = DoubleAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
    if (isEventParsing) {
        ptr2 = &registerEventLabelSchema;
        ptr = &attribute_name_to_table;
    } else if (status == MissingDataParsing)
        ptr = &approximate_attribute_to_table;
    else
        return;
    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

void KnowledgeBaseNDPLoader::visitField(const std::string &key, const std::string &value) {
    constexpr AttributeTableType type = StringAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
    if (isEventParsing) {
        ptr2 = &registerEventLabelSchema;
        ptr = &attribute_name_to_table;
    } else if (status == MissingDataParsing)
        ptr = &approximate_attribute_to_table;
    else
        return;
    maximumStringLength = std::max(maximumStringLength, value.size());
    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

void KnowledgeBaseNDPLoader::visitField(const std::string &key, size_t value) {
    constexpr AttributeTableType type = SizeTAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
    if (isEventParsing) {
        ptr2 = &registerEventLabelSchema;
        ptr = &attribute_name_to_table;
    } else if (status == MissingDataParsing)
        ptr = &approximate_attribute_to_table;
    else
        return;
    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

KnowledgeBaseNDPLoader::KnowledgeBaseNDPLoader(const std::filesystem::path &folder) : folder{folder} {
    DEBUG_ASSERT(std::filesystem::is_directory(folder));
}
