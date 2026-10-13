//
// Created by gyankos on 13/10/26.
//

#ifndef METATENSOR_HIERARCHICALLISTVIEW_H
#define METATENSOR_HIERARCHICALLISTVIEW_H

#include <unordered_map>
#include <unordered_set>
#include <yaucl/structures/gamma_context/MultiNode.h>
#include <yaucl/structures/gamma_context/ViewFactory.h>

namespace logds::yaucl::gamma_context {
    /*
template <typename Key, typename Value>
class HierarchicalListView {
private:
    ListId my_id;
    ViewFactory<Key, Value>& factory;
    HierarchicalListView* parent = nullptr;

    std::unordered_map<Key, std::shared_ptr<MultiNode<Key, Value>>> local_lookup;
    std::unordered_set<Key> detached_keys;
    bool is_globally_detached = false;

    MultiNode<Key, Value>* local_head = nullptr;
    MultiNode<Key, Value>* local_tail = nullptr;

    // Contatori per l'analisi ammortizzata
    std::size_t read_count = 0;
    std::size_t total_nodes_at_creation = 0;

    HierarchicalListView(ListId id, ViewFactory<Key, Value>& fac, HierarchicalListView* p = nullptr)
        : my_id(id), factory(fac), parent(p) {
        if (parent) {
            total_nodes_at_creation = parent->local_lookup.size() + parent->detached_keys.size();
        }
    }

    friend class ViewFactory<Key, Value>;

    void append_local_internal(MultiNode<Key, Value>* node) {
        if (!local_head) {
            local_head = local_tail = node;
            node->set_links(my_id, nullptr, nullptr);
        } else {
            local_tail->set_links(my_id, local_tail->get_prev(my_id), node);
            node->set_links(my_id, local_tail, nullptr);
            local_tail = node;
        }
    }

    void detach_local_internal(MultiNode<Key, Value>* node) {
        if (!node) return;
        MultiNode<Key, Value>* p = node->get_prev(my_id);
        MultiNode<Key, Value>* n = node->get_next(my_id);

        if (p) p->set_links(my_id, p->get_prev(my_id), n);
        else local_head = n;

        if (n) n->set_links(my_id, p, n->get_next(my_id));
        else local_tail = p;

        node->set_links(my_id, nullptr, nullptr);
    }

    // Controllo euristico a costo zero (chiamato solo in scrittura)
    void valuta_auto_detach() {
        if (is_globally_detached || total_nodes_at_creation == 0) return;
        double alpha = static_cast<double>(detached_keys.size()) / total_nodes_at_creation;

        // 1. Soglia RAM rigida (20%)
        if (alpha > 0.20) {
            detach();
            return;
        }

        // 2. Soglia CPU single-thread sbloccata (70% se molte letture)
        double rapporto_letture = static_cast<double>(read_count) / total_nodes_at_creation;
        if (rapporto_letture > 1.0 && alpha > 0.70) {
            detach();
        }
    }

public:
    ~HierarchicalListView() {
        factory.unregister_view(my_id);
    }

    ListId get_id() const { return my_id; }

    void detach() {
        if (is_globally_detached || !parent) return;

        ListId parent_id = parent->my_id;
        MultiNode<Key, Value>* current = parent->local_head;

        MultiNode<Key, Value>* frozen_head = nullptr;
        MultiNode<Key, Value>* frozen_tail = nullptr;

        while (current) {
            if (detached_keys.find(current->key) == detached_keys.end()) {
                auto cloned_node = std::make_shared<MultiNode<Key, Value>>(current->key, current->value);
                factory.register_node(cloned_node);
                local_lookup[current->key] = cloned_node;
                detached_keys.insert(current->key);

                if (!frozen_head) {
                    frozen_head = frozen_tail = cloned_node.get();
                    cloned_node->set_links(my_id, nullptr, nullptr);
                } else {
                    frozen_tail->set_links(my_id, frozen_tail->get_prev(my_id), cloned_node.get());
                    cloned_node->set_links(my_id, frozen_tail, nullptr);
                    frozen_tail = cloned_node.get();
                }
            }
            current = current->get_next(parent_id);
        }

        if (frozen_head) {
            if (local_head) {
                frozen_tail->set_links(my_id, frozen_tail->get_prev(my_id), local_head);
                local_head->set_links(my_id, frozen_tail, local_head->get_next(my_id));
                local_head = frozen_head;
            } else {
                local_head = frozen_head;
                local_tail = frozen_tail;
            }
        }

        parent = nullptr;
        is_globally_detached = true;
    }

    template <typename... Args>
    void emplace(Key key, Args&&... args) {
        auto new_node = std::make_shared<MultiNode<Key, Value>>(key, Value(std::forward<Args>(args)...));
        factory.register_node(new_node);
        local_lookup[key] = new_node;
        detached_keys.insert(key);
        append_local_internal(new_node.get());
        valuta_auto_detach();
    }

    std::shared_ptr<MultiNode<Key, Value>> find_node(const Key& key) {
        ++read_count; // Sola istruzione Assembly senza barriere atomiche
        if (detached_keys.find(key) != detached_keys.end() || is_globally_detached) {
            auto it = local_lookup.find(key);
            return it != local_lookup.end() ? it->second : nullptr;
        }
        if (parent) return parent->find_node(key);
        return nullptr;
    }

    void update_value(const Key& key, Value new_value) {
        if (is_globally_detached || detached_keys.find(key) != detached_keys.end()) {
            auto it = local_lookup.find(key);
            if (it != local_lookup.end()) {
                MultiNode<Key, Value>* node = it->second.get();
                node->value = new_value;
                detach_local_internal(node);
                append_local_internal(node);
            }
        } else {
            detached_keys.insert(key);
            auto isolated_node = std::make_shared<MultiNode<Key, Value>>(key, new_value);
            factory.register_node(isolated_node);
            local_lookup[key] = isolated_node;
            append_local_internal(isolated_node.get());
        }
        valuta_auto_detach();
    }
};

template <typename Key, typename Value>
std::unique_ptr<HierarchicalListView<Key, Value>> ViewFactory<Key, Value>::create_root_view() {
    return std::unique_ptr<HierarchicalListView<Key, Value>>(new HierarchicalListView<Key, Value>(generate_id(), *this));
}

template <typename Key, typename Value>
std::unique_ptr<HierarchicalListView<Key, Value>> ViewFactory<Key, Value>::create_child_view(HierarchicalListView<Key, Value>& parent) {
    return std::unique_ptr<HierarchicalListView<Key, Value>>(new HierarchicalListView<Key, Value>(generate_id(), *this, &parent));
}*/


    template<typename Key, typename Value>
    class HierarchicalListView {
    private:
        ListId my_id;
        ViewFactory<Key, Value> &factory;
        HierarchicalListView *parent = nullptr;

        // --- STRUTTURA PER MODALITÀ LAZY / GERARCHICA (> 2000) ---
        // Sfrutta appieno l'Arena Allocator tramite puntatori grezzi contigui
        std::unordered_map<Key, MultiNodeV2<Key, Value> *> local_lookup;
        bool is_globally_detached = false;

        static constexpr std::size_t SMALL_SET_CAPACITY = 8;
        Key small_detached_keys[SMALL_SET_CAPACITY];
        std::size_t small_detached_size = 0;
        std::unordered_set<Key> *large_detached_keys = nullptr;

        MultiNodeV2<Key, Value> *local_head = nullptr;
        MultiNodeV2<Key, Value> *local_tail = nullptr;
        std::size_t read_count = 0;
        std::size_t total_nodes_at_creation = 0;

        // --- STRUTTURA PER MODALITÀ FLAT NATIVA (<= 2000) ---
        bool is_pure_flat_mode = false;
        std::list<std::pair<Key, Value> > flat_list;
        std::unordered_map<Key, typename std::list<std::pair<Key, Value> >::iterator> flat_lookup;

        HierarchicalListView(ListId id, ViewFactory<Key, Value> &fac, HierarchicalListView *p = nullptr)
            : my_id(id), factory(fac), parent(p) {
            if (parent) {
                total_nodes_at_creation = parent->size();

                // --- ATTIVAZIONE MODALITÀ FLAT NATIVA ALLA NASCITA ---
                if (total_nodes_at_creation <= 2000) {
                    is_pure_flat_mode = true;
                    if (parent->is_pure_flat_mode) {
                        flat_list = parent->flat_list;
                        for (auto it = flat_list.begin(); it != flat_list.end(); ++it) {
                            flat_lookup[it->first] = it;
                        }
                    } else {
                        parent->dump_to_flat(flat_list, flat_lookup);
                    }
                    parent = nullptr; // Tagliamo i ponti logici, siamo autonomi e contigui
                }
            } else {
                is_pure_flat_mode = true;
            }
        }

        friend class ViewFactory<Key, Value>;

        std::size_t get_detached_size() const {
            return large_detached_keys ? large_detached_keys->size() : small_detached_size;
        }

        bool is_key_detached(const Key &key) const {
            if (large_detached_keys) return large_detached_keys->find(key) != large_detached_keys->end();
            for (std::size_t i = 0; i < small_detached_size; ++i) {
                if (small_detached_keys[i] == key) return true;
            }
            return false;
        }

        void insert_detached_key(const Key &key) {
            if (large_detached_keys) {
                large_detached_keys->insert(key);
                return;
            }
            for (std::size_t i = 0; i < small_detached_size; ++i) {
                if (small_detached_keys[i] == key) return;
            }
            if (small_detached_size < SMALL_SET_CAPACITY) {
                small_detached_keys[small_detached_size++] = key;
            } else {
                large_detached_keys = new std::unordered_set<Key>();
                for (std::size_t i = 0; i < small_detached_size; ++i) {
                    large_detached_keys->insert(small_detached_keys[i]);
                }
                large_detached_keys->insert(key);
            }
        }

        void append_local_internal(MultiNodeV2<Key, Value> *node) {
            if (!local_head) {
                local_head = local_tail = node;
                node->set_links(my_id, nullptr, nullptr);
            } else {
                local_tail->set_links(my_id, local_tail->get_prev(my_id), node);
                node->set_links(my_id, local_tail, nullptr);
                local_tail = node;
            }
        }

        void detach_local_internal(MultiNodeV2<Key, Value> *node) {
            if (!node) return;
            MultiNodeV2<Key, Value> *p = node->get_prev(my_id);
            MultiNodeV2<Key, Value> *n = node->get_next(my_id);
            if (p) p->set_links(my_id, p->get_prev(my_id), n);
            else local_head = n;
            if (n) n->set_links(my_id, p, n->get_next(my_id));
            else local_tail = p;
            node->set_links(my_id, nullptr, nullptr);
        }

        void valuta_auto_detach() {
            if (is_pure_flat_mode || is_globally_detached || total_nodes_at_creation == 0) return;
            double alpha = static_cast<double>(get_detached_size()) / total_nodes_at_creation;
            if (alpha > 0.20) {
                detach();
                return;
            }
            double rapporto_letture = static_cast<double>(read_count) / total_nodes_at_creation;
            if (rapporto_letture > 1.0 && alpha > 0.70) { detach(); }
        }

        void dump_to_flat(std::list<std::pair<Key, Value> > &out_list,
                          std::unordered_map<Key, typename std::list<std::pair<Key, Value> >::iterator> &out_lookup) {
            if (parent) parent->dump_to_flat(out_list, out_lookup);
            MultiNodeV2<Key, Value> *current = local_head;
            while (current) {
                auto it = out_lookup.find(current->key);
                if (it != out_lookup.end()) out_list.erase(it->second);
                out_list.emplace_back(current->key, current->value);
                auto new_it = out_list.end();
                --new_it;
                out_lookup[current->key] = new_it;
                current = current->get_next(my_id);
            }
        }

    public:
        ~HierarchicalListView() {
            factory.unregister_view(my_id);
            delete large_detached_keys;
        }

        ListId get_id() const { return my_id; }

        void emplace(Key key, Value value) {
            if (is_pure_flat_mode) {
                flat_list.emplace_back(key, value);
                auto it = flat_list.end();
                --it;
                flat_lookup[key] = it;

                if (flat_lookup.size() > 2000) {
                    is_pure_flat_mode = false;
                    for (const auto &pair: flat_list) {
                        // AGGIORNATO: Sfrutta l'allocazione contigua del pool
                        MultiNodeV2<Key, Value> *new_node = factory.allocate_node(pair.first, pair.second);
                        local_lookup[pair.first] = new_node;
                        insert_detached_key(pair.first);
                        append_local_internal(new_node);
                    }
                    flat_lookup.clear();
                    flat_list.clear();
                }
                return;
            }

            // AGGIORNATO: Sfrutta l'allocazione contigua del pool
            MultiNodeV2<Key, Value> *new_node = factory.allocate_node(key, value);
            local_lookup[key] = new_node;
            insert_detached_key(key);
            append_local_internal(new_node);
            valuta_auto_detach();
        }

        // 1. Cambia la firma del metodo da std::shared_ptr a puntatore grezzo
        MultiNodeV2<Key, Value> *find_node(const Key &key) {
            ++read_count;
            if (is_key_detached(key) || is_globally_detached) {
                auto it = local_lookup.find(key);
                return it != local_lookup.end() ? it->second : nullptr; // Ritorno diretto O(1)
            }
            if (parent) return parent->find_node(key);
            return nullptr;
        }

        // 2. Di conseguenza, aggiorna find_value per leggere il puntatore grezzo
        std::pair<Value, bool> find_value(const Key &key) {
            if (is_pure_flat_mode) {
                auto it = flat_lookup.find(key);
                if (it == flat_lookup.end()) return {Value(), false};
                return {it->second->second, true};
            }

            auto *node = find_node(key); // Lettura a costo zero dal Pool
            if (node) return {node->value, true};
            return {Value(), false};
        }

        void update_value(const Key &key, Value new_value) {
            if (is_pure_flat_mode) {
                auto it = flat_lookup.find(key);
                if (it == flat_lookup.end()) return;
                flat_list.erase(it->second);
                flat_list.emplace_back(key, new_value);
                auto new_it = flat_list.end();
                --new_it;
                flat_lookup[key] = new_it;
                return;
            }

            if (is_globally_detached || is_key_detached(key)) {
                auto it = local_lookup.find(key);
                if (it != local_lookup.end()) {
                    MultiNodeV2<Key, Value> *node = it->second;
                    node->value = new_value;
                    detach_local_internal(node);
                    append_local_internal(node);
                }
            } else {
                insert_detached_key(key);
                // CORRETTO: Sostituito std::make_shared con l'allocazione ad Arena contigua
                MultiNodeV2<Key, Value> *isolated_node = factory.allocate_node(key, new_value);
                local_lookup[key] = isolated_node;
                append_local_internal(isolated_node);
            }
            valuta_auto_detach();
        }

        void move_to_tail(const Key &key) {
            if (is_pure_flat_mode) {
                auto it = flat_lookup.find(key);
                if (it == flat_lookup.end()) return;
                Value val = it->second->second;
                update_value(key, val);
                return;
            }
            if (is_globally_detached || is_key_detached(key)) {
                auto it = local_lookup.find(key);
                if (it != local_lookup.end()) {
                    MultiNodeV2<Key, Value> *node = it->second;
                    detach_local_internal(node);
                    append_local_internal(node);
                }
                return;
            }
            // CORRETTO: Cambiato parent->find_value in parent->find_node per estrarre il puntatore reale
            auto parent_node_shared = parent ? parent->find_node(key) : nullptr;
            if (!parent_node_shared) return;
            // CORRETTO: Sostituito std::make_shared con l'allocazione ad Arena contigua
            MultiNodeV2<Key, Value> *cloned_node = factory.allocate_node(key, parent_node_shared->value);
            local_lookup[key] = cloned_node;
            insert_detached_key(key);
            append_local_internal(cloned_node);
            valuta_auto_detach();
        }

        std::size_t size() const {
            if (is_pure_flat_mode) return flat_lookup.size();
            return local_lookup.size() + (parent ? parent->size() - get_detached_size() : 0);
        }

        void detach() {
            if (is_pure_flat_mode || is_globally_detached || !parent) return;
            ListId parent_id = parent->my_id;
            MultiNodeV2<Key, Value> *current = parent->local_head;
            MultiNodeV2<Key, Value> *frozen_head = nullptr;
            MultiNodeV2<Key, Value> *frozen_tail = nullptr;
            while (current) {
                if (!is_key_detached(current->key)) {
                    // CORRETTO: Sostituito std::make_shared con l'allocazione ad Arena contigua
                    MultiNodeV2<Key, Value> *cloned_node = factory.allocate_node(current->key, current->value);
                    local_lookup[current->key] = cloned_node;
                    insert_detached_key(current->key);
                    if (!frozen_head) {
                        frozen_head = frozen_tail = cloned_node;
                        cloned_node->set_links(my_id, nullptr, nullptr);
                    } else {
                        frozen_tail->set_links(my_id, frozen_tail->get_prev(my_id), cloned_node);
                        cloned_node->set_links(my_id, frozen_tail, nullptr);
                        frozen_tail = cloned_node;
                    }
                }
                current = current->get_next(parent_id);
            }
            if (frozen_head) {
                if (local_head) {
                    frozen_tail->set_links(my_id, frozen_tail->get_prev(my_id), local_head);
                    local_head->set_links(my_id, frozen_tail, local_head->get_next(my_id));
                    local_head = frozen_head;
                } else {
                    local_head = frozen_head;
                    local_tail = frozen_tail;
                }
            }
            parent = nullptr;
            is_globally_detached = true;
        }
    };


    // Ritorna direttamente l'oggetto concreto unico (Inlining totale abilitato)
    template<typename Key, typename Value>
    std::unique_ptr<HierarchicalListView<Key, Value> > ViewFactory<Key, Value>::create_root_view() {
        return std::unique_ptr<HierarchicalListView<Key, Value> >(
            new HierarchicalListView<Key, Value>(generate_id(), *this));
    }

    template<typename Key, typename Value>
    std::unique_ptr<HierarchicalListView<Key, Value> > ViewFactory<Key, Value>::create_child_view(
        HierarchicalListView<Key, Value> &parent) {
        return std::unique_ptr<HierarchicalListView<Key, Value> >(
            new HierarchicalListView<Key, Value>(generate_id(), *this, &parent));
    }

    // namespace logds::yaucl::gamma_context
}

#endif //METATENSOR_HIERARCHICALLISTVIEW_H
