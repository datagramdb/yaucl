//
// Created by giacomo on 26/12/20.
//

#include "knobab/server/tables/AttributeTable.h"
#include "SimplifiedFuzzyStringMatching.h"
#include <cassert>

static inline union_type cast_unions(AttributeTableType type, const union_minimal &x) {
    switch (type) {
        case DoubleAtt:
            return std::get<double>(x);
        case LongAtt:
            return (long long)std::get<double>(x);
        case StringAtt:
            return  std::get<std::string>(x);
        case BoolAtt:
            return std::abs(std::get<double>(x)) > std::numeric_limits<double>::epsilon();
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return (size_t)std::get<double>(x);
    }
}

static inline double similarityFunction(const union_type& lhs, const union_type& rhs, double c = 2.0) {
    if (lhs.index() != rhs.index())
        return std::numeric_limits<double>::max(); // If the elements are not of the same type, then they are associated to the maximum distance.
    if (std::holds_alternative<double>(lhs)) {
        return 1.0 / ( (std::abs(std::get<double>(lhs)- std::get<double>(rhs)))/c + 1.0);
    } else if (std::holds_alternative<size_t>(lhs)) {
        auto l = std::get<size_t>(lhs);
        auto r = std::get<size_t>(rhs);
        return 1.0 / ( ((double)(((l > r) ? (l-r) : (r-l))))/c + 1.0);
    } else if (std::holds_alternative<long long>(lhs)) {
        return 1.0 / ( (std::abs((double)std::get<long long>(lhs)- (double)std::get<long long>(rhs)))/c + 1.0);
    }

}

union_type AttributeTable::resolve(const AttributeTable::record &x) const {
    switch (type) {
        case DoubleAtt:
            return *(double*)(&x.value);
        case LongAtt:
            return *(long long*)(&x.value);
        case StringAtt:
            return ptr.get(x.value);
        case BoolAtt:
            return x.value != 0;
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return x.value;
    }
}

bool AttributeTable::assertVariant(const std::variant<double, size_t, long long int, std::string, bool> &val) {
    switch (type) {
        case DoubleAtt:
            return std::holds_alternative<double>(val);
        case SizeTAtt:
            return std::holds_alternative<size_t>(val);
        case LongAtt:
            return std::holds_alternative<long long>(val);
        case StringAtt:
            return std::holds_alternative<std::string>(val);
        case BoolAtt:
            return std::holds_alternative<bool>(val);
        default:
            return false;
    }
}

void
AttributeTable::record_load(act_t act_id, const std::variant<double, size_t, long long int, std::string, bool> &val,
                            trace_t tid, event_t eid) {
    DEBUG_ASSERT(assertVariant(val));
    while (elements.size() <= act_id)
        elements.emplace_back();
    elements[act_id][val].emplace_back(tid, eid);
}

size_t AttributeTable::storeLoad(const std::variant<double, size_t, long long int, std::string, bool> &x) {
    switch (type) {
        case DoubleAtt: {
            double tmp = std::get<double>(x);
            return *(size_t*)(&tmp);
        }

        case LongAtt: {
            long long tmp = std::get<long long>(x);
            return *(size_t*)(&tmp);
        }

        case StringAtt: {
            std::string tmp = std::get<std::string>(x);
            return ptr.put(tmp).first;
        }
        case BoolAtt:
            return std::get<bool>(x) ? 1 : 0;
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return std::get<size_t>(x);
    }
}

void AttributeTable::index(const std::vector<std::vector<size_t>> &trace_id_to_event_id_to_offset) {
    for (size_t act_id = 0, N = elements.size(); act_id < N; act_id++) {
        auto& ref = elements[act_id];
        size_t begin = table.size();
        if (!ref.empty()) {
            std::map<union_type, std::vector<size_t>> valueToOffsetInTable;
            for (const auto& val_offset : ref) {
                for (const auto& traceid_eventid : val_offset.second) {
                    size_t offset =
                            trace_id_to_event_id_to_offset.at(traceid_eventid.first).at(traceid_eventid.second);

                    valueToOffsetInTable[val_offset.first].emplace_back(offset);
                }
            }
            for (auto it = valueToOffsetInTable.begin(); it != valueToOffsetInTable.end(); it++) {
                std::sort(it->second.begin(), it->second.end());
                size_t val = storeLoad(it->first);
                std::string current_string;
                if (type == StringAtt)
                    current_string = std::get<std::string>(it->first);
                for (const auto& refx : it->second) {
                    if (type == StringAtt)
                        string_offset_mapping[current_string].emplace_back(table.size());
                    secondary_index[refx] = table.size();
                    table.emplace_back(act_id, val, refx);
                }
//                it = valueToOffsetInTable.erase(it);
            }
            valueToOffsetInTable.clear();
            ref.clear();
        }
        size_t end = table.size();
        primary_index.emplace_back(begin, end);
    }
    elements.clear();
}

const AttributeTable::record * AttributeTable::resolve_record_if_exists(size_t actTableOffset) const {
    auto it = secondary_index.find(actTableOffset);
    if (it == secondary_index.end())
        return nullptr;
    else
        return (table.data()) + it->second;
}

std::ostream &AttributeTable::resolve_and_print(std::ostream &os, const AttributeTable::record &x) const {
    switch (type) {
        case DoubleAtt:
            return os << *(double*)(&x.value);
        case LongAtt:
            return os << *(long long*)(&x.value);
        case StringAtt:
            return os << ptr.get(x.value);
        case BoolAtt:
            return os << ((x.value != 0) ? "true" : "false");
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return os << x.value;
    }
}

#include <sstream>
#include <magic_enum.hpp>
#include <yaucl/structures/set_operations.h>
#include <yaucl/functional/assert.h>


AttributeTable::range_query_result AttributeTable::range_query(DataPredicate prop, ssize_t act, double min_threshold, const double c) const {
    // Just for consistency checking, I need to evaluate the predicate over the specific table. This is also for efficiency reasons
    // Plus, this has to be checked only if the predicate is not the always true predicate, forsooth!

    DEBUG_ASSERT((prop.casusu != TTRUE)); // Universal queries should be handled at the higher level: id est, I do not even attempt at returning the data if I know that
    // everything is always true!
    DEBUG_ASSERT((prop.var == this->attr_name));
    bool isNotExactMatch = min_threshold < 1.0;
    if ((prop.casusu != INTERVAL)) {
        prop.asInterval();// If it is not an interval, make that as so, so we need to deal with only one case!
    }

    // At this stage, we know for sure that this element should be an interval at most.
    // Furthermore, given that I never handle always true queries, it is never an implicit
    // universe solution.
    AttributeTable::range_query_result result{false};
    if (act == -1) {
        for (size_t actId = 0, N = primary_index.size(); actId < N; actId++) {
            range_query(actId, prop, result, isNotExactMatch, min_threshold, c);
        }
    } else {
        range_query(act, prop, result, isNotExactMatch, min_threshold, c);
    }
    return result;
}

bool AttributeTable::range_query(size_t actId,
                                 const DataPredicate &prop,
                                 AttributeTable::range_query_result &result,
                                 bool isNotExactMatch,
                                 double min_threshold,
                                 double c) const {
    DEBUG_ASSERT(prop.casusu == INTERVAL); // At this stage, I should only have interval queries!
    if (actId > primary_index.size()) return false; // missing act
    auto it = primary_index.at(actId);
    if (it.first == it.second)
        return false; // missing attribute ~ for the meantime, we are not approximating the match on the attribute name, but we should in the future
    {
        DEBUG_ASSERT(table.size() >= it.first);
        DEBUG_ASSERT(table.size() >= it.second);

        const union_type prop_leftValue = cast_unions(type, prop.value);
        const union_type prop_rightValue = cast_unions(type, prop.value_upper_bound);

        auto begin = table.data() + it.first;
        auto end = table.data() + it.second;

        auto lb = std::lower_bound(begin, end, prop_leftValue, [&](const record &r, const union_type &value) {
            return resolve(r) < value;
        });

        if (lb != end) {
            auto ub = std::upper_bound(begin, end, prop_rightValue, [&](const union_type &value, const record &r) {
                return resolve(r) > value;
            });

            auto &thisResult = result.emplace_back();
            auto tmpLeft = lb;
            auto tmpRight = ub;
            if (tmpRight != end) {
                tmpRight--;
            }
            else
                tmpRight = table.data() + (it.second-1);

            if (std::distance(tmpLeft, tmpRight) < 0) {
                return false;
            }
            ssize_t left = std::distance(begin, tmpLeft);
            ssize_t right = std::distance(begin, tmpRight);

            thisResult.exact_solution.first = tmpLeft;
            thisResult.exact_solution.second = tmpRight;

            if (isNotExactMatch) {
                // this computation shall be performed only if we also need to provide the approximated match
                if (type == StringAtt) {
                    SimplifiedFuzzyStringMatching sfzm;
                    for (auto i = left; i <= right; ++i) {
                        // Filling the approximate match element with the items of interest
                        sfzm.put(get<std::string>(resolve(table[it.first + i])));
                    }

                    // Retrieving the best candidate for each element of the interval: we cannot do better than this...
                    for (auto i = 0; i < left; ++i) {
                        getIterator(min_threshold, thisResult, sfzm, &table.at(it.first + i));
                    }
                    for (auto i = right+1; i < ((ssize_t)(it.second-it.first)); ++i) {
                        getIterator(min_threshold, thisResult, sfzm, &table.at(it.first + i));
                    }
                } else {
                    for (ssize_t i = ((ssize_t)left)-1; i >= ((ssize_t)0); i--) {
                        double thisValue = similarityFunction(resolve(table.at(it.first+i)), prop_leftValue, c);
                        if (thisValue < min_threshold)
                            break; // Stop the iteration if we reached the maximum part
                        thisResult.approx_solution.emplace_back(&table.at(it.first+i), thisValue);
                    }
                    for (ssize_t i = ((ssize_t)(it.second-it.first))-1; i > right; i--) {
                        double thisValue = similarityFunction(resolve(table.at(it.first+i)), prop_rightValue, c);
                        if (thisValue < min_threshold)
                            break; // Stop the iteration if we reached the maximum part
                        thisResult.approx_solution.emplace_back(&table.at(it.first+i), thisValue);
                    }
                }
                remove_duplicates(thisResult.approx_solution);
            }
        } else {
            // No solution found! only approximations are admissable.
            if (type == StringAtt) {
                // Instead of scanning the whole records, just use ptr, and then perform the intersection!
                SimplifiedFuzzyStringMatching sfzm;
                for (auto i = it.first; i < it.second; ++i) {
                    // Filling the approximate match element with the items of interest
                    sfzm.put(get<std::string>(resolve(table[i])));
                }
                std::unordered_map<std::string, double> aggr;
                {
                    std::multimap<double, std::string> result;
                    sfzm.fuzzyMatch(min_threshold, 1, std::get<std::string>(prop_leftValue), result);
                    for (const auto &cp: result) {
                        auto it2 = aggr.emplace(cp.second, cp.first);
                        if (!it2.second) {
                            it2.first->second = std::max(it2.first->second, cp.first);
                        }
                    }
                }
                {
                    std::multimap<double, std::string> result;
                    sfzm.fuzzyMatch(min_threshold, 1, std::get<std::string>(prop_rightValue), result);
                    for (const auto &cp: result) {
                        auto it2 = aggr.emplace(cp.second, cp.first);
                        if (!it2.second) {
                            it2.first->second = std::max(it2.first->second, cp.first);
                        }
                    }
                }

                if (!aggr.empty()) {
                    auto &thisResult = result.emplace_back();
                    for (const auto &ref: aggr) {
                        auto it2 = string_offset_mapping.find(ref.first);
                        if (it2 != string_offset_mapping.end()) {
                            for (const size_t offset: it2->second) {
                                thisResult.approx_solution.emplace_back((const record *) (&this->table[offset]),
                                                                        ref.second);
                            }
                        }
                    }
                    remove_duplicates(thisResult.approx_solution);
                }
                return !aggr.empty();
            } else {
                disjunctive_range_query_result *current_result = nullptr;
                for (auto i = ((ssize_t)it.second) - 1; i >= ((ssize_t)it.first); i--) {
                    double thisValue = similarityFunction(resolve(table.at(i)), prop_leftValue, c);
                    if (thisValue < min_threshold)
                        break; // Stop the iteration if we reached the maximum part
                    if (!current_result) {
                        result.emplace_back();
                        current_result = result.data() + (result.size() - 1);
                    }
                    current_result->approx_solution.emplace_back(&table.at(i), thisValue);
                }
                for (auto i = it.first; i < it.second; i++) {
                    double thisValue = similarityFunction(resolve(table.at(i)), prop_rightValue, c);
                    if (thisValue < min_threshold)
                        break; // Stop the iteration if we reached the maximum part
                    if (!current_result) {
                        result.emplace_back();
                        current_result = result.data() + (result.size() - 1);
                    }
                    current_result->approx_solution.emplace_back(&table.at(i), thisValue);
                }
                if (current_result) {
                    remove_duplicates(current_result->approx_solution);
                }
                return current_result != nullptr;
            }
        }
    }
    return true;
}



std::ostream &operator<<(std::ostream &os, const AttributeTable &table) {
    const double at16 = std::pow(2, 16);
    //os << "          AttributeTable[" << table.attr_name << " : " << magic_enum::enum_name(table.type) << ']' << std::endl << "-------------------------------" << std::endl;
    size_t i = 0;
    os << "RowId,Act,Value,TableOffset" << std::endl;
    for (const auto& ref : table.table) {
        auto v = table.resolve(ref);
        os << (i++) << "," << ref.act << ",";
        switch (table.type) {
            case DoubleAtt:
                os << std::get<double>(v);
                break;
            case SizeTAtt:
                os << std::get<size_t>(v);
                break;
            case LongAtt:
                os << std::get<long long>(v);
                break;
            case StringAtt:
                os << std::get<std::string>(v);
                break;
            case BoolAtt:
                os << std::get<bool>(v) ? "TRUE" : "FALSE";
                break;
        }
        os << ", +" << ref.act_table_offset << std::endl;
    }
    //os << std::endl << "-------------------------------" << std::endl;
    return os;
}

std::optional<union_minimal> AttributeTable::resolve_record_if_exists2(size_t actTableOffset) const {
    auto ptr = resolve_record_if_exists(actTableOffset);
    if (!ptr) return {};
    else return {resolveUnionMinimal(*this, *ptr)};
}


AttributeTable::record::record(act_t act, size_t value, size_t actTableOffset) : act(act), value(value),
                                                                                 act_table_offset(actTableOffset) {}

union_minimal resolveUnionMinimal(const AttributeTable &table, const AttributeTable::record &x) {
    switch (table.type) {
        case DoubleAtt:
            return *(double*)(&x.value);
        case LongAtt:
            return (double)(*(long long*)(&x.value));
        case StringAtt:
            return table.ptr.get(x.value);
        case BoolAtt:
            return (x.value != 0 ? 0.0 : 1.0);
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return (double)x.value;
    }
}


std::pair<const AttributeTable::record *, const AttributeTable::record *>
AttributeTable::exact_range_query(size_t actId, const DataPredicate &prop) const {
    std::pair<const AttributeTable::record *, const AttributeTable::record *> thisResult{nullptr, nullptr};
    DEBUG_ASSERT(prop.casusu == INTERVAL); // At this stage, I should only have interval queries!
    if (actId > primary_index.size()) return thisResult; // missing act
    auto it = primary_index.at(actId);
    if (it.first == it.second)
        return thisResult; // missing attribute ~ for the meantime, we are not approximating the match on the attribute name, but we should in the future
    {
        DEBUG_ASSERT(table.size() >= it.first);
        DEBUG_ASSERT(table.size() >= it.second);

        const union_type prop_leftValue = cast_unions(type, prop.value);
        const union_type prop_rightValue = cast_unions(type, prop.value_upper_bound);

        const record* begin = table.data() + it.first;
        const record* end = table.data() + it.second;

        const record* lb = std::lower_bound(begin, end, prop_leftValue, [&](const record &r, const union_type &value) {
            return resolve(r) < value;
        });

        if (lb != end) {
            const record* ub = std::upper_bound(begin, end, prop_rightValue, [&](const union_type &value, const record &r) {
                return resolve(r) > value;
            });

            auto tmpLeft = lb;
            auto tmpRight = ub;
            if (tmpRight != end) {
                tmpRight--;
            }
            else
                tmpRight = table.data() + (it.second-1);

            if (std::distance(tmpLeft, tmpRight) < 0) {
                return thisResult;
            }
            ssize_t left = std::distance(begin, tmpLeft);
            ssize_t right = std::distance(begin, tmpRight);

            thisResult.first = tmpLeft;
            thisResult.second = tmpRight;

        }
    }
    return thisResult;
}

std::vector<std::vector<std::pair<const AttributeTable::record *, const AttributeTable::record *>>>
AttributeTable::exact_range_query(const std::vector<std::pair<size_t, std::vector<DataQuery*>>>& propList) const {
    std::vector<std::vector<std::pair<const AttributeTable::record *, const AttributeTable::record *>>> finalResult;
    //std::pair<const AttributeTable::record *, const AttributeTable::record *> thisResult{nullptr, nullptr};
    for (const auto& cps : propList) {
        auto& actualResult = finalResult.emplace_back();
        size_t actId = cps.first;
        if ((actId > primary_index.size())) {
            for (size_t i = 0, N = cps.second.size(); i<N; i++)
                actualResult.emplace_back(nullptr, nullptr);
        }
        auto it = primary_index.at(actId);
        if (it.first == it.second) {
            for (size_t i = 0, N = cps.second.size(); i<N; i++)
                actualResult.emplace_back(nullptr, nullptr);
        } else {
            DEBUG_ASSERT(table.size() >= it.first);
            DEBUG_ASSERT(table.size() >= it.second);
            const record* begin = table.data() + it.first;
            const record* end = table.data() + it.second;

            auto propRef = cps.second.begin(), propEnd = cps.second.end();
            do {
                const union_type prop_leftValue = cast_unions(type, (*propRef)->lower_bound);
                const union_type prop_rightValue = cast_unions(type, (*propRef)->upper_bound);

                const record* lb = std::lower_bound(begin, end, prop_leftValue, [&](const record &r, const union_type &value) {
                    return resolve(r) < value;
                });

                if (lb != end) {
                    const record* ub = std::upper_bound(begin, end, prop_rightValue, [&](const union_type &value, const record &r) {
                        return resolve(r) > value;
                    });

                    auto tmpLeft = lb;
                    auto tmpRight = ub;
                    if (tmpRight != end) {
                        tmpRight--;
                    }
                    else
                        tmpRight = table.data() + (it.second-1);

                    if (std::distance(tmpLeft, tmpRight) < 0) {
                        actualResult.emplace_back(nullptr, nullptr);
                    } else {
                        actualResult.emplace_back(tmpLeft, tmpRight);
                        begin = tmpRight;
                    }
                } else {
                    actualResult.emplace_back(nullptr, nullptr);
                }

                propRef++;
            } while (propRef != propEnd);
        }
    }
    return finalResult;
}

