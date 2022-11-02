//
// Created by giacomo on 16/04/2022.
//

#include <knobab/server/tables/KnowledgeBase.h>

#include <cassert>
#include <cmath>
#include <magic_enum.hpp>

std::string KnowledgeBase::default_string;
double KnowledgeBase::maximum_reliability_for_insertion = 0.99;// TODO: PREV_DOUBLE(PREV_DOUBLE(1.0));

static inline void fillInAtGivenStep(const std::string &key,
                                     const std::variant<double, size_t, long long int, std::string, bool>& value,
                                     const AttributeTableType &type,
                                     std::unordered_map<std::string, AttributeTable> *ptr,
                                     std::unordered_map<std::string, std::unordered_set<std::string>> *ptr2,
                                     size_t TraceId, size_t EventId, const std::string &EventLabel,
                                     size_t ActId)  {
    auto it = ptr->find(key);
    if (it == ptr->end()) {
        it = ptr->emplace(key, AttributeTable{key, type}).first;
    } else if (it->second.type != type) {
        std::string s{magic_enum::enum_name(it->second.type)};
        std::string currentType{magic_enum::enum_name(type)};
        throw std::runtime_error(key+" was already associated to " + s+", now it also have "+currentType+": FATAL ERROR!");
    }
    if (ptr2) ptr2->operator[](EventLabel).insert(key);
    it->second.record_load(ActId, value, TraceId, EventId);
}

KnowledgeBase::KnowledgeBase() : alreadySet{false} {
    status = FinishParsing;
}

void KnowledgeBase::reconstruct_trace_no_data(std::ostream &os) const {
    for (size_t trace_id = 0, N = act_table_by_act_id.secondary_index.size(); trace_id < N; trace_id++) {
        os << "Trace #" << trace_id << std::endl << "\t- ";
        const auto& ref = act_table_by_act_id.secondary_index[trace_id];
        auto ptr = ref.first;
        while (ptr) {
            os << event_label_mapper.get(ptr->entry.id.parts.act) << "; ";
            ptr = ptr->next;
        };
        os << std::endl;
    }
}


void KnowledgeBase::reconstruct_trace_with_data(std::ostream &os) const {
    constexpr size_t record_size = sizeof(ActTable::record);
    for (size_t trace_id = 0, N = act_table_by_act_id.secondary_index.size(); trace_id < N; trace_id++) {
        os << "Trace #" << trace_id << std::endl << "\t- ";
        const auto& ref = act_table_by_act_id.secondary_index[trace_id];
        auto ptr = ref.first;
        while (ptr) {
            size_t offset = (((size_t)ptr) - ((size_t)act_table_by_act_id.table.data()));
            offset = offset / record_size;
            // printing one event at a time
            os << event_label_mapper.get(ptr->entry.id.parts.act) << "{ ";
            for (const auto& attr_table : this->attribute_name_to_table) {
                const AttributeTable::record* recordPtr = attr_table.second.resolve_record_if_exists(offset);
                if (recordPtr) {
                    DEBUG_ASSERT(recordPtr->act_table_offset == offset);
                    os << attr_table.first << '=';
                    attr_table.second.resolve_and_print(os, *recordPtr);
                    os << ", ";
                }
            }
            os << "} ";
            ptr = ptr->next;
        };
        os << std::endl;
    }
}


void KnowledgeBase::index_data_structures(bool missingDataIndexing) {

    /// Filling the gaps in the sparse table, so to take into account the events that are missing from a given trace
    count_table.indexing(actId, this->noTraces-1);

    /// generating the primary index for the ActTable, and returning its intermediate index, M2
    const auto& idx = act_table_by_act_id.indexing1();

    /// Applying the intermediate index M2 to each attribute table, so to continue with the value indexing
    for (auto& attr_name_to_table_cp : attribute_name_to_table)
        attr_name_to_table_cp.second.index(idx);

    /// Continuing to create the secondary index out of M2, as well as clearing M2
    act_table_by_act_id.indexing2();
    act_table_by_act_id.sanityCheck();

    /// Creating the universe relationships, so to return it efficiently without taking too much time!
    universe.clear();
    universeApprox.clear();
    /*for (event_t i = 0; i<noTraces; i++) {
        for (act_t j = 0, N = act_table_by_act_id.getTraceLength(i); j<N; j++) {
            universe.emplace_back(std::make_pair(i, j), 1.0);
            universeApprox.emplace_back(std::make_pair(i, j), maximum_reliability_for_insertion);
        }
    }*/
    DEBUG_ASSERT(std::is_sorted(universe.begin(), universe.end()));

    if (missingDataIndexing) {
        std::cout << "NOW INDEXING 'MISSING DATA'" << std::endl;
        auto tmp = status;
        status = MissingDataParsing;
        constexpr size_t record_size = sizeof(ActTable::record);
        for (size_t trace_id = 0, N = act_table_by_act_id.secondary_index.size(); trace_id < N; trace_id++) {
            ///os << "Trace #" << trace_id << std::endl << "\t- ";
            const auto& ref = act_table_by_act_id.secondary_index[trace_id];
            auto ptr = ref.first;
            size_t event_id = 0;
            while (ptr) {
                size_t offset = (((size_t)ptr) - ((size_t)act_table_by_act_id.table.data()));
                offset = offset / record_size;
                std::string eventLabel = event_label_mapper.get(ptr->entry.id.parts.act);
                for (const auto& attr_table : this->attribute_name_to_table) {
                    const AttributeTable::record* recordPtr = attr_table.second.resolve_record_if_exists(offset);
                    if (!recordPtr) {
                        std::variant<double, size_t, long long int, std::string, bool> val;
                        switch (attr_table.second.type) {
                            case DoubleAtt:
                                val = default_double;
                                break;
                            case SizeTAtt:
                                val = default_size_t;
                                break;
                            case LongAtt:
                                val = default_longlong;
                                break;
                            case StringAtt:
                                val = default_string;
                                break;
                            case BoolAtt:
                                val = default_bool;
                                break;
                        }
                        fillInAtGivenStep(attr_table.first, val, attr_table.second.type, &approximate_attribute_to_table,
                                          nullptr, trace_id, event_id, eventLabel, ptr->entry.id.parts.act);
                    }
                }
                ptr = ptr->next;
                event_id++;
            }
            status = tmp;

        }

        /// Applying the intermediate index M2 to each attribute table, so to continue with the value indexing
        for (auto& attr_name_to_table_cp : approximate_attribute_to_table)
            attr_name_to_table_cp.second.index(idx);
    }

    //act_table_by_act_id.clearIDX();
}

///////////////// Event System
#include <iostream>
#include <yaucl/functional/assert.h>


void KnowledgeBase::enterLog(const std::string &source, const std::string &name) {
    DEBUG_ASSERT(!this->alreadySet);
    this->source = source;
    this->name = name;
    this->alreadySet = true;
    status = LogParsing;
    noTraces = 0;
    count_table.table.clear();
}

void KnowledgeBase::exitLog(const std::string &source, const std::string &name) {
    DEBUG_ASSERT(this->alreadySet);
    DEBUG_ASSERT(this->name == name);
    DEBUG_ASSERT(this->source == source);
    status = FinishParsing;
    count_table.sort();
}

size_t KnowledgeBase::enterTrace(const std::string &trace_label) {
    currentEventId = 0;
    counting_reference.clear();
    status = TraceParsing;
    actId = 0;
    return (noTraces++);
}

void KnowledgeBase::exitTrace(size_t traceId) {
    DEBUG_ASSERT(noTraces == (traceId+1));
    status = LogParsing;
}

size_t KnowledgeBase::enterEvent(size_t chronos_tick, const std::string &event_label) {
    currentEventLabel = event_label;
    actId = event_label_mapper.put(event_label).first;
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

void KnowledgeBase::exitEvent(size_t event_id) {
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

void KnowledgeBase::enterData_part(bool isEvent) {
    if (isEvent)
        DEBUG_ASSERT(status == EventParsing);
    else
        DEBUG_ASSERT(status == TraceParsing);
}

void KnowledgeBase::exitData_part(bool isEvent) {
    if (isEvent)
        DEBUG_ASSERT(status == EventParsing);
    else
        DEBUG_ASSERT(status == TraceParsing);
}

void KnowledgeBase::visitField(const std::string &key, bool value) {
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



void KnowledgeBase::visitField(const std::string &key, double value) {
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

void KnowledgeBase::visitField(const std::string &key, const std::string &value) {
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

void KnowledgeBase::visitField(const std::string &key, size_t value) {
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

void
KnowledgeBase::collectValuesFrom(std::set<union_type> &S, ssize_t trace_id, uint16_t act_id,
                                 const std::string &attribute_name) const {
    bool hasNoAttribute = attribute_name.empty();
    bool hasNoActId = act_id == (uint16_t)-1;
    ssize_t N = act_table_by_act_id.secondary_index.size();
    if (trace_id == -1) {
        for (size_t currentTraceId = 0; currentTraceId < N; currentTraceId++) {
            collectValuesAmongTraces(S, currentTraceId, act_id, hasNoActId, attribute_name, hasNoAttribute);
        }
    } else if ((trace_id >= 0) && (trace_id < N)) {
        collectValuesAmongTraces(S, trace_id, act_id, hasNoActId, attribute_name, hasNoAttribute);
    }
}

#include <bitset>


void KnowledgeBase::collectValuesAmongTraces(std::set<union_type> &S, size_t trace_id, act_t acts, bool hasNoActId,
                                             const std::string &attribute_name, bool hasNoAttribute) const {
    const auto& ref = act_table_by_act_id.secondary_index[trace_id];
    auto ptr = ref.first;
    while (ptr) {
        if (hasNoActId || (ptr->entry.id.parts.act == acts)) {
            for (const auto& attr_table : attribute_name_to_table) {
                ptrdiff_t offset = ptr - act_table_by_act_id.table.data();
                const AttributeTable::record* recordPtr = attr_table.second.resolve_record_if_exists(offset);
                if (recordPtr &&  (hasNoAttribute || (attribute_name == attr_table.first))) {
                    S.insert(attr_table.second.resolve(*recordPtr));
                }
            }
        }
        ptr = ptr->next;
    }
}

void
KnowledgeBase::collectValuesFrom(std::set<union_type> &S, const std::unordered_set<size_t> &trace_ids, uint16_t act_id,
                                 const std::string &attribute_name) const {

    bool hasNoAttribute = attribute_name.empty();
    bool hasNoActId = act_id == (uint16_t)-1;
    ssize_t N = act_table_by_act_id.secondary_index.size();
    for (size_t currentTraceId : trace_ids) {
        if (currentTraceId < N)
            collectValuesAmongTraces(S, currentTraceId, act_id, hasNoActId, attribute_name, hasNoAttribute);
    }
}

void KnowledgeBase::collectValuesFrom(
        std::unordered_map<std::string, std::unordered_map<std::string, std::set<union_type>>> &result,
        std::unordered_map<std::string, std::set<union_type>> &resultOtherValues,
        const std::unordered_set<trace_t> &trace_ids,
        const std::unordered_map<std::string, std::unordered_set<std::string>> &actToTables,
        const std::unordered_set<std::string> &otherValues) const {
    size_t N = act_table_by_act_id.secondary_index.size();
    if (trace_ids.empty()) {
        for (size_t currentTraceId = 0; currentTraceId < N; currentTraceId++) {
            collectValuesAmongTraces(result, resultOtherValues, actToTables, otherValues, currentTraceId);
        }
    } else {
        for (trace_t traceId : trace_ids) {
            if (traceId < N) {
                collectValuesAmongTraces(result, resultOtherValues, actToTables, otherValues, traceId);
            }
        }
    }
}

void KnowledgeBase::collectValuesAmongTraces(
        std::unordered_map<std::string, std::unordered_map<std::string, std::set<union_type>>> &result,
        std::unordered_map<std::string, std::set<union_type>> &resultOtherValues,
        const std::unordered_map<std::string, std::unordered_set<std::string>> &actToTables,
        const std::unordered_set<std::string> &otherValues, trace_t traceId) const {
    const auto& ref = act_table_by_act_id.secondary_index[traceId];
    ActTable::record* ptr = ref.first;
    while (ptr) {
        std::string sAct = event_label_mapper.get(ptr->entry.id.parts.act);
        auto it = actToTables.find(sAct);
        bool hasId = it != actToTables.end();
        if ((!resultOtherValues.empty()) || (hasId)) {
            for (const auto& attr_table : attribute_name_to_table) {
                ptrdiff_t offset = ptr - act_table_by_act_id.table.data();
                const AttributeTable::record* recordPtr = attr_table.second.resolve_record_if_exists(offset);
                if (recordPtr) {
                    if (hasId && it->second.contains(attr_table.first)) {
                        result[sAct][attr_table.first].insert(attr_table.second.resolve(*recordPtr));
                    }
                    if (otherValues.contains(attr_table.first)) {
                        resultOtherValues[attr_table.first].insert(attr_table.second.resolve(*recordPtr));
                    }
                }
            }
        }
        ptr = ptr->next;
    }
}

void KnowledgeBase::clear() {
    status = FinishParsing;
    source.clear();
    name.clear();
    noTraces = 0;
    count_table.clear();
    act_table_by_act_id.clear();
    attribute_name_to_table.clear();
    string_values.clear();
    noTraces = 0;
    currentEventId = 0;
    actId = 0;
    approximate_attribute_to_table.clear();
}

#include <sstream>

// TODO: to be replaced with Sam's function
template<typename InputIt1, typename InputIt2, typename OutputIt, typename Aggregation>
OutputIt tmp_set_union(InputIt1 first1, InputIt1 last1,
                       InputIt2 first2, InputIt2 last2,
                       OutputIt d_first, Aggregation aggr)
{
    for (; first1 != last1; ++d_first) {
        if (first2 == last2)
            return std::copy(first1, last1, d_first);
        if (first2->first < first1->first) {
            *d_first = *first2++;
        } else if (first2->first == first1->first) {
            *d_first = std::make_pair(first2->first, aggr(first1->second, first2->second));
            *first2++;
            *first1++;
        } else {
            *d_first = *first1++;
        }
    }
    return std::copy(first2, last2, d_first);
}

std::vector<std::pair<std::pair<trace_t, event_t>, double>>
KnowledgeBase::range_query(DataPredicate prop, double min_threshold, const double c) const {
    if (prop.casusu == TTRUE)
        return universe; // Immediately returning the universe queries
    else
        prop.asInterval();
    constexpr size_t max_int = std::numeric_limits<size_t>::max();
    DEBUG_ASSERT(!prop.var.empty());
    DEBUG_ASSERT((min_threshold >= std::numeric_limits<double>::epsilon()) && (min_threshold <= 1.0)); // Cannot have a negative approximation, as the approximation is just a distance
    if (!prop.labelRHS.empty()) {
        std::stringstream sstr;
        sstr << "Predicate " << prop << ": cannot have a predicate over two distinct variables! ";
        throw std::runtime_error(sstr.str());
    }
    if (!prop.exceptions.empty()) {
        std::stringstream sstr;
        sstr << "Predicate " << prop << ": cannot have excepted values at this stage: this should be an already decomposed interval ";
        throw std::runtime_error(sstr.str());
    }
    if (!prop.BiVariableConditions.empty()) {
        std::stringstream sstr;
        sstr << "Predicate " << prop << ": cannot variable conditions that are proper to join conditions: either the decomposition is faulty, or the data clearing is not effective, or the interval decomposition isn't, or the interval is used inappropriately ";
        throw std::runtime_error(sstr.str());
    }

    auto tmp = range_query(prop, min_threshold, 1.0, c, true);
    if (tmp.first == 1)
        // isuniverse, already for the exact! any union will always return the universe
        return universe;
    else if (tmp.first == 0 && (min_threshold<1.0)) {
        // if the answer is empty and we want to return an approximated solution, the solution is only over non
        // existing data
        auto tmp2 = range_query(prop, min_threshold, maximum_reliability_for_insertion, c, false);
        if (tmp2.first == 1)
            return universeApprox;
        else if (tmp.first == 0)
            return empty;
        else return tmp.second;
    } else if ((!approximate_attribute_to_table.empty()) && (min_threshold < 1.0)) {
        ///return tmp.second;
        auto tmp2 = range_query(prop, min_threshold, maximum_reliability_for_insertion, c, false);
        if (tmp2.first == 1) {
            // performs the union with the approximated universe: TODO: replace tmp_set_union with Sam's
            std::vector<std::pair<std::pair<trace_t, event_t>, double>> Result;
            tmp_set_union(tmp.second.begin(), tmp.second.end(),
                          universeApprox.begin(), universeApprox.end(),
                          std::back_inserter(Result), [](double x, double y) {return std::max(x,y);});
            return Result;
        } else if (tmp.first == 0)
            // return the original data directly, with no union
            return tmp.second;
        else {
            // performs he union with the two datasets. TODO: replace tmp_set_union with Sam's
            std::vector<std::pair<std::pair<trace_t, event_t>, double>> Result;
            tmp_set_union(tmp.second.begin(), tmp.second.end(),
                          tmp2.second.begin(), tmp2.second.end(),
                          std::back_inserter(Result), [](double x, double y) {return std::max(x,y);});
            return Result;
        }
    } else {
        return tmp.second;
    }
}

std::pair<int, std::vector<std::pair<std::pair<trace_t, event_t>, double>>>
KnowledgeBase::range_query(DataPredicate &prop,
                           double min_threshold,
                           double correction,
                           const double c,
                           bool forExistingData) const {

    //static const double at16 = std::pow(2, 16);
    auto it = (forExistingData ? attribute_name_to_table : approximate_attribute_to_table).find(prop.var);
    if (it == (forExistingData ? attribute_name_to_table : approximate_attribute_to_table).end()) {
        // if no attribute is there, then I must assume that all of the traces are valid!
        return {1, {}}; // Immediately returning the universe queries
    } else {
        // The attribute exists within the dataset
        ssize_t act_id = -1;
        if (!prop.label.empty()) {
            act_id = (ssize_t) event_label_mapper.get(prop.label);
        }
        auto tmp = it->second.range_query(prop, act_id, min_threshold, c);
        if (tmp.isUniverse())
            return {1, {}}; // Immediately returning the universe queries
        else if (tmp.isEmptySolution())
            return {0, {}}; // Return empty solution
        else {
            std::vector<std::pair<std::pair<trace_t, event_t>, double>> S;
            for (const auto& element : tmp._data) {
                if (element.exact_solution.first != nullptr) {
                    size_t N = std::distance(element.exact_solution.first, element.exact_solution.second);
                    for (size_t i = 0; i<=N; i++) {
                        const auto& exactIt = element.exact_solution.first[i];
                        const auto& resolve = act_table_by_act_id.table.at(exactIt.act_table_offset).entry.id.parts;
                        S.emplace_back(std::make_pair(resolve.trace_id,resolve.event_id
                                /*trunc((((double)resolve.event_id)/at16) *
                                      act_table_by_act_id.getTraceLength(resolve.trace_id))*/), 1.0 * correction);
                    }

                }

                for (auto item : element.approx_solution) {
                    const auto& resolve = act_table_by_act_id.table.at(item.first->act_table_offset).entry.id.parts;
                    S.emplace_back(std::make_pair(resolve.trace_id,resolve.event_id
                            /*trunc((((double)resolve.event_id)/at16) *
                                  act_table_by_act_id.getTraceLength(resolve.trace_id))*/), item.second * correction);

                }
            }
            std::sort(S.begin(), S.end());
            S.erase(std::unique(S.begin(), S.end()), S.end());
            return {2, S};
        }
    }
}



void KnowledgeBase::load_data_without_antlr4(const KnowledgeBase::no_antlr_log &L, const std::string &source,
                                             const std::string &name) {
    enterLog(source, name);
    for (auto const& trace : L ){ //sigma_i
        size_t strace_id = enterTrace("bogus");
        size_t chronos = 0;
        enterData_part(false);
        exitData_part(false);
        for (auto const & event : trace) {
            auto const act = event.first;
            size_t event_id = enterEvent(chronos++, act);
            enterData_part(true);
            for (auto const& cp : event.second) {
                if (std::holds_alternative<bool>(cp.second)) {
                    visitField(cp.first, std::get<bool>(cp.second));
                } else if (std::holds_alternative<double>(cp.second)) {
                    visitField(cp.first, std::get<double>(cp.second));
                } else if (std::holds_alternative< std::string>(cp.second)) {
                    visitField(cp.first, std::get< std::string>(cp.second));
                }else if (std::holds_alternative< size_t>(cp.second)) {
                    visitField(cp.first, std::get< size_t>(cp.second));
                }
            }
            exitData_part(true);
            exitEvent(event_id);
        }
        exitTrace(strace_id);
    }
    exitLog(source, name);
}

uint16_t KnowledgeBase::getMappedValueFromAction(const std::string &act) const {
    try{
        return event_label_mapper.get(act);
    }
    catch(const std::exception& e){
        return -1;
    }
}

std::pair<const uint32_t, const uint32_t> KnowledgeBase::resolveCountingData(const std::string &act) const {
    const uint16_t& mappedVal = getMappedValueFromAction(act);

    if(mappedVal < 0){
        return {-1, -1};
    }

    return count_table.resolve_primary_index(mappedVal);
}

std::vector<std::pair<std::pair<trace_t, event_t>, double>> KnowledgeBase::untimed_dataless_exists(const std::pair<const uint32_t, const uint32_t>& indexes,
                                                                                                   const uint16_t& amount) const {
    std::vector<std::pair<std::pair<trace_t, event_t>, double>> foundElems;

    if ((indexes.first == indexes.second) && (indexes.first == (uint32_t)-1))
        return foundElems;

    for (auto it = count_table.table.begin() + indexes.first; it != count_table.table.begin() + indexes.second + 1; ++it) {
        //uint16_t approxConstant = act_table_by_act_id.getTraceLength(it->id.parts.trace_id) / 2;
        //double satisfiability = getSatisifiabilityBetweenValues(amount, it->id.parts.event_id, approxConstant);
        if (it->id.parts.event_id >= amount)
            foundElems.emplace_back(std::pair<trace_t, event_t>{it->id.parts.trace_id, 0}, 1.0);
    }

    return foundElems;
}


Result KnowledgeBase::timed_dataless_exists(const std::string &act, LeafType leafType) const {
    Result foundData;
    ResultRecord result{{0,0}, {1.0, {}}};
//    std::pair<uint32_t, uint16_t> timePair;
//    std::pair<double, std::vector<uint16_t>> dataPair{1.0, {}};

    bool marked_event_type = (((short)leafType)>=1) && (((short)leafType)<=3);
    switch (leafType) {
        case ActivationLeaf:
            result.second.second.push_back(marked_event::activation(0));
            break;
        case TargetLeaf:
            result.second.second.push_back(marked_event::target(0));
            break;
        case MatchActivationTarget:
            result.second.second.push_back(marked_event::join(0, 0));
            break;
        default:
            break;
    }

    const uint16_t& mappedVal = getMappedValueFromAction(act);
    if(mappedVal < 0){
        return foundData;
    }
    std::pair<const uint32_t , const uint32_t> indexes = act_table_by_act_id.resolve_index(mappedVal);
    if(indexes.first < 0){
        return foundData;
    }
    for (auto it = act_table_by_act_id.table.begin() + indexes.first; it != act_table_by_act_id.table.begin() + indexes.second + 1; ++it) {
        result.first.first = it->entry.id.parts.trace_id;
        result.first.second = it->entry.id.parts.event_id;

        if (marked_event_type) {
            auto& ref = result.second.second[0];
            SET_EVENT(ref, result.first.second);
        }

//        if (markEventsForMatch)
//            dataPair.second[0] = timePair.second;
        foundData.emplace_back(result);
    }
    return foundData;
}

float KnowledgeBase::getSatisifiabilityBetweenValues(const uint16_t& val1, const uint16_t& val2, const uint16_t& approxConstant) const {
    return 1 / (((float)std::abs(val1 - val2) / approxConstant) + 1);
}


uint16_t KnowledgeBase::getPositionFromEventId(const std::pair<uint32_t, uint16_t> pair) const {
    uint16_t traceLength = act_table_by_act_id.getTraceLength(pair.first);

    /* Guard against length 1 traces */
    if(traceLength == 1){
        return 0;
    }

    uint16_t posFromEventId = std::ceil((float)(pair.second / (float)MAX_UINT16) * (traceLength - 1));
    return posFromEventId;
}

uint16_t KnowledgeBase::getPositionFromEventId(const oid* event) const {
    uint16_t traceLength = act_table_by_act_id.getTraceLength(event->id.parts.trace_id);

    /* Guard against length 1 traces */
    if(traceLength == 1){
        return 0;
    }

    uint16_t posFromEventId = std::ceil((float)(event->id.parts.event_id / (float)MAX_UINT16) * (traceLength - 1));
    return posFromEventId;
}


std::pair<std::unordered_map<std::string, AttributeTable>::iterator,
        std::unordered_map<std::string, AttributeTable>::iterator> KnowledgeBase::getAttrNameTableIt() {
    return {attribute_name_to_table.begin(), attribute_name_to_table.end()};
}

union_type KnowledgeBase::resolveRecord(const ActTable::record *eventFromTrace,
                                        const std::unordered_map<std::string, AttributeTable>::iterator &attr_table) const {

    return attr_table->second.resolve(*attr_table->second.resolve_record_if_exists(eventFromTrace - act_table_by_act_id.table.data()));
}

union_minimal KnowledgeBase::resolveMinimalRecord(const ActTable::record *eventFromTrace,
                                                  const std::unordered_map<std::string, AttributeTable>::iterator &attr_table) const {
    return resolveUnionMinimal(attr_table->second,
                               *attr_table->second.resolve_record_if_exists(eventFromTrace - act_table_by_act_id.table.data()));
}

void KnowledgeBase::print_count_table(std::ostream &os) const {
    os << count_table;
}

void KnowledgeBase::print_act_table(std::ostream &os) const {
    os << act_table_by_act_id;
}

void KnowledgeBase::print_attribute_tables(std::ostream &os) const {
    for (const auto& ref : attribute_name_to_table)
        os << ref.second;

    if (!approximate_attribute_to_table.empty())
        os << " ~~~~~~~~~~~~~~~~~~~~ APPROXIMATIONS ~~~~~~~~~~~~~~~~~~~~ ";
    for (const auto& ref : approximate_attribute_to_table)
        os << ref.second;
}

std::vector<std::pair<trace_t, event_t>> KnowledgeBase::exact_range_query(DataPredicate prop) const {
    DEBUG_ASSERT(prop.casusu != TTRUE);
    prop.asInterval();
    constexpr size_t max_int = std::numeric_limits<size_t>::max();
    DEBUG_ASSERT(!prop.var.empty());
    DEBUG_ASSERT(prop.labelRHS.empty());
    DEBUG_ASSERT(prop.exceptions.empty());
    DEBUG_ASSERT(prop.BiVariableConditions.empty());
    auto it = attribute_name_to_table.find(prop.var);
    if (it == attribute_name_to_table.end()) {
        // if no attribute is there, for the exact match I assume that no value was matched
        return {};
    } else {
        // The attribute exists within the dataset
        ssize_t act_id = -1;
        if (!prop.label.empty()) {
            act_id = (ssize_t) event_label_mapper.get(prop.label);
        }
        auto tmp = it->second.exact_range_query(act_id, prop);
        if ((tmp.first == tmp.second) && tmp.first == nullptr )
            return {}; // Return empty solution
        else {
            std::vector<std::pair<trace_t, event_t>> S;
            size_t N = std::distance(tmp.first, tmp.second);
            for (size_t i = 0; i<=N; i++) {
                const auto& exactIt = tmp.first[i];
                const auto& resolve = act_table_by_act_id.table.at(exactIt.act_table_offset).entry.id.parts;
                S.emplace_back(resolve.trace_id,resolve.event_id);
            }
            std::sort(S.begin(), S.end());
            S.erase(std::unique(S.begin(), S.end()), S.end());
            return S;
        }
    }
}

void KnowledgeBase::exact_range_query(const std::string &field_name,
                                      const std::unordered_map<std::string, std::vector<size_t>> &ActNameToPredicates,
                                      std::vector<std::pair<DataQuery, PartialResult>> &Qs,
                                      const std::optional<uint16_t> &temporalTimeMatch) const {

    bool doTemporalMatchQuery = temporalTimeMatch.has_value();
    uint16_t isTemproalVal = temporalTimeMatch.value_or(0);
    auto it = attribute_name_to_table.find(field_name);
    if (it == attribute_name_to_table.end()) {
        // if no attribute is there, for the exact match I assume that no value was matched
        return;
    } else {
        // The attribute exists within the dataset
        std::vector<std::pair<size_t, std::vector<DataQuery*>>> V;
        std::unordered_map<DataQuery*, size_t> qToItsId;
        for (const auto& mapRef : ActNameToPredicates) {
            std::pair<size_t, std::vector<DataQuery*>>& DQ = V.emplace_back(event_label_mapper.get(mapRef.first), std::vector<DataQuery*>{});
            for (const auto& qId : mapRef.second) {
                auto& prop = Qs.at(qId).first;
                auto it = qToItsId.emplace(&prop, qId);
                DEBUG_ASSERT(it.second);
                DQ.second.emplace_back(&prop);
            }
            // I do not need to compare the pointers, rather than compare the values associated to those
            std::sort(DQ.second.begin(), DQ.second.end(), [](auto lhs, auto rhs) {return *lhs < *rhs;});
        }

        // TODO: this is just the concrete doing.
        auto tmp = it->second.exact_range_query(V);

        for (size_t i = 0, N = V.size(); i<N; i++) {
            const auto& actIdToPropList = V.at(i);
            auto v_actId = actIdToPropList.first;
            auto& v_propList = actIdToPropList.second;
            auto& tmpResult = tmp.at(i);
            DEBUG_ASSERT(v_propList.size() == tmpResult.size()); // We return the result for each query
            for (size_t j = 0, M = v_propList.size(); j<M; j++) {
                auto& tmpRef = tmpResult.at(j);
                if (!((tmpRef.first == tmpRef.second) && tmpRef.first == nullptr )) {
                    auto qPTr = v_propList.at(j);
                    auto qId = qToItsId.at(qPTr);
                    auto& refQ = Qs.at(qId);
                    DEBUG_ASSERT(refQ.first == *qPTr); // They are the same query
//                    LeafType qT = refQ.first.t;
                    PartialResult& S = refQ.second;
                    size_t N = std::distance(tmpRef.first, tmpRef.second);
                    for (size_t i = 0; i<=N; i++) {
                        const auto& exactIt = tmpRef.first[i];
                        const auto& resolve = act_table_by_act_id.table.at(exactIt.act_table_offset).entry.id.parts;
// TODO: for the approximate match, in the later future
//                        bool doInsert = true;
//                        float satisfiability = 1.0;
//                        if (doTemporalMatchQuery) {
//                            auto L = resolve.event_id;
//                            satisfiability = getSatisifiabilityBetweenValues(((L <= 1) ? 0 : isTemproalVal),
//                                                                             cast_to_float2(resolve.event_id,L), approxConstant);
//                            doInsert = satisfiability >= 1.0;
//                        }
//                        if (doInsert)
                        S.emplace_back(std::pair<trace_t,event_t>{resolve.trace_id, resolve.event_id},
                                       /*std::pair<double,std::vector<uint16_t>>{*/1.0/*, W}*/);
                    }
                    std::sort(S.begin(), S.end());
                    S.erase(std::unique(S.begin(), S.end()), S.end());
                }
                //j++;
            }
        }

//        size_t j = 0;
//        for (const auto& mapRef : ActNameToPredicates) {
//            for (const auto& qId : mapRef.second) {
//                auto& tmpRef = tmp.at(j++);// TODO: fix, funziona solo per puro caso
//                if ((tmpRef.first == tmpRef.second) && tmpRef.first == nullptr ) continue;
//                else {

//                }
//            }
//        }
    }
}

const Result KnowledgeBase::getLastElements(LeafType leafType) const {
    Result elems{};
    ResultRecord result{{0,0}, {1, {}}};
    bool marked_event_type = (((short)leafType)>=1) && (((short)leafType)<=3);
    switch (leafType) {
        case ActivationLeaf:
            result.second.second.push_back(marked_event::activation(0));
            break;
        case TargetLeaf:
            result.second.second.push_back(marked_event::target(0));
            break;
        case MatchActivationTarget:
            result.second.second.push_back(marked_event::join(0, 0));
            break;
        default:
            break;
    }

    for (const std::pair<ActTable::record *, ActTable::record *> &rec: act_table_by_act_id.secondary_index) {
        result.first.first = rec.second->entry.id.parts.trace_id;
        result.first.second = rec.second->entry.id.parts.event_id;
        result.first.second = getPositionFromEventId(result.first);
        ///const std::pair<uint32_t, uint16_t> traceEventPair{traceId, eventId};

        if (marked_event_type) {
            auto& ref = result.second.second[0];
            SET_EVENT(ref, result.first.second);
        }
        elems.push_back(result);
    }

    return elems;
}

const Result KnowledgeBase::getNotFirstElements(LeafType leafType) {
    Result elems{};
    ResultRecord result{{0,0}, {1, {}}};
    bool marked_event_type = (((short)leafType)>=1) && (((short)leafType)<=3);
    switch (leafType) {
        case ActivationLeaf:
            result.second.second.push_back(marked_event::activation(0));
            break;
        case TargetLeaf:
            result.second.second.push_back(marked_event::target(0));
            break;
        case MatchActivationTarget:
            result.second.second.push_back(marked_event::join(0, 0));
            break;
        default:
            break;
    }

    auto itr = act_table_by_act_id.secondary_index.begin();
    while (itr != act_table_by_act_id.secondary_index.end()) {
        auto currentElem = itr->first;

        while (currentElem = currentElem->next) {
            result.first.first = currentElem->entry.id.parts.trace_id;
            result.first.second = currentElem->entry.id.parts.event_id;
            result.first.second = getPositionFromEventId(result.first);

            if (marked_event_type) {
                auto& ref = result.second.second[0];
                SET_EVENT(ref, result.first.second);
            }
            elems.push_back(result);
        }

        ++itr;
    }

    return elems;
}

Result KnowledgeBase::init(const std::string &act, bool doExtractEvent, const double minThreshold) const {
    return initOrEnds(act, true, doExtractEvent, minThreshold);
}

Result KnowledgeBase::ends(const std::string &act, bool doExtractEvent, const double minThreshold) const {
    return initOrEnds(act, false, doExtractEvent, minThreshold);
}

Result
KnowledgeBase::initOrEnds(const std::string &act, bool beginOrEnd, bool doExtractEvent, const double minThreshold) const {
    Result foundData;

    Result tracePair;
    const uint16_t& mappedVal = getMappedValueFromAction(act);

    if(mappedVal < 0){
        return foundData;
    }

    ResultIndex eventPair;
    ResultRecordSemantics dataPair{1.0, {}};
    if (doExtractEvent) dataPair.second.emplace_back(marked_event::left(0));
    std::pair<const uint32_t , const uint32_t> indexes = act_table_by_act_id.resolve_index(mappedVal);

    if(indexes.first < 0){
        return foundData;
    }

    for (auto it = act_table_by_act_id.table.begin() + indexes.first; it != act_table_by_act_id.table.begin() + indexes.second + 1; ++it) {
        uint16_t approxConstant = MAX_UINT16 / 2;
        auto eventId = beginOrEnd ? 0 : act_table_by_act_id.getTraceLength(it->entry.id.parts.trace_id)-1;

        float satisfiability = getSatisifiabilityBetweenValues(eventId, it->entry.id.parts.event_id, approxConstant);

        if(satisfiability >= minThreshold) {
            dataPair.first = satisfiability;
            eventPair.first = it->entry.id.parts.trace_id;
            eventPair.second = it->entry.id.parts.event_id;
            if (doExtractEvent) dataPair.second[0].id.parts.left = eventPair.second;
            foundData.emplace_back(eventPair, dataPair);
        }
    }

    return foundData;
}

PartialResult KnowledgeBase::timed_dataless_exists(const std::string &act) const {
    PartialResult foundData;
    ResultIndex timePair;
    uint16_t mappedVal = getMappedValueFromAction(act);
    if(mappedVal == (uint16_t)-1){
        return foundData;
    }
    auto indexes = act_table_by_act_id.resolve_index(mappedVal);
    if(indexes.first < 0){
        return foundData;
    }
    for (auto it = act_table_by_act_id.table.begin() + indexes.first; it != act_table_by_act_id.table.begin() + indexes.second + 1; ++it) {
        timePair.first = it->entry.id.parts.trace_id;
        timePair.second = it->entry.id.parts.event_id;
        foundData.emplace_back(timePair, 1.0);
    }
    return foundData;
}

PartialResult
KnowledgeBase::untimed_dataless_absence(const std::pair<const uint32_t, const uint32_t> &indexes, const event_t &amount) const {
    PartialResult foundElems;

    if ((indexes.first == indexes.second) && (indexes.first == (uint32_t)-1))
        return foundElems;

    for (auto it = count_table.table.begin() + indexes.first; it != count_table.table.begin() + indexes.second + 1; ++it) {
        //uint16_t approxConstant = act_table_by_act_id.getTraceLength(it->id.parts.trace_id) / 2;
        //double satisfiability = getSatisifiabilityBetweenValues(amount, it->id.parts.event_id, approxConstant);
        if (it->id.parts.event_id < amount)
            foundElems.emplace_back(std::pair<trace_t, event_t>{it->id.parts.trace_id, 0}, 1.0);
    }

    return foundElems;
}

void KnowledgeBase::dump_for_sqlminer(std::ostream &log, std::ostream &payload, std::ostream &schema_configuration) {
    constexpr size_t record_size = sizeof(ActTable::record);
    size_t eventId = 0;
    // Writing the schema associated to the payload
    schema_configuration << "event_id\tbigint" << std::endl;
    for (const auto kv : this->attribute_name_to_table) {
        std::string data = kv.first;
        // convert string to back to lower case
        std::for_each(data.begin(), data.end(), [](char & c) {
            c = ::tolower(c);
        });
        schema_configuration << data << '\t';
        switch (kv.second.type) {
            case DoubleAtt:
                schema_configuration <<"real" << std::endl;
                break;
            case SizeTAtt:
            case LongAtt:
                schema_configuration <<"bigint" << std::endl;
                break;
                break;
            case StringAtt:
                schema_configuration <<"varchar" << std::endl;
                break;
            case BoolAtt:
                schema_configuration <<"boolean" << std::endl;
                break;
        }
    }
    for (size_t trace_id = 0, N = act_table_by_act_id.secondary_index.size(); trace_id < N; trace_id++) {
        //os << "Trace #" << trace_id << std::endl << "\t- ";
        const auto& ref = act_table_by_act_id.secondary_index[trace_id];
        auto ptr = ref.first;
        size_t time = 0;
        while (ptr) {
            size_t offset = (((size_t)ptr) - ((size_t)act_table_by_act_id.table.data()));
            offset = offset / record_size;
            // printing one event at a time
            log << eventId << "\t" << trace_id << "\t" << event_label_mapper.get(ptr->entry.id.parts.act) << "\t" << time << std::endl;
            payload << eventId << "\t";
            auto it = this->attribute_name_to_table.begin(), en = this->attribute_name_to_table.end();
            while (it != en) {
                const AttributeTable::record* recordPtr = it->second.resolve_record_if_exists(offset);
                if (recordPtr) {
                    DEBUG_ASSERT(recordPtr->act_table_offset == offset);
                    it->second.resolve_and_print(payload, *recordPtr);
                }
                it++;
                if (it != en)
                    payload << "\t";
                else
                    payload << std::endl;
            }
            ptr = ptr->next;
            eventId++;
            time++;
        };
    }
}

PartialResult KnowledgeBase::getFirstLastOtherwise(const bool isFirst) const {
    PartialResult elems{};
    PartialResultRecord traceEventPair{{0,0}, 1.0};
    for (const std::pair<ActTable::record *, ActTable::record *> &rec: act_table_by_act_id.secondary_index) {
        traceEventPair.first.first = isFirst ? rec.first->entry.id.parts.trace_id : rec.second->entry.id.parts.trace_id;
        traceEventPair.first.second = isFirst ? rec.first->entry.id.parts.event_id : rec.second->entry.id.parts.event_id;
        elems.push_back(traceEventPair);
    }
    return elems;
}


