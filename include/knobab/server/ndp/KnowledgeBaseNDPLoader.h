//
// Created by giacomo on 23/04/2022.
//

#ifndef KNOBAB_SERVER_KNOWLEDGEBASENDPLOADER_H
#define KNOBAB_SERVER_KNOWLEDGEBASENDPLOADER_H


#include <knobab/server/dataStructures/oid.h>
#include <yaucl/bpm/structures/log/trace_visitor.h>
#include <knobab/server/tables/KnowledgeBase.h>
#include <filesystem>
#include <fstream>
#include "KnowledgeBaseNDPReader.h"

#include <array>

class KnowledgeBaseNDPLoader : public trace_visitor {
    std::string source, name, currentEventLabel;
    bool alreadySet;
    ParsingState status;
    size_t noTraces, currentEventId, actId, currentTrace;
    yaucl::structures::any_to_uint_bimap<std::string> event_label_mapper;
    std::vector<uint64_t> maxActPerTrace;
    std::vector<uint64_t> traceLengthCount;
    std::vector<uint64_t> att_table_counting, att_table_primary_index_from_second_element; // after sorting, the displacement where to get the next act id
    std::filesystem::path folder;
    std::fstream count_table_tmp, act_table_tmp;
    void finalize_count_table();
    void finalize_act_table();
    std::unordered_set<size_t> editedTraces;

public:
    KnowledgeBaseNDPLoader(const std::filesystem::path& folder);

    void enterLog(const std::string &source, const std::string &name) override;
    void exitLog(const std::string &source, const std::string &name) override;
    size_t enterTrace(const std::string &trace_label) override;
    void exitTrace(size_t traceId) override;
    size_t enterEvent(size_t chronos_tick, const std::string &event_label) override;
    void exitEvent(size_t event_id) override;
    void enterData_part(bool isEvent) override;
    void exitData_part(bool isEvent) override;
    void visitField(const std::string &key, bool value) override;
    void visitField(const std::string &key, double value) override;
    void visitField(const std::string &key, const std::string &value) override;
    void visitField(const std::string &key, size_t value) override;

    void startAppendToExistingTrace(size_t append, KnowledgeBaseNDPReader& file_storage) {
        editedTraces.insert(append);
        DEBUG_ASSERT(append < noTraces);
        std::ios_base::sync_with_stdio(false);
        count_table_tmp = std::fstream(folder / "count.table", std::ios::out | std::ios::binary | std::ios_base::app);
        act_table_tmp = std::fstream(folder / "act.table", std::ios::out | std::ios::binary | std::ios_base::app);
        DEBUG_ASSERT(!this->alreadySet);
        this->alreadySet = true;
        status = LogParsing;
        size_t N = file_storage.get_n_activity_labels();
        att_table_counting.resize(N, 0);
        currentTrace = append;
        for (size_t i = 0; i<N; i++) att_table_counting[i] = file_storage.count_table_per_trace(append, i);
        currentEventId = file_storage.get_ith_trace_length(append);
    }
    void stopAppendToExistingTrace(size_t append, KnowledgeBaseNDPReader& file_storage) {
        traceLengthCount[append] = (currentEventId);
        maxActPerTrace[append] = (event_label_mapper.int_to_T.size());
        DEBUG_ASSERT(noTraces == (traceId+1));
        status = LogParsing;
        size_t N = file_storage.get_n_activity_labels();
        for (size_t i = 0; i < N; i++) {
            file_storage.update_event_count(append, i, att_table_counting[i]);
        }
        for (size_t i = N; i<maxActPerTrace[append]; i++) {

        }

        for (size_t i : editedTraces) {
            for (size_t j = 0; j<att_table_counting.size(); j++) {

            }
        }
    }

    size_t nTraces() const { return noTraces; }
    void reloadFromFiles(KnowledgeBaseNDPReader& file_storage);
};


#endif //KNOBAB_SERVER_KNOWLEDGEBASENDPLOADER_H
