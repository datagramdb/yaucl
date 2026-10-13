//
// Created by gyankos on 13/10/26.
//

#ifndef METATENSOR_MULTINODE_H
#define METATENSOR_MULTINODE_H

#include <unordered_map>
#include <cstddef>

namespace logds::yaucl::gamma_context {
    using ListId = std::size_t;

    template <typename Key, typename Value>
    struct MultiNode {
        Key key;
        Value value;

        // Indice inverso per appartenenza a liste multiple (Nessun Lock)
        std::unordered_map<ListId, MultiNode*> next;
        std::unordered_map<ListId, MultiNode*> prev;

        MultiNode(Key k, Value v) : key(std::move(k)), value(v) {}

        MultiNode* get_next(ListId id) const {
            auto it = next.find(id);
            return it != next.end() ? it->second : nullptr;
        }

        MultiNode* get_prev(ListId id) const {
            auto it = prev.find(id);
            return it != prev.end() ? it->second : nullptr;
        }

        void set_links(ListId id, MultiNode* p, MultiNode* n) {
            if (p) prev[id] = p; else prev.erase(id);
            if (n) next[id] = n; else next.erase(id);
        }

        void erase_list_id(ListId id) {
            prev.erase(id);
            next.erase(id);
        }
    };

    template <typename Key, typename Value>
struct MultiNodeV2 {
    Key key;
    Value value;

    // --- OPTIMIZATION: HYBRID LINK STORAGE ---
    // Soglia per la Small Object Optimization dei link inversi
    static constexpr std::size_t SMALL_CAPACITY = 4;

    // Buffer statico in linea per evitare l'allocazione della mappa hash su contesti piccoli
    std::pair<ListId, MultiNodeV2*> small_next[SMALL_CAPACITY];
    std::pair<ListId, MultiNodeV2*> small_prev[SMALL_CAPACITY];
    std::size_t small_next_size = 0;
    std::size_t small_prev_size = 0;

    // Struttura di fallback usata solo se il numero di liste concorrenti supera SMALL_CAPACITY
    std::unordered_map<ListId, MultiNodeV2*>* large_next = nullptr;
    std::unordered_map<ListId, MultiNodeV2*>* large_prev = nullptr;

    MultiNodeV2(Key k = {}, Value v = {}) : key(std::move(k)), value(v) {}

    ~MultiNodeV2() {
        delete large_next;
        delete large_prev;
    }

    MultiNodeV2* get_next(ListId id) const {
        if (!large_next) {
            // Ricerca lineare nel buffer statico (Velocissima, Cache-Locality ottimale)
            for (std::size_t i = 0; i < small_next_size; ++i) {
                if (small_next[i].first == id) return small_next[i].second;
            }
            return nullptr;
        }
        auto it = large_next->find(id);
        return it != large_next->end() ? it->second : nullptr;
    }

    MultiNodeV2* get_prev(ListId id) const {
        if (!large_prev) {
            for (std::size_t i = 0; i < small_prev_size; ++i) {
                if (small_prev[i].first == id) return small_prev[i].second;
            }
            return nullptr;
        }
        auto it = large_prev->find(id);
        return it != large_prev->end() ? it->second : nullptr;
    }

    void set_links(ListId id, MultiNodeV2* p, MultiNodeV2* n) {
        // Gestione NEXT
        if (large_next) {
            if (n) (*large_next)[id] = n; else large_next->erase(id);
        } else {
            bool found = false;
            for (std::size_t i = 0; i < small_next_size; ++i) {
                if (small_next[i].first == id) {
                    if (n) small_next[i].second = n;
                    else {
                        small_next[i] = small_next[--small_next_size];
                    }
                    found = true; break;
                }
            }
            if (!found && n) {
                if (small_next_size < SMALL_CAPACITY) {
                    small_next[small_next_size++] = {id, n};
                } else {
                    // Upgrade a mappa hash reale (Fallback)
                    large_next = new std::unordered_map<ListId, MultiNodeV2*>();
                    for (std::size_t i = 0; i < small_next_size; ++i) {
                        (*large_next)[small_next[i].first] = small_next[i].second;
                    }
                    (*large_next)[id] = n;
                }
            }
        }

        // Gestione PREV
        if (large_prev) {
            if (p) (*large_prev)[id] = p; else large_prev->erase(id);
        } else {
            bool found = false;
            for (std::size_t i = 0; i < small_prev_size; ++i) {
                if (small_prev[i].first == id) {
                    if (p) small_prev[i].second = p;
                    else {
                        small_prev[i] = small_prev[--small_prev_size];
                    }
                    found = true; break;
                }
            }
            if (!found && p) {
                if (small_prev_size < SMALL_CAPACITY) {
                    small_prev[small_prev_size++] = {id, p};
                } else {
                    large_prev = new std::unordered_map<ListId, MultiNodeV2*>();
                    for (std::size_t i = 0; i < small_prev_size; ++i) {
                        (*large_prev)[small_prev[i].first] = small_prev[i].second;
                    }
                    (*large_prev)[id] = p;
                }
            }
        }
    }

    void erase_list_id(ListId id) {
        set_links(id, nullptr, nullptr);
    }
};


}

#endif //METATENSOR_MULTINODE_H
