//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_LTLF_OPERATORS_H
#define KNOBAB_SERVER_LTLF_OPERATORS_H


template<typename InputIt1, typename InputIt2, typename OutputIt, typename Aggregation> inline
OutputIt setUnion(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                  OutputIt d_first, Aggregation aggr, const DeclareDataAware *manager = nullptr, bool dropMatches = false) {
    env e1, e2;
    ResultIndex pair, pair1;
    bool hasMatch;
    ResultRecord result{{0, 0}, {0.0, {}}};

    for (; first1 != last1; ++d_first) {
        if (first2 == last2)
            return std::copy(first1, last1, d_first);
        if (first1->first > first2->first) {
            *d_first = *first2++;
        } else if (first1->first < first2->first) {
            *d_first = *first1++;
        } else {
            pair.first = first1->first.first;
            pair1.first = first2->first.first;
            result.first = first1->first;
            result.second.first = aggr(first1->second.first, first2->second.first);
            result.second.second.clear();
            hasMatch = false;

            if (manager) {
                for (const marked_event &elem: first1->second.second) {
                    if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                    pair.second = GET_ACTIVATION_EVENT(elem);

                    for (const marked_event &elem1: first2->second.second) {
                        if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                        pair1.second = GET_TARGET_EVENT(elem1);
                        e1 = manager->GetPayloadDataFromEvent(pair);
                        e2 = manager->GetPayloadDataFromEvent(pair1);

                        if (manager->checkValidity(e1, e2)) {
                            hasMatch = true;
                            if (!dropMatches) {
                                result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                            }
                        }
                    }
                }
            } else {
                hasMatch = true;
                if (!dropMatches) {
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                    result.second.second.insert(result.second.second.end(), first2->second.second.begin(), first2->second.second.end());
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                *d_first = result;
            }

            first1++;
            first2++;
        }
    }
    return std::copy(first2, last2, d_first);
}

template<typename InputIt1, typename InputIt2, typename OutputIt, typename Aggregation> inline
OutputIt
setUnionUntimed(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Aggregation aggr,
                const DeclareDataAware *manager = nullptr, bool dropMatches = false) {
    std::map<uint32_t, Result> group1 = GroupByKeyExtractor<InputIt1, uint32_t, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    std::map<uint32_t, Result> group2 = GroupByKeyExtractor<InputIt2, uint32_t, ResultRecord>(
            first2, last2,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    env e1, e2;
    std::pair<uint32_t, uint16_t> pair, pair1;
    auto start1 = group1.begin(), end1 = group1.end();
    auto start2 = group2.begin(), end2 = group2.end();
    ResultRecord result{{0, 0}, {0.0, {}}};
    bool hasMatch;

    for (; start1 != end1; ++d_first) {
        if (start2 == end2) {
            while (start1 != end1) {
                std::copy(start1->second.begin(), start1->second.end(), d_first);
                start1++;
            }
            return d_first;
        } else if (start1->first > start2->first) {
            std::copy(start2->second.begin(), start2->second.end(), d_first);
            start2++;
        } else if (start1->first < start2->first) {
            std::copy(start1->second.begin(), start1->second.end(), d_first);
            start1++;
        } else {
            result.first.first = pair.first = start1->first;
            result.second.first = 0.0;
            result.second.second.clear();
            pair1.first = start2->first;
            hasMatch = false;

            for (const auto &cont1: start1->second) {
                for (const auto &cont2: start2->second) {
                    result.second.first = std::max(aggr(first1->second.first, first2->second.first), result.second.first);

                    if (manager) {
                        for (const auto &elem: cont1.second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            pair.second = GET_ACTIVATION_EVENT(elem);
                            for (const auto &elem1: cont2.second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                pair1.second = GET_TARGET_EVENT(elem1);
                                e1 = manager->GetPayloadDataFromEvent(pair);
                                e2 = manager->GetPayloadDataFromEvent(pair1);

                                if (manager->checkValidity(e1, e2)) {
                                    hasMatch = true;
                                    if (!dropMatches) {
                                        result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                                    }
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        if (!dropMatches) {
                            result.second.second.insert(result.second.second.end(), cont1.second.second.begin(), cont1.second.second.end());
                            result.second.second.insert(result.second.second.end(), cont2.second.second.begin(), cont2.second.second.end());
                        }
                    }
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                *d_first = result;
            }

            start1++;
            start2++;
        }
    }

    while (start2 != end2) {
        std::copy(start2->second.begin(), start2->second.end(), d_first);
        start2++;
    }
    return d_first;
}

template<typename InputIt1, typename InputIt2, typename OutputIt, typename Aggregation> inline
OutputIt setIntersection(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         OutputIt d_first, Aggregation aggr, const DeclareDataAware *manager = nullptr, bool dropMatches = false) {
    env e1, e2;
    ResultIndex pair, pair1;
    ResultRecord result{{0, 0}, {1.0, {}}};
    bool hasMatch;

    for (; first1 != last1; ++d_first) {
        if (first2 == last2)
            return d_first;
        if (first1->first > first2->first) {
            first2++;
        } else if (first1->first < first2->first) {
            first1++;
        } else {
            pair.first = first1->first.first;
            result.first = first1->first;
            result.second.first = aggr(first1->second.first, first2->second.first);
            result.second.second.clear();
            pair1.first = first2->first.first;
            hasMatch = false;

            if (manager) {
                for (const marked_event &elem: first1->second.second) {
                    if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                    pair.second = GET_ACTIVATION_EVENT(elem);

                    for (const marked_event &elem1: first2->second.second) {
                        if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                        pair1.second = GET_TARGET_EVENT(elem1);

                        e1 = manager->GetPayloadDataFromEvent(pair);
                        e2 = manager->GetPayloadDataFromEvent(pair1);

                        if (manager->checkValidity(e1, e2)) {
                            hasMatch = true;
                            if (!dropMatches) {
                                result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                            }
                        }
                    }
                }
            } else {
                hasMatch = true;
                if (!dropMatches) {
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                    result.second.second.insert(result.second.second.end(), first2->second.second.begin(), first2->second.second.end());
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                *d_first = result;
            }

            first1++;
            first2++;
        }
    }
    return d_first;
}

template<typename InputIt1, typename InputIt2, typename OutputIt, typename Aggregation> inline
OutputIt setIntersectionUntimed(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first,
                                Aggregation aggr,
                                const DeclareDataAware *manager = nullptr,
                                bool dropMatches = false) {
    std::map<uint32_t, Result> group1 = GroupByKeyExtractor<InputIt1, uint32_t, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    std::map<uint32_t, Result> group2 = GroupByKeyExtractor<InputIt2, uint32_t, ResultRecord>(
            first2, last2,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    env e1, e2;
    std::pair<uint32_t, uint16_t> pair, pair1;
    auto start1 = group1.begin(), end1 = group1.end();
    auto start2 = group2.begin(), end2 = group2.end();
    bool hasMatch;
    ResultRecord result{{0, 0}, {1.0, {}}};

    for (; start1 != end1; ++d_first) {
        if (start2 == end2) {
            return d_first;
        } else if (start1->first > start2->first) {
            start2++;
        } else if (start1->first < start2->first) {
            start1++;
        } else {
            result.first.first = pair.first = start1->first;
            result.second.first = 1.0;
            result.second.second.clear();
            pair1.first = start2->first;
            hasMatch = false;

            for (const auto &cont1: start1->second) {
                for (const auto &cont2: start2->second) {
                    result.second.first = std::min(aggr(first1->second.first, first2->second.first), result.second.first);

                    if (manager) {
                        for (const auto &elem: cont1.second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            pair.second = GET_ACTIVATION_EVENT(elem);
                            for (const auto &elem1: cont2.second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                pair1.second = GET_TARGET_EVENT(elem1);

                                e1 = manager->GetPayloadDataFromEvent(pair);
                                e2 = manager->GetPayloadDataFromEvent(pair1);

                                if (manager->checkValidity(e1, e2)) {
                                    hasMatch = true;
                                    if (!dropMatches) {
                                        result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                                    }
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        if (!dropMatches) {
                            result.second.second.insert(result.second.second.end(), cont1.second.second.begin(), cont1.second.second.end());
                            result.second.second.insert(result.second.second.end(), cont2.second.second.begin(), cont2.second.second.end());
                        }
                    }
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                *d_first = result;
            }

            start1++;
            start2++;
        }
    }

    return d_first;
}

#define SCAN_MIN_MAX(traceId, startEventId, min, max, lower, upper, section) RESULT_RECORD_MIN(min, traceId, startEventId); RESULT_RECORD_MIN(max, traceId, startEventId); auto lower = std::lower_bound(section.begin(), section.end(), min); auto upper = std::upper_bound(lower, section.end(), max);

template<typename TableSection> inline
Result next(const uint32_t &traceId, const uint16_t &startEventId, const uint16_t& endEventId, const TableSection &section) {
    SCAN_MIN_MAX(traceId, startEventId, min, max, lower, upper, section)
    ResultIndex idx{traceId, 0};
    Result temp {};
    while (lower != upper) {
        idx.second = lower->first.second;
        if(idx.second > 0){
            idx.second--;
            temp.emplace_back(idx, lower->second);
        }
        lower++;
    }

    return temp;
}

template<typename TableSection> inline
Result next(const TableSection &section) {
    Result temp;
    ResultIndex idx;

    auto itr = section.begin();
    while (itr != section.end()) {
        idx = itr->first;
        if(idx.second > 0){
            idx.second--;
            temp.emplace_back(idx, itr->second);
        }
        itr++;
    }

    return temp;
}



template<typename TableSection> inline
Result global(const uint32_t &traceId, const uint16_t &startEventId, const uint16_t& endEventId, const TableSection &section) {
//    auto lower = std::lower_bound(section.begin(), section.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, startEventId},  {0, {}}});
//    auto upper = std::upper_bound(lower, section.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, endEventId},  {1, maxVec}});
    SCAN_MIN_MAX(traceId, startEventId, min, max, lower, upper, section)
    Result temp {};
    if(lower == upper){
        return temp;
    }

    const uint32_t dist = std::distance(lower, upper - 1);

    if(dist == (endEventId - startEventId)){
        auto vec = populateAndReturnEvents(lower, upper);
        temp.emplace_back(std::pair<uint32_t, uint16_t>{traceId, startEventId}, std::pair<double, std::vector<uint16_t>>{1,vec});
    }

    return temp;
}

template<typename TableSection> inline
Result global(const TableSection &section, const std::vector<size_t>& lengths) {
    Result temp {};
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();
    ResultRecord result;
    ResultRecord var{{0,0}, {1.0,{}}};
    ResultRecord maxVar{{0,0}, {0,{}}};

    while(upper != end){
        uint32_t currentTraceId = upper->first.first;
        var.first.first = maxVar.first.first = result.first.first = currentTraceId;
        maxVar.first.second = lengths.at(currentTraceId);

        lower = upper;
        upper = std::upper_bound(lower, section.end(), maxVar);

        const uint32_t dist = std::distance(lower, upper - 1);

        if(dist == lengths[currentTraceId] - 1){
            var.second.second = populateAndReturnEvents(lower, upper);
            temp.emplace_back(var);
        }
    }

    return temp;
}

#include <iostream>


template<typename TableSection> inline
Result future(const uint32_t &traceId, const uint16_t &startEventId, const uint16_t& endEventId, const TableSection &section) {
//    auto lower = std::lower_bound(section.begin(), section.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, startEventId}, {0, {}}});
//    auto upper = std::upper_bound(section.begin(), section.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, endEventId}, {1, maxVec}});
    SCAN_MIN_MAX(traceId, startEventId, min, max, lower, upper, section)
    Result  temp {};
    ResultIndex idx{traceId, 0};

    if(lower == upper){
        return temp;
    }

    while (lower != upper) {
        idx.second = lower->first.second;
        temp.emplace_back(idx, lower->second);
        lower++;
    }

    return temp;
}

template<typename TableSection> inline
Result future(const TableSection &section) {
    return section;
}

template<typename TableSection> inline
Result negateUntimed(TableSection &data_untimed, const std::vector<size_t> &lengths, bool preserveNegatedFacts = true) {
    Result result;
    size_t first1 = 0, last1 = lengths.size();
    ResultRecord rc{{0, 0}, {1.0, {}}};
    auto first2 = data_untimed.begin(), last2 = data_untimed.end();
    for (; first1 != last1; ) {
        if (first2 == last2) {
            while (first1 != last1) {
                result.emplace_back(std::make_pair(first1++, 0), std::make_pair(1.0, MarkedEventsVector{}));
            }
            return result;
        }
        if (first1 > first2->first.first) {
            if (preserveNegatedFacts) {
                auto tmp = *first2;
                tmp.second.first = 1.0 - tmp.second.first;
                result.push_back(tmp);
            }
            first2++;
        } else if (first1 < first2->first.first) {
            result.emplace_back(std::make_pair(first1, 0), std::make_pair(1.0, MarkedEventsVector{}));
            first1++;
        } else {
            // MEMO: if you want to preserve the condition where it didn't hold for repairs or givin advices, then you should return a result having 0, and containing the result of the match
            if (preserveNegatedFacts) {
                rc.first.first = first1;
                rc.second.first = 1.0;
                while (first2 != last2 && (first2->first.first != first1)) {
                    rc.second.second.insert(rc.second.second.end(), first2->second.second.begin(), first2->second.second.end());
                    rc.second.first *= first2->second.first;
                    first2++;
                }
                rc.second.first = 1.0 - rc.second.first;
                remove_duplicates(rc.second.second);
                result.push_back(rc);
            }
            first1++;
            first2++;
        }
    }
    if (preserveNegatedFacts) {
        if (first2 != last2) {
            trace_t currTraceId = first2->first.first;
            rc.first.first = currTraceId;
            rc.second.first = 1.0;
            while ((first2 != last2) && (first2->first.first == currTraceId)) {
                rc.second.second.insert(rc.second.second.end(), first2->second.second.begin(), first2->second.second.end());
                rc.second.first *= first2->second.first;
                first2++;
            }
            rc.second.first = 1.0 - rc.second.first;
            remove_duplicates(rc.second.second);
            result.push_back(rc);
            first2++;
        }
    }
    return result;
}

template<typename TableSection> inline
Result until(const uint32_t &traceId,
             const uint16_t &startEventId,
             const uint16_t& endEventId,

             const TableSection &aSection,
             const TableSection &bSection,

             const DeclareDataAware* manager = nullptr) {

    SCAN_MIN_MAX(traceId, startEventId, min, max, lower, upper, bSection)
//
//    auto lower = std::lower_bound(bSection.begin(), bSection.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, startEventId}, {0, {}}});
//    //auto localUpper = lower;
//    auto upper = std::upper_bound(lower, bSection.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, endEventId},  {1, maxVec}});
    if(lower == upper){
        return {};
    }
    ResultRecord result{{traceId, startEventId}, {1, {}}};

    auto aIt = std::lower_bound(aSection.begin(), aSection.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, startEventId}, {0, {}}});
    auto aEn = aIt;//std::upper_bound(aIt, aSection.end(), std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, endEventId}, {1, maxVec}});
    auto aEnd = aSection.end();

    Result temp {};
    env e1, e2;
    std::pair<uint32_t, uint16_t> Fut, Prev;

    for( ; lower != upper; lower++) {
        Fut.first = lower->first.first;
        if (lower->first.second == startEventId) {
            temp.emplace_back(*lower);
        } else {
            aEn = std::upper_bound(aEn, aEnd, std::pair<std::pair<uint32_t, uint16_t>, std::pair<double, std::vector<uint16_t>>>{{traceId, lower->first.second-1},  {1, maxVec}});
            if(aIt == aEn){
                // Rationale: (1)
                // if the condition does not hold for a time [startEventId, lower->first.second-1], it is because one event makes it not hold.
                // Therefore, it should never hold even if you are extending the data that you have.
                return temp;
            } else {
                result.second.second.clear();
                const uint32_t dist = std::distance(aIt, aEn - 1);
                //MarkedEventsVector V;
                if(dist == ((lower->first.second) - startEventId)-1){
                    if (manager) {
                        for (marked_event& activationEvent : lower->second.second) {
                            if (!IS_MARKED_EVENT_TARGET(activationEvent)) continue;
                            Fut.second = GET_TARGET_EVENT(activationEvent);
                            e1 = manager->GetPayloadDataFromEvent(Fut);
                            for (auto curr = aIt; curr != aEn; curr++) {
                                Prev.first = curr->first.first;
                                for (marked_event& targetEvent : curr->second.second) {
                                    if (!IS_MARKED_EVENT_ACTIVATION(targetEvent)) continue;
                                    Prev.second = GET_ACTIVATION_EVENT(targetEvent);
                                    e2 = manager->GetPayloadDataFromEvent(Prev);
                                    if (!manager->checkValidity(e2, e1)) {
                                        return temp;
                                    } else {
                                        result.second.second.emplace_back(targetEvent);
                                    }
                                }
                            }
                        }
                        ///V.insert(V.begin(), lower->second.second.begin(), lower->second.second.end());
                    } else {
                        result.second.second = populateAndReturnEvents(aIt, aEn);
                        result.second.second.insert(result.second.second.begin(), lower->second.second.begin(), lower->second.second.end());
                    }
                    temp.emplace_back(result);
                } else {
                    // For (1)
                    return temp;
                }
            }
        }

    }

    return temp;
}

template<typename TableSection> inline
Result weakUntil(const uint32_t &traceId,
                 const uint16_t &startEventId,
                 const uint16_t& endEventId,

                 const TableSection &aSection,
                 const TableSection &bSection,

                 const DeclareDataAware* manager = nullptr) {
    Result result;
    Result untilR = until(traceId, startEventId, endEventId, aSection, bSection, manager);
    Result globally = global(traceId, startEventId, endEventId, aSection);
    setUnion(untilR.begin(), untilR.end(), globally.begin(), globally.end(), std::back_inserter(result), [](double x, double y) {return x*y;}, nullptr);
    return result;
}

template<typename TableSection> inline
Result release(const uint32_t &traceId,
               const uint16_t &startEventId,
               const uint16_t& endEventId,

               const TableSection &aSection,
               const TableSection &bSection,

               const DeclareDataAware* manager = nullptr) {

    Result newB;
    Result result;
    setIntersection(aSection.begin(), aSection.end(),
                    bSection.begin(), bSection.end(),
                    std::back_inserter(newB),
                    [](double x, double y) {return x*y;},
                    manager);

    if (manager) {
        auto flipped = manager->flip();
        weakUntil(traceId, startEventId, endEventId, bSection, newB, flipped);
    } else {
        weakUntil(traceId, startEventId, endEventId, bSection, newB, nullptr);
    }
    return result;
}


template<typename TableSection> inline
Result until(const TableSection &aSection, const TableSection &bSection, const std::vector<size_t>& lengths, const DeclareDataAware* manager = nullptr) {
    auto lower = bSection.begin();
    auto localUpper = lower;
    auto upper = bSection.end();

    auto aIt = aSection.begin();
    auto aEn = aSection.begin();
    auto upperA = aSection.end();

    Result temp {};
    env e1, e2;
    std::pair<uint32_t, uint16_t> Fut, Prev;
    ResultRecord rU{{0, 0}, {0, {}}};
    ResultRecord rD{{0, 0}, {0, {}}};
    ResultRecord rMV{{0, 0}, {1, maxVec}};

    while (lower != upper) {
        uint32_t currentTraceId = localUpper->first.first;
        rMV.first.first = rD.first.first = rU.first.first = currentTraceId;
        rU.first.second = lengths.at(currentTraceId);

        localUpper = std::upper_bound(lower, upper, rU);

        aIt = std::lower_bound(aIt, upperA, rD);
        aEn = aIt;

        for( ; lower != localUpper; lower++) {
            Fut.first = lower->first.first;
            if (lower->first.second == 0) {
                temp.emplace_back(*lower);
            } else {
                rMV.first.second = lower->first.second-1;
                aEn = std::upper_bound(aEn, upperA, rMV);
                if(aIt == aEn){
                    // Rationale: (1)
                    // if the condition does not hold for a time [startEventId, lower->first.second-1], it is because one event makes it not hold.
                    // Therefore, it should never hold even if you are extending the data that you have.
                    break;
                } else {
                    const uint32_t dist = std::distance(aIt, aEn - 1);
                    MarkedEventsVector V;
                    if(dist == ((lower->first.second))-1){
                        if (manager) {
                            bool hasFail = false;
                            for (const marked_event& activationEvent : lower->second.second) {
                                if (hasFail) break;
                                if (!IS_MARKED_EVENT_TARGET(activationEvent)) continue;
                                Fut.second = GET_TARGET_EVENT(activationEvent);
                                e1 = manager->GetPayloadDataFromEvent(Fut);
                                for (auto curr = aIt; curr != aEn; curr++) {
                                    if (hasFail) break;
                                    Prev.first = curr->first.first;
                                    for (const marked_event& targetEvent : curr->second.second) {
                                        if (!IS_MARKED_EVENT_ACTIVATION(targetEvent)) continue;
                                        Prev.second = GET_ACTIVATION_EVENT(targetEvent);
                                        e2 = manager->GetPayloadDataFromEvent(Prev);
                                        if (!manager->checkValidity(e2, e1)) {
                                            hasFail = true;
                                            break;
                                        } else {
                                            V.emplace_back(marked_event::join(Fut.second, Prev.first));
                                        }
                                    }
                                }
                            }
                            if (hasFail) break;
                            std::sort(V.begin(), V.end());
                            V.erase(std::unique(V.begin(), V.end()), V.end());
                            V.insert(V.begin(), lower->second.second.begin(), lower->second.second.end());
                        } else {
                            V = populateAndReturnEvents(aIt, aEn);
                            V.insert(V.begin(), lower->second.second.begin(), lower->second.second.end());
                        }
                        temp.emplace_back(std::pair<uint32_t, uint16_t>{currentTraceId, 0}, std::pair<double, MarkedEventsVector>{1,V});
                    } else {
                        // For (1)
                        break;
                    }
                }
            }

        }
        lower = localUpper;
    }

    return temp;
}

template<typename TableSection> inline
Result weakUntil(const TableSection &aSection,
                 const TableSection &bSection,
                 const std::vector<size_t>& lengths,
                 const DeclareDataAware* manager = nullptr) {
    Result result;
    Result untilR = until(aSection, bSection, manager);
    Result globally = global(aSection, lengths);
    setUnionUntimed(untilR.begin(), untilR.end(), globally.begin(), globally.end(), std::back_inserter(result), [](double x, double y) {return x*y;}, nullptr);
    return result;
}

template<typename TableSection> inline
Result release(const TableSection &psi,
               const TableSection &phi,
               const std::vector<size_t>& lengths,
               const DeclareDataAware* manager = nullptr) {

    Result intersection;
    Result result;
    setIntersectionUntimed(psi.begin(), psi.end(),
                           phi.begin(), phi.end(),
                           std::back_inserter(intersection),
                           [](double x, double y) {return x*y;},
                           manager);

    if (manager) {
        auto flipped = manager->flip();
        weakUntil(phi, intersection, lengths, flipped);
    } else {
        weakUntil(phi, intersection, lengths, nullptr);
    }

    return result;
}


#endif //KNOBAB_SERVER_LTLF_OPERATORS_H
