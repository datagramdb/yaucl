//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_FAST_LTLF_OPERATORS_H
#define KNOBAB_SERVER_FAST_LTLF_OPERATORS_H


/**
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void or_fast_timed(const Result& lhs, const Result& rhs, Result& out, const DeclareDataAware *manager = nullptr, const std::vector<size_t>& lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    ResultIndex pair, pair1;
    bool hasMatch;
    ResultRecord result{{0, 0}, {0.0, {}}};
    auto join = marked_event::join(0,0);
    std::unordered_set<std::string> cache;

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
            //pair1.first = first2->first.first;
            result.first = first1->first;
            result.second.first = std::min(first1->second.first, first2->second.first);
            result.second.second.clear();
            hasMatch = false;

            if (manager && (!first1->second.second.empty()) && (!first2->second.second.empty())) {
                for (const marked_event &elem: first1->second.second) {
                    if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                    join.id.parts.left = pair.second = GET_ACTIVATION_EVENT(elem);
                    e1 = manager->GetPayloadDataFromEvent(pair.first, pair.second, true, cache);

                    for (const marked_event &elem1: first2->second.second) {
                        if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                        join.id.parts.right = GET_TARGET_EVENT(elem1);

                        if (manager->checkValidity(e1, first2->first.first, join.id.parts.right)) {
                            hasMatch = true;
                            result.second.second.emplace_back(join);
                        }
                    }
                }
            } else {
                hasMatch = true;
                if (!first1->second.second.empty())
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                else
                    result.second.second.emplace_back(marked_event::activation(first1->first.second));
                if (!first2->second.second.empty())
                    result.second.second.insert(result.second.second.end(), first2->second.second.begin(), first2->second.second.end());
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
 * This implementation is faster than the one provided in the logical implementation, as we don't group by trace id,
 * but we directly do the loop on all the events of the trace
 *
 * @author Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void or_fast_untimed(const Result& lhs, const Result& rhs, Result& out, const DeclareDataAware *manager = nullptr, const std::vector<size_t>& lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    size_t localTrace = 0;
    ResultRecord result{{0, 0}, {0.0, {}}};
    ResultRecord idx{{0, 0}, {0.0, {}}};
    ResultIndex pair, pair1;
    std::unordered_set<std::string> cache;
    bool hasMatch, completeInsertionRight;
    auto join = marked_event::join(0, 0);
    while (first1 != last1) {
        if (first2 == last2) {
            result.first.second = 0;
            result.second.first = 0.0;
            while (first1 != last1){
                auto dx = first1;
                localTrace = first1->first.first;
                result.first.first = first1->first.first;
                result.second.second.clear();
                do {
                    result.second.first = std::max(result.second.first, first1->second.first);
                    result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
                    dx++;
                } while ((dx != last1) && (dx->first.first == localTrace));
                remove_duplicates(result.second.second);
                out.emplace_back(result);
                first1 = dx;
            }
            return;
        }
        if (first1->first.first > first2->first.first) {
            auto dx = first2;
            localTrace = first2->first.first;
            result.first.first = first2->first.first;
            result.second.second.clear();
            do {
                result.second.first = std::max(result.second.first, first2->second.first);
                result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
                dx++;
            } while ((dx != last2) && (dx->first.first == localTrace));
            remove_duplicates(result.second.second);
            out.emplace_back(result);
            first2 = dx;
        } else if (first1->first.first < first2->first.first) {
            auto dx = first1;
            localTrace = first1->first.first;
            result.first.first = first1->first.first;
            result.second.second.clear();
            do {
                result.second.first = std::max(result.second.first, first1->second.first);
                result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
                dx++;
            } while ((dx != last1) && (dx->first.first == localTrace));
            remove_duplicates(result.second.second);
            out.emplace_back(result);
            first1 = dx;
        } else {
            localTrace = first1->first.first;
            idx.first.first = first1->first.first+1; // pointing towards the next trace
            auto endFirst1 = std::upper_bound(first1, last1, idx);
            pair.first = pair1.first = localTrace;
            result.first = first1->first;
            result.second.first = std::min(first1->second.first, first2->second.first);
            result.second.second.clear();
            hasMatch = completeInsertionRight = false;
            decltype(endFirst1) endFirst2;
            for (; first1 != endFirst1; first1++) {
                auto dx = first2;
                do {

                    if (manager && (!first1->second.second.empty()) && (!first2->second.second.empty())) {
                        for (const marked_event &elem: first1->second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            join.id.parts.left = pair.second = GET_ACTIVATION_EVENT(elem);
                            e1 = manager->GetPayloadDataFromEvent(pair.first, pair.second, true, cache);

                            for (const marked_event &elem1: first2->second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                join.id.parts.right = pair1.second = GET_TARGET_EVENT(elem1);

                                if (manager->checkValidity(e1, localTrace, join.id.parts.right)) {
                                    hasMatch = true;
                                    result.second.second.emplace_back(join);
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        if (!completeInsertionRight) {
                            result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
                        }
                    }

                    dx++;
                } while ((dx != last2) && (dx->first.first == localTrace));
                endFirst2 = dx;

                if (!manager) {
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                    completeInsertionRight = true;
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }
            result.second.second.clear();
            first2 = endFirst2;
        }
    }
    result.first.second = 0;
    result.second.first = 0.0;
    while (first2 != last2){
        auto dx = first2;
        localTrace = first2->first.first;
        result.first.first = first2->first.first;
        result.second.second.clear();
        do {
            result.second.first = std::max(result.second.first, first2->second.first);
            result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
            dx++;
        } while ((dx != last2) && (dx->first.first == localTrace));
        remove_duplicates(result.second.second);
        out.emplace_back(result);
        result.second.second.clear();
        first2 = dx;
    }
}

/**
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void and_fast_timed(const Result& lhs, const Result& rhs, Result& out, const DeclareDataAware *manager = nullptr, const std::vector<size_t>& lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    ResultIndex pair, pair1;
    ResultRecord result{{0, 0}, {1.0, {}}};
    bool hasMatch;
    marked_event join = marked_event::join(0,0);
    std::unordered_set<std::string> cache;

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
                    join.id.parts.left = pair.second = GET_ACTIVATION_EVENT(elem);
                    e1 = manager->GetPayloadDataFromEvent(pair.first, pair.second, true, cache);

                    for (const marked_event &elem1: first2->second.second) {
                        if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                        join.id.parts.right = pair1.second = GET_TARGET_EVENT(elem1);

                        if (manager->checkValidity(e1, first2->first.first, join.id.parts.right)) {
                            hasMatch = true;
                            result.second.second.emplace_back(join);
                        }
                    }
                }
            } else {
                hasMatch = true;
                result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                result.second.second.insert(result.second.second.end(), first2->second.second.begin(), first2->second.second.end());
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
 * This implementation is faster than the one provided in the logical implementation, as we don't group by trace id,
 * but we directly do the loop on all the events of the trace
 * @author Giacomo Bergami
 *
 * @param lhs
 * @param rhs
 * @param out
 * @param manager
 */
inline void and_fast_untimed(const Result& lhs, const Result& rhs, Result& out, const DeclareDataAware *manager = nullptr, const std::vector<size_t>& lengths = {}) {
    auto first1 = lhs.begin(), first2 = rhs.begin(),
            last1 = lhs.end(), last2 = rhs.end();
    env e1, e2;
    ResultRecord result{{0, 0}, {0.0, {}}};
    ResultRecord idx{{0, 0}, {0.0, {}}};
    ResultIndex pair, pair1;
    auto join = marked_event::join(0, 0);
    bool hasMatch, completeInsertionRight;
    std::unordered_set<std::string> cache;
    while (first1 != last1) {
        if (first2 == last2) {
            return;
        }
        if (first1->first.first > first2->first.first) {
            first2++;
        } else if (first1->first.first < first2->first.first) {
            first1++;
        } else {
            auto localTrace = first1->first.first;
            idx.first.first = first1->first.first+1; // pointing towards the next trace
            auto endFirst1 = std::upper_bound(first1, last1, idx);
            decltype(endFirst1) endFirst2;
            pair.first = pair1.first = localTrace;
            result.first = first1->first;
            result.second.first = std::min(first1->second.first, first2->second.first);
            result.second.second.clear();
            hasMatch = completeInsertionRight = false;
            for (; first1 != endFirst1; first1++) {
                auto dx = first2;
                do {

                    if (manager && (!first1->second.second.empty()) && (!first2->second.second.empty())) {
                        for (const marked_event &elem: first1->second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            join.id.parts.left = pair.second = GET_ACTIVATION_EVENT(elem);
                            e1 = manager->GetPayloadDataFromEvent(pair.first, pair.second, true, cache);

                            for (const marked_event &elem1: first2->second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                join.id.parts.right = GET_TARGET_EVENT(elem1);

                                if (manager->checkValidity(e1, localTrace, join.id.parts.right)) {
                                    hasMatch = true;
                                    result.second.second.emplace_back(join);
                                }
                            }
                        }
                    } else {
                        hasMatch = true;
                        if (!completeInsertionRight) {
                            result.second.second.insert(result.second.second.end(), dx->second.second.begin(), dx->second.second.end());
                        }
                    }

                    dx++;
                } while ((dx != last2) && (dx->first.first == localTrace));

                endFirst2 = dx;
                if (!manager){
                    completeInsertionRight = true;
                    result.second.second.insert(result.second.second.end(), first1->second.second.begin(), first1->second.second.end());
                }
            }

            if (hasMatch) {
                remove_duplicates(result.second.second);
                out.emplace_back(result);
            }

            first2 = endFirst2;
        }
    }
}

inline void next_fast(const Result &section, Result& temp, const std::vector<size_t>& lengths = {}) {
    next_logical(section, temp, lengths);
}

/**
 *
 * @author Samuel 'Sam' Appleby
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void future_fast_timed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    result = section;
}

/**
 *
 * @author Samuel 'Sam' Appleby
 *
 * @param section
 * @param lengths
 * @param result
 */
inline void future_fast_untimed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    result = section;
}


inline void global_fast_timed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    result.clear();
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();

    ResultIndex first;
    ResultRecordSemantics second{1.0, 0.0};
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
        for (int64_t i = (upper - 1)->first.second; i >= 0; i--) {
            first.second = i;
            const uint32_t dist = std::distance(it, upper);

            if ((cp.first.first == it->first.first) && (dist == (cp.first.second - it->first.second))) {
                second.first = std::min(it->second.first, second.first);
                second.second.insert(second.second.begin(), it->second.second.begin(), it->second.second.end());
                remove_duplicates(second.second);
                it--;
            } else {
                break; // If after this the condition does not hold, then it means that in the remainder I will have
                // events that are not matching the condition
            }
            toBeReversed.emplace_back(first, second);
        }

        // Inserting the elements in reversed order
        result.insert(result.end(), std::make_move_iterator(toBeReversed.rbegin()),
                      std::make_move_iterator(toBeReversed.rend()));

        second.second.clear();
    }

}

inline void global_fast_untimed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    auto lower = section.begin(), upper = section.begin();
    auto end = section.end();
    result.clear();

    ResultIndex first{0, 0};
    ResultRecordSemantics second{1.0, {}};
    ResultRecord cp{{0,   0},
                    {1.0, {}}};
    while (upper != end) {
        uint32_t currentTraceId = upper->first.first;
        first.first = cp.first.first = currentTraceId;
        cp.first.second = lengths.at(currentTraceId);
        cp.second.second.clear();
        lower = upper;
        second.second.clear();
        upper = lower + (cp.first.second-1);//std::upper_bound(lower, section.end(), cp);
        ///const uint32_t dist = std::distance(lower, upper - 1);
        if ((upper < end) && (upper->first.first == lower->first.first)) {
            populateAndReturnEvents(lower, ++upper, second.second);
            result.emplace_back(first, second);
        } else {
            cp.first.first = currentTraceId+1;
            cp.first.second = 0;
            upper = std::lower_bound(lower, section.end(), cp);
        }
    }
}

inline void negated_fast_untimed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    size_t first1 = 0, last1 = lengths.size();
    ResultRecord rc{{0, 0}, {1.0, {}}};
    auto first2 = section.begin(), last2 = section.end();
    for (; first1 != last1; ) {
        if (first2 == last2) {
            while (first1 != last1) {
                rc.first.first = first1++;
                result.emplace_back(rc);
            }
        }
        if (first1 > first2->first.first) {
            first2++;
        } else if (first1 < first2->first.first) {
            rc.first.first = first1++;
            result.emplace_back(rc);
        } else {
            first1++;
            first2++;
        }
    }
}

inline void negated_fast_timed(const Result &section, Result& result, const std::vector<size_t>& lengths) {
    ResultIndex last1{lengths.size(), 0};
    ResultRecord rc{{0, 0}, {1.0, {}}};
    auto first2 = section.begin(), last2 = section.end();
    for (; rc.first != last1; ) {
        if (first2 == last2) {
            while (rc.first != last1) {
                result.emplace_back(rc);
                rc.first.second++;
                if (rc.first.second >= lengths.at(rc.first.first)) {
                    rc.first.first++;
                    rc.first.second = 0;
                }
            }
        }
        if (rc.first > first2->first) {
            first2++;
        } else if (rc.first < first2->first) {
            result.emplace_back(rc);
            rc.first.second++;
            if (rc.first.second >= lengths.at(rc.first.first)) {
                rc.first.first++;
                rc.first.second = 0;
            }
        } else {
            rc.first.second++;
            if (rc.first.second >= lengths.at(rc.first.first)) {
                rc.first.first++;
                rc.first.second = 0;
            }
            first2++;
        }
    }
}


/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param aResult
 * @param bResult
 * @param result
 * @param manager
 * @param lengths
 */
inline void aAndFutureB_timed(const Result& aResult, const Result& bResult, Result& result, const DeclareDataAware *manager = nullptr, const std::vector<size_t>& lengths = {}) {
    if (bResult.empty()) {
        result.clear();
        return;
    }
    auto bCurrent = bResult.begin(), bEnd = bResult.end();
    ResultRecord rcx;
    marked_event join = marked_event::join(0,0);
    bool hasMatch;
    std::unordered_set<std::string> cache;

    for (auto aCurrent = aResult.begin(), aEnd = aResult.end(); aCurrent != aEnd; /*++d_first*/) {
        /* Our iterators should never be pointing at the same event */
        // assert(aCurrent != bCurrent);

        if (aCurrent->first > bCurrent->first) {
            bCurrent++;
            if (bCurrent == bEnd) break;
        } else {
            auto newItr = bCurrent;
            rcx.first = aCurrent->first;
            rcx.second.second.clear();
            rcx.second.first = 1.0;
            hasMatch = false;

            while (newItr != bEnd) {
                if(newItr->first.first != aCurrent->first.first){
                    break;
                }
                if (manager) {
                    for (const auto &elem: aCurrent->second.second) {
                        if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                        join.id.parts.left = GET_ACTIVATION_EVENT(elem);
                        env e1 = manager->GetPayloadDataFromEvent(aCurrent->first.first, join.id.parts.left, true, cache);
                        for (const auto &elem1: newItr->second.second) {
                            if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                            join.id.parts.right = GET_TARGET_EVENT(elem1);

                            if (manager->checkValidity(e1, newItr->first.first, join.id.parts.right)) {
                                hasMatch = true;
                                rcx.second.second.push_back(join);
                                rcx.second.first *= (1.0 - std::min(aCurrent->second.first, newItr->second.first));
                            }
                        }
                    }
                } else {
                    hasMatch = true;
                    rcx.second.second.insert(rcx.second.second.end(), newItr->second.second.begin(), newItr->second.second.end());
                }

                newItr++;
            }

            if (hasMatch) {
                if (!manager) rcx.second.second.insert(rcx.second.second.end(), aCurrent->second.second.begin(), aCurrent->second.second.end());
                remove_duplicates(rcx.second.second);
                if (manager) rcx.second.first = 1.0 - rcx.second.first;
                result.emplace_back(rcx);
            }

            if (aCurrent->first == bCurrent->first) {
                bCurrent++;
                if (bCurrent == bEnd) break;
            }

            aCurrent++;
        }
    }
}


/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param aResult
 * @param bResult
 * @param result
 * @param manager
 * @param lengths
 */
inline void aAndNextGloballyB_timed(const Result& a, const Result& b,Result& result, const DeclareDataAware *manager = nullptr, const std::vector<size_t> lengths = {}) {
    if (b.empty()) {
        result.clear();
        return;
    }
    auto bCurrent = b.begin(), bEnd = b.end();
    ResultRecord rcx;
    marked_event join = marked_event::join(0,0);
    bool hasMatch;
    std::unordered_set<std::string> cache;

    for (auto aCurrent = a.begin(), aEnd = a.end(); aCurrent != aEnd; ) {

        if (aCurrent->first > bCurrent->first) {
            bCurrent++;
            if (bCurrent == bEnd) break;
        } else {
            auto newItr = bCurrent;
            rcx.first = aCurrent->first;
            rcx.second.second.clear();
            rcx.second.first = 1.0;
            hasMatch = false;

            if(newItr->first == aCurrent->first){
                newItr++;
            }

            if (newItr == bEnd) return;

            if((newItr->first.first == aCurrent->first.first) && (newItr->first.second - aCurrent->first.second == 1)){
                std::vector<uint16_t> activations{};

                uint16_t count = 0;

                while (newItr != bEnd) {
                    if(newItr->first.first != aCurrent->first.first){
                        break;
                    }
                    if (manager) {
                        for (const auto &elem: aCurrent->second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            join.id.parts.left = GET_ACTIVATION_EVENT(elem);
                            env e1 = manager->GetPayloadDataFromEvent(aCurrent->first.first, join.id.parts.left, true, cache);
                            for (const auto &elem1: newItr->second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                join.id.parts.right = GET_TARGET_EVENT(elem1);

                                if (manager->checkValidity(e1, newItr->first.first, join.id.parts.right)) {
                                    rcx.second.second.push_back(join);
                                    rcx.second.first *= (1.0 - std::min(aCurrent->second.first, newItr->second.first));
                                    count++;
                                    hasMatch = true;
                                }
                            }
                        }
                    } else {
                        count++;
                        hasMatch = true;
                        rcx.second.second.insert(rcx.second.second.end(), newItr->second.second.begin(), newItr->second.second.end());
                    }

                    newItr++;
                }

                hasMatch = hasMatch && (count == lengths.at( aCurrent->first.first) - 1 - aCurrent->first.second);
            }

            if (hasMatch) {
                if (!manager) rcx.second.second.insert(rcx.second.second.end(), aCurrent->second.second.begin(), aCurrent->second.second.end());
                remove_duplicates(rcx.second.second);
                if (manager) rcx.second.first = 1.0 - rcx.second.first;
                result.emplace_back(rcx);
            }

            if (aCurrent->first == bCurrent->first) {
                bCurrent++;
                if (bCurrent == bEnd) break;
            }

            aCurrent++;
        }
    }

}



/**
 *
 * @author Samuel 'Sam' Appleby, Giacomo Bergami
 *
 * @param aResult
 * @param bResult
 * @param result
 * @param manager
 * @param lengths
 */
inline void aAndGloballyB_timed(const Result& a, const Result& b,Result& result, const DeclareDataAware *manager = nullptr, const std::vector<size_t> lengths = {}) {
    if (b.empty()) {
        result.clear();
        return;
    }
    auto bCurrent = b.begin(), bEnd = b.end();
    ResultRecord rcx;
    marked_event join = marked_event::join(0,0);
    bool hasMatch;
    std::unordered_set<std::string> cache;

    for (auto aCurrent = a.begin(), aEnd = a.end(); aCurrent != aEnd; ) {

        if (aCurrent->first > bCurrent->first) {
            bCurrent++;
            if (bCurrent == bEnd) break;
        } else {
            auto newItr = bCurrent;
            rcx.first = aCurrent->first;
            rcx.second.second.clear();
            rcx.second.first = 1.0;
            hasMatch = false;

            if (newItr == bEnd) return;

            if((newItr->first.first == aCurrent->first.first) && (newItr->first.second == aCurrent->first.second)){
                std::vector<uint16_t> activations{};

                uint16_t count = 0;

                while (newItr != bEnd) {
                    if(newItr->first.first != aCurrent->first.first){
                        break;
                    }
                    if (manager) {
                        for (const auto &elem: aCurrent->second.second) {
                            if (!IS_MARKED_EVENT_ACTIVATION(elem)) continue;
                            join.id.parts.left = GET_ACTIVATION_EVENT(elem);
                            env e1 = manager->GetPayloadDataFromEvent(aCurrent->first.first, join.id.parts.left, true, cache);
                            for (const auto &elem1: newItr->second.second) {
                                if (!IS_MARKED_EVENT_TARGET(elem1)) continue;
                                join.id.parts.right = GET_TARGET_EVENT(elem1);

                                if (manager->checkValidity(e1, newItr->first.first, join.id.parts.right)) {
                                    rcx.second.second.push_back(join);
                                    rcx.second.first *= (1.0 - std::min(aCurrent->second.first, newItr->second.first));
                                    count++;
                                    hasMatch = true;
                                }
                            }
                        }
                    } else {
                        count++;
                        hasMatch = true;
                        rcx.second.second.insert(rcx.second.second.end(), newItr->second.second.begin(), newItr->second.second.end());
                    }

                    newItr++;
                }

                hasMatch = hasMatch && (count == lengths.at( aCurrent->first.first)  - aCurrent->first.second);
            }

            if (hasMatch) {
                if (!manager) rcx.second.second.insert(rcx.second.second.end(), aCurrent->second.second.begin(), aCurrent->second.second.end());
                remove_duplicates(rcx.second.second);
                if (manager) rcx.second.first = 1.0 - rcx.second.first;
                result.emplace_back(rcx);
            }

            if (aCurrent->first == bCurrent->first) {
                bCurrent++;
                if (bCurrent == bEnd) break;
            }

            aCurrent++;
        }
    }

}

inline void until_fast_timed(const Result &aSection, const Result &bSection, Result &temp,
                             const DeclareDataAware *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    until_logic_timed(aSection, bSection, temp, manager, lengths);
}

inline void until_fast_untimed(const Result &aSection, const Result &bSection, Result &temp,
                               const DeclareDataAware *manager = nullptr, const std::vector<size_t> &lengths = {}) {
    auto bCurrent = bSection.begin();
    auto localBUpper = bCurrent;
    auto upper = bSection.end();

    auto aIt = aSection.begin(), aEn = aSection.begin(), bestAEn = aSection.begin();
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
        aEn = bestAEn = aIt;

        bool atLeastOneResult = false;
        for (; bCurrent != localBUpper; bCurrent++) {
            Fut.first = bCurrent->first.first;
            if (bCurrent->first.second == 0) {
                cpResult.second.second.insert(cpResult.second.second.end(), bCurrent->second.second.begin(),
                                              bCurrent->second.second.end());
                atLeastOneResult = true;
            } else {
                if (aIt->first.second > 0) break;
                cpAEn.first.second = bCurrent->first.second - 1;
                if (aIt >= upperA) break;

                // Applying the same concept from the new globally timed... (2)
                aEn = aIt + (cpAEn.first.second);//std::upper_bound(aEn, upperA, cpAEn);
                if ((aEn >= upperA) || (aEn->first.first > currentTraceId) || (aEn->first.second != ((bCurrent->first.second)) - 1)) {
                    // Rationale: (1)
                    // if the condition does not hold for a time [startEventId, lower->first.second-1], it is because
                    // one event makes it not hold. Therefore, it should never hold even if you are extending the data
                    // that you have.
                    break;
                } else {
                    if (manager) {
                        ++aEn;
                        bestAEn = aEn;
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
                        populateAndReturnEvents(aIt, ++aEn, cpResult.second.second);
                        bestAEn = aEn;
                        cpResult.second.second.insert(cpResult.second.second.end(), bCurrent->second.second.begin(),
                                                      bCurrent->second.second.end());
                        atLeastOneResult = true;
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
        aIt = bestAEn;
    }
}


inline void implies_fast_timed(const Result &aSection, const Result &bSection, const Result &notaSection, Result& result, const DeclareDataAware* manager = nullptr, const std::vector<size_t>& lengths = {}) {
    Result aTrue;
    and_fast_timed(aSection, bSection, aTrue, manager, lengths);
    or_fast_timed(aTrue, notaSection, result, nullptr, lengths);
}

inline void implies_fast_untimed(const Result &aSection, const Result &bSection, const Result &notaSection, Result& result, const DeclareDataAware* manager = nullptr, const std::vector<size_t>& lengths = {}) {
    Result aTrue;
    and_fast_untimed(aSection, bSection, aTrue, manager, lengths);
    or_fast_untimed(aTrue, notaSection, result, nullptr, lengths);
}

#endif //KNOBAB_SERVER_FAST_LTLF_OPERATORS_H
