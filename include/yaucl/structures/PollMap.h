/*
 * PollMap.h
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 26/09/18.
//

#ifndef EDGEINDEXING_POLLMAP_H
#define EDGEINDEXING_POLLMAP_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <limits>

namespace yaucl {
    namespace structures {
        /**
 *
 *
 * @tparam K  Score associated to the element
 * @tparam V  Value which may have multiple score. For each element, we select the highest score
 */
        template <typename K, typename V> class PollMap {
            std::map<K, std::set<V>> poll;
            std::unordered_map<V, K> valueScore;
            bool sanitize = false;
            size_t topK;

            void addKeyValue(K key, V value) {
                sanitize = false;
                typename std::unordered_map<V, K>::iterator ptr = valueScore.find(value);
                if (ptr != valueScore.end() && ptr->second < key) {
                    valueScore[value] = key;
                    typename std::map<K, std::set<V>>::iterator pf = poll.find(key);
                    if (pf != poll.cend()) {
                        auto toErase = pf->second.find(value);
                        if (toErase != pf->second.cend())
                            pf->second.erase(toErase);
                        if (pf->second.empty()) {
                            poll.erase(pf);
                        }
                    }
                }
                typename std::map<K, std::set<V>>::iterator kfind = poll.find(key);
                if (kfind == poll.cend()) {
                    typename std::set<V> newSet;
                    newSet.insert(value);
                    poll.emplace(key, newSet);
                    valueScore[value] = key;
                } else {
                    poll[key].insert(value);
                    valueScore[value] = key;
                }
            }

        public:
            /**
             * TopK map initialization.
             * @param topK  If the integer is negative, then we preserve all the possible results
             */
            PollMap(size_t topK = std::numeric_limits<int>::max()) {
                this->topK = (topK <= 0) ? std::numeric_limits<int>::max() : topK;
            }

            PollMap(const PollMap& x) : poll{x.poll}, valueScore{x.valueScore}, sanitize{x.sanitize}, topK{x.topK} {}
            PollMap& operator=(const PollMap& x) {
                poll = x.poll;
                valueScore = x.valueScore;
                sanitize = x.sanitize;
                topK = x.topK;
            }

            size_t size() const {
                return valueScore.size();
            }

            void resize(size_t topK) {
                this->topK = (topK <= 0) ? std::numeric_limits<int>::max() : topK;
                const size_t originalSize = poll.size();
                auto eraseIter = poll.begin();
                auto realNumToRemove = std::max(((int)originalSize)-((int)topK), 0);
                if (!realNumToRemove) return;
                std::advance(eraseIter, realNumToRemove);
                poll.erase(poll.begin(), eraseIter);
            }

            void add(K key, V value) {
                if (poll.size() <= this->topK) {
                    addKeyValue(key, value);
                } else {
                    typename std::map<K, std::set<V>>::iterator cmpPtr = poll.begin();
                    /*if (cmpPtr->first < key) {
                        poll.erase(cmpPtr);
                        addKeyValue(key, value);
                    }*/
                    K first = cmpPtr->first;
                    if (first < key) {
                        poll.erase(cmpPtr);
                    } else if (first > key)
                        return;
                    addKeyValue(key, value);
                }
            }

            std::vector<K> getTopKKeys() {
                std::vector<K> elements;
                size_t limitK = 0;
                for (auto x = poll.rbegin(); x != poll.rend(); x++) {
                    size_t current = x->second.size();
                    if (elements.empty()) {
                        elements.emplace_back(x->first);
                        if (current >= topK)
                            return elements;
                    } else {
                        /*if (current + limitK >= topK)
                            return elements;*/
                        elements.emplace_back(x->first);
                    }
                    limitK += current;
                    if (limitK >= topK)
                        return elements;
                }
                return elements;
            }

            // Please note that the multimap won't be in reversed order, and therefore we have to reversely scan it
            void getPoll(std::multimap<K, V>& toretMap) {
                doSanitize();
                toretMap.clear();
                //typename std::map<K, std::set<V>>::iterator it;
                for (const auto& it : poll) {
                    for (V value : it.second) {
                        toretMap.emplace(it.first, value);
                    }
                }
            }

            const std::map<K, std::set<V>>& getPoll() const {
                //doSanitize();
                return poll;
            }

            void getRankedPoll(std::map<V, size_t>& rankingMap) {
                doSanitize();
                rankingMap.clear();
                size_t currentPos = 1;
                for (auto ls = poll.rbegin(); ls != poll.rend(); ls++) {
                    for (const V& x : ls->second) {
                        rankingMap[x] = currentPos;
                    }
                    currentPos++;
                }
            }

            void getNarrowRankedPoll(std::map<V, size_t>& rankingMap) {
                doSanitize();
                rankingMap.clear();
                size_t currentPos = 1;
                for (auto lsx : getTopKKeys()) {
                    for (const V& x : poll[lsx]) {
                        rankingMap[x] = currentPos;
                    }
                    currentPos++;
                }
            }

            void printNarrow() {
                doSanitize();
                for (K k : getTopKKeys()) {
                    std::cout << k << " = {";
                    for (const V& x : poll[k]) {
                        std::cout << x << " ";
                    }
                    std::cout << "}" << std::endl;
                }
            }

        private:
            void doSanitize() {
                if (!sanitize) {
                    valueScore.clear();
                    std::multimap<K, V> map{};
                    typename std::map<K, std::set<V>>::iterator it;
                    for ( it = poll.begin(); it != poll.end(); it++ ) {
                        for (V value : it->second) {
                            map.emplace(it->first, value);
                        }
                    }
                    poll.clear();
                    typename std::multimap<K, V>::reverse_iterator it2;
                    // value sanitizing process
                    for (it2 = map.rbegin(); it2 != map.rend(); it2++) {
                        if (valueScore.find(it2->second) == valueScore.end()) {
                            valueScore[it2->second] = it2->first;
                            typename std::map<K, std::set<V>>::iterator pf = poll.find(it2->first);
                            if (pf == poll.cend()) {
                                std::set<V> values{};
                                values.insert(it2->second);
                                poll.emplace(it2->first, values);
                            } else {
                                poll[it2->first].insert(it2->second);
                            }
                        }
                    }
                    sanitize = true;
                }
            }
        };
    }
}



#endif //EDGEINDEXING_POLLMAP_H
