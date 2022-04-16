//
// Created by giacomo on 16/04/2022.
//

#include <knobab/server/tables/CountTemplate.h>


void CountTemplate::load_record(uint16_t act, uint32_t trace_id, uint16_t event_pos) {
    sparseTable[act][trace_id] = event_pos;
    //table.emplace_back(act, trace_id, event_pos);
}

/*void CountTemplate::emplace_back(const uint_least64_t &monotone_hash) {
    table.emplace_back(monotone_hash);
}*/

void CountTemplate::sanityCheck() {
    assert(std::is_sorted(table.begin(), table.end()));
}

void CountTemplate::sort() {
    std::sort(table.begin(), table.end());
}

void CountTemplate::indexing(uint16_t maxAct, uint32_t maxTraceId) {
    this->maxAct = sparseTable.size();
    this->maxTraceId = maxTraceId;
    for (uint16_t fact = 0; fact < this->maxAct; fact++) {
        auto it = sparseTable.find(fact);
        for (uint32_t sigma_id = 0; sigma_id <= maxTraceId; sigma_id++) {
            if (it == sparseTable.end())
                table.emplace_back(fact, sigma_id, 0);
            else {
                auto it2 = it->second.find(sigma_id);
                if (it2 == it->second.end())
                    table.emplace_back(fact, sigma_id, 0);
                else
                    table.emplace_back(fact, sigma_id, it2->second);
            }
        }
    }
    std::sort(table.begin(), table.end());
    sparseTable.clear();
}

std::pair<const oid *, const oid *> CountTemplate::resolve_primary_index(const uint16_t actId, uint32_t& start, uint32_t& end) const {
    if (actId < maxAct) {
        start = (maxTraceId + 1) * actId;
        end = start + maxTraceId;
        return {table.data() + start, table.data() + end};       // Pointers to first and last oid from Count Table subsection
    } else {
        return {nullptr, nullptr};
    }
}


std::pair<const uint32_t, const uint32_t> CountTemplate::resolve_primary_index(const uint16_t actId) const {
    if (actId < maxAct) {
        const uint32_t start = (maxTraceId + 1) * actId;
        const uint32_t end = start + maxTraceId;
        return {start, end};       // Pointers to first and last oid from Count Table subsection
    } else {
        return {-1, -1};
    }
}

std::ostream &operator<<(std::ostream &os, const CountTemplate &aTemplate) {
    //os << "          CountTemplate" << std::endl << "-------------------------------" << std::endl;
    size_t i = 0;
    os << "RowId,Act,TraceId,EventId" << std::endl;
    for (const auto& ref : aTemplate.table) {
        os << (i++) << "," <<  ref.id.parts.act << "," << ref.id.parts.trace_id << "," << ref.id.parts.event_id << std::endl;
    }
    //os << std::endl << "-------------------------------" << std::endl;
    return os;
}

