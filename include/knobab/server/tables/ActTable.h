//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_ACTTABLE_H
#define KNOBAB_SERVER_ACTTABLE_H


#include <knobab/server/dataStructures/oid.h>
#include <tuple>
#include <vector>
#include <ostream>
#include <string>


uint16_t cast_to_float(size_t x, size_t l);
uint16_t cast_to_float2(size_t x, size_t l);

/**
 * TODO: use a graph to store the prev/next information
 *        look up at the original setting, too
 *  then, populate using emplace_back
 */

struct ActTable {

    struct record {
        in_memory_oid   entry;
        struct record*  prev;
        struct record*  next;

        record();
        record(in_memory_act_id_t act, in_memory_trace_id_t id, time_t time, struct record* prev, struct record* next);
        record(const record& ) = default;
        record& operator=(const record&) = default;

        bool operator<(const record &rhs) const;
        bool operator>(const record &rhs) const;
        bool operator<=(const record &rhs) const;
        bool operator>=(const record &rhs) const;
        bool operator==(const record &rhs) const;
        bool operator!=(const record &rhs) const;
    };

    /**
     * Actual table containing the data and the navigation indices. Those are mainly required for reconstructing the log
     * information for isomorphism purposes
     */
    std::vector<record> table;


    std::vector<size_t> primary_index;

    /**
     * Associating an act id to all of the events from all the traces having the same act id
     */
    [[nodiscard]] std::pair<const uint32_t, const uint32_t> resolve_index(in_memory_act_id_t id) const;

    /**
     * Mapping the trace id to the first and last event (see the log printer from the KnowledgeBase for a usage example)
     */
    std::vector<std::pair<record*, record*>> secondary_index;

    void load_record(in_memory_trace_id_t id, in_memory_act_id_t act, time_t time); // rename: loading_step (emplace_back)
    const std::vector<std::vector<size_t>> & indexing1();
    void indexing2();
    void sanityCheck();
    void clear();
    void clearIDX() {
        builder.trace_id_to_event_id_to_offset.clear();
    }

    std::vector<size_t> trace_length; // L1
    size_t getTraceLength(size_t id) const { return trace_length.at(id); }

    friend std::ostream &operator<<(std::ostream &os, const ActTable &table);

private:
    std::vector<std::tuple<in_memory_trace_id_t, in_memory_event_id_t, size_t>> expectedOrdering; // TODO: remove?

    struct table_builder {
        std::vector<std::vector<std::pair<in_memory_trace_id_t, in_memory_event_id_t>>> act_id_to_trace_id_and_time; // M1
        std::vector<std::vector<size_t>> trace_id_to_event_id_to_offset; // M2
    };

    table_builder builder;

public:
    const std::vector<size_t>& getTraceLengths() const {
        return trace_length;
    }

    const table_builder& getBuilder() const{
        return builder;
    }
};



#endif //KNOBAB_SERVER_ACTTABLE_H
