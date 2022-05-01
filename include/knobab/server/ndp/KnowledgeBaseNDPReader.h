#ifndef KBN_R
#define KBN_R

#include <filesystem>
#include "yaucl/data/VariadicSizeArrayElements.h"
#include "act_table_rcx.h"
#include "count_table_rcx.h"
#include <unordered_map>

class KnowledgeBaseNDPReader {
    std::filesystem::path path;
    yaucl::data::VariadicSizeArrayElements act_array;
    yaucl::data::FixedSizeArrayElements<count_table_rcx> count_table;
    yaucl::data::VariadicSizeArrayElements act_traces;
    yaucl::data::FixedSizeArrayElements<act_table_rcx> act_table;
    std::unordered_map<std::string, size_t> act_name_to_id;
    size_t nTraces, nActs;

public:
    KnowledgeBaseNDPReader(const std::filesystem::path& path) :
            path{path}, act_array(path / "act.array"),
            act_table(path / "act.table"),
            count_table(path / "count.table"),
            act_traces(path / "act.traces") {
        nActs = act_array.size();
        nTraces = act_traces.size();
        for (size_t i = 0, N = get_n_activity_labels(); i<N; i++)
            act_name_to_id.emplace(act_array[i],  i);
    }

    size_t get_n_activity_labels() const { return nActs; }
    size_t get_n_traces() const { return nTraces; }
    size_t get_ith_trace_length(size_t i) const {
        return (i>=nTraces) ? 0 : act_traces.representation_size(i)/sizeof(size_t);
    }
    act_table_rcx* get_event_act(size_t traceId, size_t eventId) const {
        if (traceId >= nTraces) return nullptr;
        else if (eventId >= get_ith_trace_length(traceId)) return nullptr;
        else return &act_table[((size_t*)act_traces[traceId])[eventId]];
    }
    std::pair<count_table_rcx*, count_table_rcx*> act_table_scan(size_t activity_id) const {
        return (activity_id >= nActs) ?
                std::pair<count_table_rcx*, count_table_rcx*>{nullptr, nullptr} :
                std::pair<count_table_rcx*, count_table_rcx*> {&count_table[get_n_traces() * (activity_id)], (&count_table[get_n_traces() * (activity_id+1)-1])+1};
    }
    std::pair<count_table_rcx*, count_table_rcx*> act_table_scan(const std::string& activity_id) const {
        auto it = act_name_to_id.find(activity_id);
        return (it == act_name_to_id.end()) ?
                std::pair<count_table_rcx*, count_table_rcx*>{nullptr, nullptr} :
               std::pair<count_table_rcx*, count_table_rcx*> {&count_table[get_n_traces() * (it->second)], (&count_table[get_n_traces() * (it->second+1)-1])+1};

    }
    size_t act_id_occurrence(size_t traceId, size_t activity_id) {
        if ((activity_id >= nActs) || (nTraces >= traceId))
            return 0;
        else
            return count_table[get_n_traces() * (activity_id)+traceId].count;
    }
    size_t act_id_occurrence(size_t traceId, const std::string& activity_id) {
        auto it = act_name_to_id.find(activity_id);
        if ((it == act_name_to_id.end()) || (nTraces >= traceId))
            return 0;
        else
            return count_table[get_n_traces() * (it->second)+traceId].count;
    }
    const char*  get_ith_activity_label(size_t i) const { return act_array[i]; }

};

#endif