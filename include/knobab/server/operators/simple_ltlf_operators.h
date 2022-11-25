//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_SIMPLE_LTLF_OPERATORS_H
#define KNOBAB_SERVER_SIMPLE_LTLF_OPERATORS_H

#include <knobab/server/operators/semantics.h>
#include "knobab/server/declare/DeclareDataAware.h"


/**
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void or_logic_timed(const Result &lhs, const Result &rhs, Result &out, const PredicateManager *manager = nullptr,
                           const std::vector<size_t> &lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    ResultIndex pair, pair1;
    bool hasMatch;
    ResultRecord result{{0,   0},
                        {0.0, {}}};

    while (first1 != last1) {
        if (first2 == last2) {
            std::copy(first1, last1, std::back_inserter(out));
            return;
        }
        if (first1->first > first2->first) {
            out.emplace_back(*first2++);
        } else if (first1->first < first2->first) {
            out.emplace_back(*first1++);
        } else {
            pair.first = first1->first.first;
            pair1.first = first2->first.first;
            result.first = first1->first;
            result.second.first = std::min(first1->second.first, first2->second.first);
            result.second.second.clear();
            hasMatch = false;

            if (manager && (!first1->second.second.empty()) && (!first2->second.second.empty())) {
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
                            result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                        }
                    }
                }
            } else {
                hasMatch = true;
                if (!first1->second.second.empty())
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(),
                                                first1->second.second.end());
                else
                    result.second.second.emplace_back(marked_event::activation(first1->first.second));
                if (!first2->second.second.empty())
                    result.second.second.insert(result.second.second.end(), first2->second.second.begin(),
                                                first2->second.second.end());
                else
                    result.second.second.emplace_back(marked_event::target(first2->first.second));
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }

            first1++;
            first2++;
        }
    }
    std::copy(first2, last2, std::back_inserter(out));
}

/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void or_logic_untimed(const Result &lhs, const Result &rhs,
                             Result &out,
                             const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    std::map<uint32_t, Result> group1 = GroupByKeyExtractor<decltype(first1), uint32_t, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    std::map<uint32_t, Result> group2 = GroupByKeyExtractor<decltype(first2), uint32_t, ResultRecord>(
            first2, last2,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    env e1, e2;
    std::pair<uint32_t, uint16_t> pair, pair1;
    auto start1 = group1.begin(), end1 = group1.end();
    auto start2 = group2.begin(), end2 = group2.end();
    ResultRecord result{{0,   0},
                        {0.0, {}}};
    bool hasMatch;
    size_t localTrace = 0;

    while (start1 != end1) {
        if (start2 == end2) {
            while (start1 != end1) {
                localTrace = start1->first;
                result.second.first = 0.0;
                result.first.first = localTrace;
                result.first.second = 0;
                result.second.second.clear();
                for (const auto &dx: start1->second) {
                    result.second.first = std::max(result.second.first, dx.second.first);
                    result.second.second.insert(result.second.second.end(), dx.second.second.begin(),
                                                dx.second.second.end());
                }
                remove_duplicates(result.second.second);
                out.emplace_back(result);
                start1++;
            }
            return;
        } else if (start1->first > start2->first) {
            localTrace = start2->first;
            result.second.first = 0.0;
            result.first.first = localTrace;
            result.first.second = 0;
            result.second.second.clear();
            for (const auto &dx: start2->second) {
                result.second.first = std::max(result.second.first, dx.second.first);
                result.second.second.insert(result.second.second.end(), dx.second.second.begin(),
                                            dx.second.second.end());
            }
            remove_duplicates(result.second.second);
            out.emplace_back(result);
            start2++;
            //std::copy(start2->second.begin(), start2->second.end(), std::back_inserter(out));
        } else if (start1->first < start2->first) {
            //std::copy(start1->second.begin(), start1->second.end(), std::back_inserter(out));
            localTrace = start1->first;
            result.second.first = 0.0;
            result.first.first = localTrace;
            result.first.second = 0;
            result.second.second.clear();
            for (const auto &dx: start1->second) {
                result.second.first = std::max(result.second.first, dx.second.first);
                result.second.second.insert(result.second.second.end(), dx.second.second.begin(),
                                            dx.second.second.end());
            }
            remove_duplicates(result.second.second);
            out.emplace_back(result);
            start1++;
        } else {
            result.first.first = pair.first = start1->first;
            result.second.first = 0.0;
            result.second.second.clear();
            pair1.first = start2->first;
            hasMatch = false;

            for (const auto &cont1: start1->second) {
                for (const auto &cont2: start2->second) {
                    result.second.first = std::max(std::min(first1->second.first, first2->second.first),
                                                   result.second.first);
                    if (manager && (!cont1.second.second.empty()) && (!cont2.second.second.empty())) {
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
                                    result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        result.second.second.insert(result.second.second.end(), cont1.second.second.begin(),
                                                    cont1.second.second.end());
                        result.second.second.insert(result.second.second.end(), cont2.second.second.begin(),
                                                    cont2.second.second.end());
                    }
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }

            start1++;
            start2++;
        }
    }
    while (start2 != end2) {
        localTrace = start2->first;
        result.second.first = 0.0;
        result.first.first = localTrace;
        result.first.second = 0;
        result.second.second.clear();
        for (const auto &dx: start2->second) {
            result.second.first = std::max(result.second.first, dx.second.first);
            result.second.second.insert(result.second.second.end(), dx.second.second.begin(), dx.second.second.end());
        }
        remove_duplicates(result.second.second);
        out.emplace_back(result);
        start2++;
    }
}

/**
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 * @param dropMatches
 */
inline void and_logic_timed(const Result &lhs, const Result &rhs,
                            Result &out,
                            const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    ResultIndex pair, pair1;
    ResultRecord result{{0,   0},
                        {1.0, {}}};
    bool hasMatch;

    while (first1 != last1) {
        if (first2 == last2)
            return /*d_first*/;
        if (first1->first > first2->first) {
            first2++;
        } else if (first1->first < first2->first) {
            first1++;
        } else {
            pair.first = first1->first.first;
            result.first = first1->first;
            result.second.first = std::min(first1->second.first, first2->second.first);
            result.second.second.clear();
            pair1.first = first2->first.first;
            hasMatch = false;

            if (manager && (!first1->second.second.empty()) && (!first2->second.second.empty())) {
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
                            result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                        }
                    }
                }
            } else {
                hasMatch = true;
                result.second.second.insert(result.second.second.end(), first1->second.second.begin(),
                                            first1->second.second.end());
                result.second.second.insert(result.second.second.end(), first2->second.second.begin(),
                                            first2->second.second.end());
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }

            first1++;
            first2++;
        }
    }
}

/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 * @param dropMatches
 */
inline void and_logic_untimed(const Result &lhs, const Result &rhs,
                              Result &out,
                              const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    std::map<uint32_t, Result> group1 = GroupByKeyExtractor<decltype(first1), uint32_t, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    std::map<uint32_t, Result> group2 = GroupByKeyExtractor<decltype(first2), uint32_t, ResultRecord>(
            first2, last2,
            [](const ResultRecord &p) {
                return p.first.first;
            });

    env e1, e2;
    std::pair<uint32_t, uint16_t> pair, pair1;
    auto start1 = group1.begin(), end1 = group1.end();
    auto start2 = group2.begin(), end2 = group2.end();
    bool hasMatch;
    ResultRecord result{{0,   0},
                        {1.0, {}}};

    while (start1 != end1) {
        if (start2 == end2) {
            return /*d_first*/;
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
                    result.second.first = std::max(std::min(first1->second.first, first2->second.first),
                                                   result.second.first);
                    if (manager && (!cont1.second.second.empty()) && (!cont2.second.second.empty())) {
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
                                    result.second.second.emplace_back(marked_event::join(pair.second, pair1.second));
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        result.second.second.insert(result.second.second.end(), cont1.second.second.begin(),
                                                    cont1.second.second.end());
                        result.second.second.insert(result.second.second.end(), cont2.second.second.begin(),
                                                    cont2.second.second.end());
                    }
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }

            start1++;
            start2++;
        }
    }
}

inline void next_logical(const Result &section, Result &temp, const std::vector<size_t> &lengths = {}) {
    temp.clear();
    ResultIndex idx;

    auto itr = section.begin();
    while (itr != section.end()) {
        idx = itr->first;
        if (idx.second > 0) {
            idx.second--;
            temp.emplace_back(idx, itr->second);
        }
        itr++;
    }
}

#include <yaucl/functional/assert.h>

/**
 *
 * @author Giacomo Bergami
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void future_logic_timed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    result.clear();
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();

    ResultIndex first;
    ResultRecordSemantics second{0.0, 0.0};
    ResultRecord cp{{0,   0},
                    {1.0, {}}};

    while (upper != end) {
        uint32_t currentTraceId = upper->first.first;
        first.first = cp.first.first = currentTraceId;
        cp.first.second = lengths.at(currentTraceId);
        first.second = 0;

        lower = upper;
        upper = std::upper_bound(lower, section.end(), cp);

        Result toBeReversed;
        auto it = lower + std::distance(lower, upper) - 1;
        bool isBegin = true;
        for (int64_t i = (upper - 1)->first.second; (i >= 0); i--) {
            first.second = i;
            if ((it >= lower) && (it->first.second == i)) {
                second.first = std::max(it->second.first, second.first);
                second.second.insert(second.second.begin(), it->second.second.begin(), it->second.second.end());
                remove_duplicates(second.second);
                if (isBegin) isBegin = false;
                it--;
            } else {
                DEBUG_ASSERT(!isBegin);
            }
            toBeReversed.emplace_back(first, second);
        }

        // Inserting the elements in reversed order
        result.insert(result.end(), std::make_move_iterator(toBeReversed.rbegin()),
                      std::make_move_iterator(toBeReversed.rend()));

        second.second.clear();
    }
}

/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void future_logic_untimed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    //dataContainer temp {};
    result.clear();
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();

    ResultIndex first{0, 0};
    ResultRecordSemantics second{0.0, {}};
    ResultRecord cp{{0,   0},
                    {1.0, {}}};

    while (upper != end) {
        uint32_t currentTraceId = upper->first.first;
        first.first = cp.first.first = currentTraceId;
        cp.first.second = lengths.at(currentTraceId);
        second.second.clear();
        second.first = 0.0;

        lower = upper;
        upper = std::upper_bound(lower, section.end(), cp);

        for (auto it = lower + std::distance(lower, upper) - 1; it >= lower; it--) {
            second.first = std::max(second.first, it->second.first);
            second.second.insert(second.second.end(), it->second.second.begin(), it->second.second.end());
        }
        remove_duplicates(second.second);
        result.emplace_back(first, second);
    }
}

/**
 *
 * @author Giacomo Bergami
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void global_logic_timed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    result.clear();
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();

    ResultIndex first_g;
    ResultRecordSemantics second_g{1.0, 0.0};
    ResultRecord cp_g{{0,   0},
                      {1.0, {}}};

    while (upper != end) {
        uint32_t currentTraceId = upper->first.first;
        first_g.first = cp_g.first.first = currentTraceId;
        second_g.first = 1.0;
        cp_g.first.second = lengths.at(currentTraceId);
        first_g.second = 0;

        lower = upper;
        upper = std::upper_bound(lower, section.end(), cp_g);

        Result toBeReversed;
        auto it = (lower == upper) ? (lower-1) : ( lower + std::distance(lower, upper) - 1);
        for (int64_t i = (upper - 1)->first.second; i >= 0; i--) {
            first_g.second = i;
            const uint32_t dist = std::distance(it, upper);

            if ((it >= lower) && (cp_g.first.first == it->first.first) && (dist == (cp_g.first.second - it->first.second))) {
                second_g.first = std::min(it->second.first, second_g.first);
                second_g.second.insert(second_g.second.begin(), it->second.second.begin(), it->second.second.end());
                remove_duplicates(second_g.second);
                it--;
            } else {
                break; // If after this the condition does not hold, then it means that in the remainder I will have
                // events that are not matching the condition
            }
            toBeReversed.emplace_back(first_g, second_g);
        }

        // Inserting the elements in reversed order
        result.insert(result.end(), std::make_move_iterator(toBeReversed.rbegin()),
                      std::make_move_iterator(toBeReversed.rend()));

        second_g.second.clear();
    }
}

/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void global_logic_untimed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();
    result.clear();

    ResultIndex first_g{0, 0};
    ResultRecordSemantics second_g{1.0, {}};
    ResultRecord cp_g{{0,   0},
                      {1.0, {}}};
    while (upper != end) {
        uint32_t currentTraceId = upper->first.first;
        first_g.first = cp_g.first.first = currentTraceId;
        cp_g.first.second = lengths.at(currentTraceId);
        cp_g.second.second.clear();
        lower = upper;
        second_g.second.clear();
        upper = lower + (cp_g.first.second - 1);//std::upper_bound(lower, section.end(), cp);
        ///const uint32_t dist = std::distance(lower, upper - 1);
        if ((upper < end) && (upper->first.first == lower->first.first)) {
            populateAndReturnEvents(lower, ++upper, second_g.second);
            result.emplace_back(first_g, second_g);
        } else {
            cp_g.first.first = currentTraceId + 1;
            cp_g.first.second = 0;
            upper = std::lower_bound(lower, section.end(), cp_g);
        }
    }
}

#include <iostream>
#include <queue>

/**
 *
 * @author Giacomo Bergami
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void until_logic_timed(const Result &aSection, const Result &bSection, Result &temp,
                              const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto bLower = bSection.begin();
    auto bLocalUpper = bLower;
    auto bUpper = bSection.end();
    auto aLower = aSection.begin();
    auto aLocalUpper = aSection.begin();
    auto aUpper = aSection.end();


    ResultRecordSemantics semein{1.0, {}};
    ResultRecordSemantics semeinMax{1.0, maxVec};
    ResultRecord cp_bLocalUpper{{0, 0}, semeinMax};
    ResultRecord cp_aLocalLower{{0, 0}, semein};
    ResultRecord cp_aLocalUpper{{0, 0}, semeinMax};
    ResultIndex first;
    //
    auto join = marked_event::join(0, 0);

    env e1, e2;
    std::pair<uint32_t, uint16_t> Fut, Prev;

    while (bLower != bUpper) {
        in_memory_trace_id_t currentTraceId = bLower->first.first;
        cp_aLocalLower.first.first = cp_aLocalUpper.first.first
                = Fut.first
                = Prev.first
                = first.first
                = cp_bLocalUpper.first.first
                = currentTraceId;
        cp_bLocalUpper.first.second = lengths.at(bLower->first.first);

        bLocalUpper = std::upper_bound(bLocalUpper, bUpper, cp_bLocalUpper);
        in_memory_event_id_t alpha = 0;
        std::map<in_memory_event_id_t, ResultRecordSemantics> globalResultBeforeShift;

        while (bLower != bLocalUpper) {
            in_memory_event_id_t t = bLower->first.second;
            if (t == alpha) {
                // TODO: add event in results
                if (globalResultBeforeShift.empty()) {
                    // If there were not previous events, directly insert this into temp
                    temp.emplace_back(*bLower);
                } else {
                    // Otherwise, the last element of the collection should be the current event:
                    // add the current data to it, and then shift it to the result.
                    auto& ref = globalResultBeforeShift.emplace(t, semein).first->second;
                    ref.first = std::max(ref.first, bLower->second.first);
                    ref.second.insert(ref.second.end(), bLower->second.second.begin(), bLower->second.second.end());
                    remove_duplicates(ref.second);
                }
                bLower++; /// Increment Upper
            } else {
                // Scanning the "a"-s
                cp_aLocalLower.first.second = alpha;
                aLower = std::lower_bound(aLower, aUpper, cp_aLocalLower);
                cp_aLocalUpper.first.second = std::max((in_memory_event_id_t)(bLower->first.second - 1), (in_memory_event_id_t)cp_aLocalLower.first.second);
                aLocalUpper = std::upper_bound(aLower, aUpper, cp_aLocalUpper);
                std::iterator_traits<decltype(aLower)>::difference_type dst;
                size_t count;

                // If there is a sequence of As preceeding the Bs
                if ((aLower != aUpper) && (aLower->first.first == currentTraceId)) {
                    dst = std::distance(aLower, aLocalUpper) +
                          1; // distance between the beginning of the sequence of the As and the bLower
                    count = ((bLower)->first.second - alpha) + 1;

                    // if there exists a contiguous sequence sequence A_alpha ... A_{t-1}
                    if (dst == count) {

                        if (manager) {
                            // For the current event, every event should satisfy the correlation condition
                            bool hasFail = false;

                            std::map<in_memory_event_id_t, ResultRecordSemantics> localResultBeforeShift;
                            for (auto &activationEvent: bLower->second.second) {
                                if (hasFail) break;
                                if (!IS_MARKED_EVENT_TARGET(activationEvent)) continue;
                                Fut.second = GET_TARGET_EVENT(activationEvent);
                                e1 = manager->GetPayloadDataFromEvent(Fut);
                                for (auto curr = aLower; curr != aLocalUpper; curr++) {
                                    Prev.first = curr->first.first;
                                    if (hasFail) break;
                                    for (auto &targetEvent: curr->second.second) {
                                        if (!IS_MARKED_EVENT_ACTIVATION(targetEvent)) continue;
                                        Prev.second = GET_ACTIVATION_EVENT(targetEvent);
                                        e2 = manager->GetPayloadDataFromEvent(Prev);
                                        if (!manager->checkValidity(e2, e1)) {
                                            hasFail = true;
                                            auto low = localResultBeforeShift.find(aLower->first.second);
                                            auto up  = localResultBeforeShift.find(curr->first.second);
                                            localResultBeforeShift.erase(low, up);
                                            break;
                                        } else {
                                            join.id.parts.left = Fut.second;
                                            join.id.parts.right = Prev.second;
                                            for (auto idx = curr; idx != aLocalUpper; idx++) {
                                                localResultBeforeShift[idx->first.second].second.emplace_back(join);
                                            }
                                        }
                                    }
                                }
                            }
                            if (!hasFail){
                                for (const auto &ref: localResultBeforeShift) {
                                    auto &ref2 = globalResultBeforeShift.emplace(ref.first, semein).first->second;
                                    ref2.first = std::min(ref2.first, ref.second.first);
                                    ref2.second.insert(ref2.second.end(), ref.second.second.begin(),
                                                       ref.second.second.end());
                                }
                            }
                        } else {
                            for (auto curr = aLower; curr != aLocalUpper; curr++) {
                                auto& ref2 = globalResultBeforeShift.emplace(curr->first.second, semein).first->second;
                                ref2.first = std::min(ref2.first, curr->second.first);
                                ref2.second.insert(ref2.second.end(),
                                                   bLower->second.second.begin(),
                                                   bLower->second.second.end());
                                ref2.second.insert(ref2.second.end(),
                                                   curr->second.second.begin(),
                                                   curr->second.second.end());
                                populateAndReturnEvents(curr, aLocalUpper, ref2.second);
                            }
                        }
                        first.second = bLower->first.second;
                        auto &ref = globalResultBeforeShift.emplace(first.second, semein).first->second;
                        ref.first = std::max(ref.first, bLower->second.first);
                        ref.second.insert(ref.second.end(), bLower->second.second.begin(), bLower->second.second.end());
                        bLower++;
                    } else if ((aLower->first.first == currentTraceId) && (aLower->first.second < t)) {
                        if (alpha == aLower->first.second)
                            alpha++;
                        else
                            alpha = aLower->first.second;
                    } else {
                        alpha = t;
                    }
                } else {
                    // Otherwise, if there is no sequence, I will need to add the current event
                    alpha = t;
                }
            }
        }

        for (auto it = globalResultBeforeShift.begin(), en = globalResultBeforeShift.end(); it!= en; it++) {
            first.second = it->first;
            remove_duplicates(it->second.second);
            temp.emplace_back(first, it->second);
        }
        globalResultBeforeShift.clear();
    }
}


inline void until_logic_untimed(const Result &aSection, const Result &bSection, Result &temp,
                                const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto bCurrent = bSection.begin();
    auto localBUpper = bCurrent;
    auto upper = bSection.end();

    auto aIt = aSection.begin();
    auto aEn = aSection.begin();
    auto upperA = aSection.end();

    ResultRecord cpAIt{{0, 0},
                       {0, {}}};
    ResultRecord cpLocalUpper{{0,   0},
                              {1.0, {}}};
    ResultRecord cpAEn{{0,   0},
                       {1.0, maxVec}};
    ResultRecord cpResult{{0,   0},
                          {1.0, {}}};

    env e1, e2;
    std::pair<uint32_t, uint16_t> Fut, Prev;
    temp.clear();
    auto join = marked_event::join(0, 0);

    while (bCurrent != upper) {
        uint32_t currentTraceId = localBUpper->first.first;
        cpAIt.first.first = cpLocalUpper.first.first = cpAEn.first.first = cpResult.first.first = currentTraceId;
        cpLocalUpper.first.second = lengths.at(currentTraceId);
        cpAIt.first.second = 0;

        localBUpper = std::upper_bound(bCurrent, upper, cpLocalUpper);
        aIt = std::lower_bound(aIt, upperA, cpAIt);
        aEn = aIt;

        bool atLeastOneResult = false;
        for (; bCurrent != localBUpper; bCurrent++) {
            Fut.first = bCurrent->first.first;
            if (bCurrent->first.second == 0) {
                atLeastOneResult = true;
                cpResult.second.second.insert(cpResult.second.second.end(), bCurrent->second.second.begin(),
                                              bCurrent->second.second.end());
            } else {
                cpAEn.first.second = bCurrent->first.second - 1;
                aEn = std::upper_bound(aEn, upperA, cpAEn);
                if (aIt == aEn) {
                    // Rationale: (1)
                    // if the condition does not hold for a time [startEventId, lower->first.second-1], it is because
                    // one event makes it not hold. Therefore, it should never hold even if you are extending the data
                    // that you have.
                    break;
                } else {
                    const uint32_t dist = std::distance(aIt, aEn - 1);
                    //cpResult.second.second.clear();

                    if (dist == ((bCurrent->first.second)) - 1) {
                        if (manager) {
                            bool hasFail = false;
                            for (auto &activationEvent: bCurrent->second.second) {
                                if (hasFail) break;
                                if (!IS_MARKED_EVENT_TARGET(activationEvent)) continue;
                                Fut.second = GET_TARGET_EVENT(activationEvent);
                                e1 = manager->GetPayloadDataFromEvent(Fut);
                                for (auto curr = aIt; curr != aEn; curr++) {
                                    if (hasFail) break;
                                    Prev.first = curr->first.first;
                                    for (auto &targetEvent: curr->second.second) {
                                        if (!IS_MARKED_EVENT_ACTIVATION(targetEvent)) continue;
                                        Prev.second = GET_ACTIVATION_EVENT(targetEvent);
                                        e2 = manager->GetPayloadDataFromEvent(Prev);
                                        if (!manager->checkValidity(e2, e1)) {
                                            hasFail = true;
                                            break;
                                        } else {
                                            join.id.parts.left = Fut.second;
                                            join.id.parts.right = Prev.second;
                                            cpResult.second.second.emplace_back(join);
                                        }
                                    }
                                }
                            }
                            if (hasFail) break;
                            else atLeastOneResult = true;
                        } else {
                            populateAndReturnEvents(aIt, aEn, cpResult.second.second);
                            cpResult.second.second.insert(cpResult.second.second.end(), bCurrent->second.second.begin(),
                                                          bCurrent->second.second.end());
                            atLeastOneResult = true;
                        }
                    } else {
                        // For (1)
                        break;
                    }
                }
            }
        }

        if (atLeastOneResult) {
            remove_duplicates(cpResult.second.second);
            temp.emplace_back(cpResult);
        }
        cpResult.second.second.clear();


        bCurrent = localBUpper;
    }
}


inline void negated_logic_untimed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    auto first1 = section.begin(), last1 = section.end();
    std::map<uint32_t, Result> group1 = GroupByKeyExtractor<decltype(first1), uint32_t, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first.first;
            });
    ResultRecord rcx{{0,   0},
                     {1.0, {}}};
    for (in_memory_trace_id_t i = 0; i < lengths.size(); i++) {
        if (!group1.contains(i)) {
            rcx.first.first = i;
            result.emplace_back(rcx);
        }
    }
}


inline void negated_logic_timed(const Result &section, Result &result, const std::vector<size_t> &lengths) {
    auto first1 = section.begin(), last1 = section.end();
    std::map<ResultIndex, Result> group1 = GroupByKeyExtractor<decltype(first1), ResultIndex, ResultRecord>(
            first1, last1,
            [](const ResultRecord &p) {
                return p.first;
            });
    ResultRecord rcx{{0,   0},
                     {1.0, {}}};
    for (in_memory_trace_id_t i = 0; i < lengths.size(); i++) {
        rcx.first.first = i;
        for (in_memory_event_id_t j = 0, N = lengths.at(i); j < N; j++) {
            rcx.first.second = j;
            if (!group1.contains(rcx.first)) {
                result.emplace_back(rcx);
            }
        }
    }
}

inline void
implies_logic_timed(const Result &aSection, const Result &bSection, const Result &notaSection, Result &result,
                    const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    Result aTrue;
    and_logic_timed(aSection, bSection, aTrue, manager, lengths);
    or_logic_timed(aTrue, notaSection, result, nullptr, lengths);
}

inline void
implies_logic_untimed(const Result &aSection, const Result &bSection, const Result &notaSection, Result &result,
                      const PredicateManager *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    Result aTrue;
    and_logic_untimed(aSection, bSection, aTrue, manager, lengths);
    or_logic_untimed(aTrue, notaSection, result, nullptr, lengths);
}

#endif //KNOBAB_SIMPLE_LTLF_OPERATORS_H
