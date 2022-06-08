//
// Created by giacomo on 23/04/2022.
//

#include "knobab/server/ndp/KnowledgeBaseNDPLoader.h"
#include <iostream>
#include <yaucl/functional/assert.h>
#include <yaucl/data/mmapFile.h>


void KnowledgeBaseNDPLoader::enterLog(const std::string &source, const std::string &name) {
    std::ios_base::sync_with_stdio(false);
//    act_table_tmp = std::fstream(folder / "act.table", std::ios::out | std::ios::binary);
    act_table.open(folder / "act.table", std::filesystem::temp_directory_path());
//    count_table_tmp = std::fstream(folder / "count.table", std::ios::out | std::ios::binary);
    count_table.open(folder / "count.table", std::filesystem::temp_directory_path());

    maxActPerTrace.clear();
    DEBUG_ASSERT(!this->alreadySet);
    this->source = source;
    this->name = name;
    this->alreadySet = true;
    status = LogParsing;
    noTraces = 0;
}

void KnowledgeBaseNDPLoader::exitLog(const std::string &source, const std::string &name) {
    DEBUG_ASSERT(this->alreadySet);
    DEBUG_ASSERT(this->name == name);
    DEBUG_ASSERT(this->source == source);
    status = FinishParsing;

    finalize_count_table();
    finalize_act_table();
}

size_t KnowledgeBaseNDPLoader::enterTrace(const std::string &trace_label) {
    actId = 0;
    att_table_counting.clear();
    att_table_counting.resize(event_label_mapper.int_to_T.size(), 0);
    att_table_primary_index_from_second_element.resize(event_label_mapper.int_to_T.size(), 0);
    currentEventId = 0;
//    counting_reference.clear();
    status = TraceParsing;
    currentTrace = noTraces++;
    return currentTrace;
}

void KnowledgeBaseNDPLoader::exitTrace(size_t traceId) {
    traceLengthCount.emplace_back(currentEventId);
    maxActPerTrace.emplace_back(event_label_mapper.int_to_T.size());
    DEBUG_ASSERT(noTraces == (traceId+1));
    status = LogParsing;

    for (size_t i = 0, N = att_table_counting.size(); i < N; i++) {
//        count_table_tmp.write((char*)&i, sizeof(size_t));
//        count_table_tmp.write((char*)&traceId, sizeof(size_t));
//        count_table_tmp.write((char*)&att_table_counting[i], sizeof(size_t));
        record2.trace_id = traceId;
        record2.act_id = i;
        record2.count = att_table_counting[i];
        count_table.put(record2);
    }
}

size_t KnowledgeBaseNDPLoader::enterEvent(size_t chronos_tick, const std::string &event_label) {
    currentEventLabel = event_label;
    actId = event_label_mapper.put(event_label).first;
    if (actId >= att_table_primary_index_from_second_element.size()) {
        att_table_primary_index_from_second_element.emplace_back(1);
        att_table_counting.emplace_back(1);
    } else {
        att_table_primary_index_from_second_element[actId]++;
        att_table_counting[actId]++;
    }

    record1.act_id = actId;
    record1.trace_id = currentTrace;
    record1.event_id = currentEventId;
    act_table.put(record1);

/// OLD
//    act_table_tmp.write((char*)&actId, sizeof(actId));
//    act_table_tmp.write((char*)&currentTrace, sizeof(currentTrace));
//    act_table_tmp.write((char*)&currentEventId, sizeof(currentEventId));

//    auto it = counting_reference.emplace(actId, 1UL);
//    if (!it.second) {
//        // Existing key, increment the count val
//        it.first->second++;
//    }
    status = EventParsing;
//    act_table_by_act_id.load_record(noTraces - 1, actId, currentEventId);
    enterData_part(true);
    size_t currentEventIdRet = currentEventId++;
    visitField("__time", chronos_tick);
    return currentEventIdRet;
}

void KnowledgeBaseNDPLoader::exitEvent(size_t event_id) {
    currentEventLabel.clear();
    DEBUG_ASSERT(currentEventId == (event_id+1));
    // using counting_reference to populate
//    std::vector<std::pair<size_t, size_t>> cp;
//    for (const auto& it : counting_reference)
//        cp.emplace_back(it.first, it.second);
//    std::sort(cp.begin(), cp.end());
//    for (const auto& it : cp)
//        count_table.load_record(it.first, noTraces-1, it.second);
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
//    if (isEventParsing) {
//        ptr2 = &registerEventLabelSchema;
//        ptr = &attribute_name_to_table;
//    } else if (status == MissingDataParsing)
//        ptr = &approximate_attribute_to_table;
//    else
//        return;
//    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}



void KnowledgeBaseNDPLoader::visitField(const std::string &key, double value) {
    constexpr AttributeTableType type = DoubleAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
//    if (isEventParsing) {
//        ptr2 = &registerEventLabelSchema;
//        ptr = &attribute_name_to_table;
//    } else if (status == MissingDataParsing)
//        ptr = &approximate_attribute_to_table;
//    else
//        return;
//    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

void KnowledgeBaseNDPLoader::visitField(const std::string &key, const std::string &value) {
    constexpr AttributeTableType type = StringAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
//    if (isEventParsing) {
//        ptr2 = &registerEventLabelSchema;
//        ptr = &attribute_name_to_table;
//    } else if (status == MissingDataParsing)
//        ptr = &approximate_attribute_to_table;
//    else
//        return;
//    maximumStringLength = std::max(maximumStringLength, value.size());
//    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

void KnowledgeBaseNDPLoader::visitField(const std::string &key, size_t value) {
    constexpr AttributeTableType type = SizeTAtt;
    std::unordered_map<std::string, AttributeTable>* ptr = nullptr;
    std::unordered_map<std::string, std::unordered_set<std::string>>* ptr2 = nullptr;
    bool isEventParsing = (status == EventParsing);
//    if (isEventParsing) {
//        ptr2 = &registerEventLabelSchema;
//        ptr = &attribute_name_to_table;
//    } else if (status == MissingDataParsing)
//        ptr = &approximate_attribute_to_table;
//    else
//        return;
//    fillInAtGivenStep(key, value, type, ptr, ptr2, noTraces-1, currentEventId-1, currentEventLabel, actId);
}

KnowledgeBaseNDPLoader::KnowledgeBaseNDPLoader(const std::filesystem::path &folder) : folder{folder}, status{FinishParsing} {
    DEBUG_ASSERT(std::filesystem::is_directory(folder));
    alreadySet = false;
}

#include <yaucl/data/FixedSizeNDPSorter.h>
#include <knobab/server/ndp/count_table_rcx.h>
#include <knobab/server/ndp/act_table_rcx.h>

void KnowledgeBaseNDPLoader::finalize_count_table() {
    // Adding the data that was missed before
    size_t noData = 0;
    size_t N =event_label_mapper.int_to_T.size();
    for (size_t i = 0; i<noTraces; i++) {
        for (size_t j = maxActPerTrace.at(i); j<N; j++) {
            record2.trace_id = i;
            record2.act_id = j;
            record2.count = noData;
            count_table.put(record2);
//            count_table_tmp.write((char*)&j, sizeof(size_t));
//            count_table_tmp.write((char*)&i, sizeof(size_t));
//            count_table_tmp.write((char*)&noData, sizeof(size_t));
        }
    }
    maxActPerTrace.clear();

    // Finalizing the file
//    count_table_tmp.flush();
//    count_table_tmp.close();

    count_table.sort(availableMemory() / 4);
//    FixedSizeNDPSorter<count_table_rcx> file{availableMemory() / 4};
//    file.sort(folder / "count.table", std::filesystem::temp_directory_path());
}

void KnowledgeBaseNDPLoader::finalize_act_table() {
//    act_table_tmp.flush();
//    act_table_tmp.close();

//    {
//
//        FixedSizeNDPSorter<act_table_rcx> file{availableMemory() / 4};
//        file.sort(folder / "act.table", std::filesystem::temp_directory_path());
//    }
// sorting the table
    act_table.sort(availableMemory() / 4);

    // Writing the primary index for the attributes table, that is, stating where each attribute will start
    {
        size_t primaryIndexOffset = 0;
        //std::fstream att_primary = std::fstream(folder/"act.primary", std::ios::out | std::ios::binary);
        att_primary.open(folder/"act.primary",  std::filesystem::temp_directory_path());
        for (size_t i = 0; i<att_table_primary_index_from_second_element.size(); i++) {
            att_primary.put(primaryIndexOffset);
//            att_primary.write((char*)&primaryIndexOffset, sizeof(i));
            primaryIndexOffset += att_table_primary_index_from_second_element.at(i);
        }
        att_primary.close();
    }

    // Writing all the attributes
    {
//        std::fstream attribute_list = std::fstream(folder/"act.array", std::ios::out | std::ios::binary);

//        // writing the total number of strings in the array
//        size_t totalOffset = event_label_mapper.int_to_T.size();
//        attribute_list.write((char*)&totalOffset, sizeof(size_t));
//
//        // writing the offsets containing the strings
//        totalOffset = 0;
//        for (auto& ref : event_label_mapper.int_to_T) {
//            attribute_list.write((char*)&totalOffset, sizeof(size_t));
//            totalOffset += ref.size() + 1;
//        }

        attribute_list.open(folder/"act.array");
        // writing the actual strings
        char ec = 0;
        new_iovec memo;
        for (auto & ref : event_label_mapper.int_to_T) {
            memo.iov_len = ref.size()+1;
            memo.iov_base = (void*)ref.c_str();
            attribute_list.put(memo);
//            attribute_list.write((char*)ref.c_str(), ref.size());
//            attribute_list.write(&ec, sizeof(char));
        }
        attribute_list.close();
    }

    {
        std::fstream trace_file = std::fstream(folder/"act.traces", std::ios::out | std::ios::binary);

        // Writing first the overall number of traces
        trace_file.write((char*)&noTraces, sizeof(noTraces));

        // For each trace, writing their displacement
        size_t traceOffset = 0;
        std::vector<size_t> totalTraceLengthCount;
        for (size_t i = 0; i<noTraces; i++) {
            trace_file.write((char*)&traceOffset, sizeof(traceOffset));
            totalTraceLengthCount.emplace_back(traceOffset/ sizeof(size_t));
            traceOffset+=traceLengthCount.at(i) * sizeof(size_t);
        }

        // Assumption: the whole traces will fit in primary memory as ids.
//        yaucl::data::MemoryMappedFile file{folder / "act.table"};

//        std::vector<size_t> posFile(file.cast_size<act_table_rcx>());

        std::vector<size_t> posFile(act_table.size());
        size_t N = act_table.size(); // file.cast_size<act_table_rcx>()
        for (size_t i = 0; i<N; i++) {
            auto& ref = act_table.get(i);//file.at<act_table_rcx>(i);
//            std::cout << totalTraceLengthCount[ref.trace_id] + ref.event_id << std::endl;
            posFile[totalTraceLengthCount[ref.trace_id] + ref.event_id] = i;
        }
        for (size_t i = 0, N = posFile.size(); i<N; i++) {
            trace_file.write((char*)&posFile[i], sizeof(size_t));
        }
        posFile.clear();
    }

}

void KnowledgeBaseNDPLoader::reloadFromFiles(KnowledgeBaseNDPReader &file_storage) {
    source.clear(); name.clear(); currentEventLabel.clear();
    alreadySet = false; status = FinishParsing;
    noTraces = file_storage.get_n_traces();
    currentEventId = 0;
    actId = 0;
    currentTrace = 0;
    traceLengthCount.clear();
    event_label_mapper.clear();
    size_t N = file_storage.get_n_activity_labels();
    maxActPerTrace.resize(noTraces, N);
    att_table_primary_index_from_second_element.resize(N, 0);
    std::vector<bool> bitsetVec(noTraces, false);
    size_t noMetMax = 0;
    for (size_t i = 0; i<N; i++) {
        event_label_mapper.put(file_storage.get_ith_activity_label(i));
        {
            auto cp = file_storage.act_table_scan(N-i-1);
            att_table_primary_index_from_second_element[N-i-1] = cp.second-cp.first;
        }
    }
    for (size_t i =0; i<noTraces; i++) {
        traceLengthCount.emplace_back(file_storage.get_ith_trace_length(i));
        att_table_counting.clear();
    }
//    editedTraces.resize(noTraces, false);
}

bool three_fields::operator<(const three_fields &rhs) const {
    if (actId < rhs.actId)
        return true;
    if (rhs.actId < actId)
        return false;
    if (currentTrace < rhs.currentTrace)
        return true;
    if (rhs.currentTrace < currentTrace)
        return false;
    return currentEventId < rhs.currentEventId;
}

bool three_fields::operator>(const three_fields &rhs) const {
    return rhs < *this;
}

bool three_fields::operator<=(const three_fields &rhs) const {
    return !(rhs < *this);
}

bool three_fields::operator>=(const three_fields &rhs) const {
    return !(*this < rhs);
}
