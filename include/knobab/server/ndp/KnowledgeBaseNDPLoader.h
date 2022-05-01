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
#include <array>

class KnowledgeBaseNDPLoader : public trace_visitor {
    std::string source, name, currentEventLabel;
    bool alreadySet;
    ParsingState status;
    size_t noTraces, currentEventId, actId, currentTrace, globalEventId;
    std::vector<oid> counting_table;
    yaucl::structures::any_to_uint_bimap<std::string> event_label_mapper;
    std::vector<uint64_t> maxActPerTrace;
    std::vector<uint64_t> traceLengthCount;
    std::vector<uint64_t> att_table_primary_index_from_second_element; // after sorting, the displacement where to get the next act id
    std::filesystem::path folder;

    std::fstream count_table_tmp, act_table_tmp;

    // 1. Qualora si inizi con una tracccia, svuotare la tabella temporanea di conteggio,
    //    e pre-alloca tanto spazio quanto i massimi identificativi finora incontrati
    // 3. Per ogni traccia, conto quanto è lunga
    // 2. Per ogni evento incontrato, salva direttamente l'act table in memoria secondaria,
    //    Per definire l'indice primario, conto quante volte occorre globalmente ciascun evento

    void finalize_count_table();

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

//    void load_record(act_t act, trace_t trace_id, event_t event_pos) {
//        sparseTable[act][trace_id] = event_pos;
//        //table.emplace_back(act, trace_id, event_pos);
//    }

    size_t nTraces() const { return noTraces; }

    void finalize_att_table();
};


#endif //KNOBAB_SERVER_KNOWLEDGEBASENDPLOADER_H
