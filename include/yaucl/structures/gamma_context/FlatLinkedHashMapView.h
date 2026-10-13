//
// Created by gyankos on 13/10/26.
//

#ifndef METATENSOR_FLATLINKEDHASHMAPVIEW_H
#define METATENSOR_FLATLINKEDHASHMAPVIEW_H
#include <unordered_map>
#include <list>
#include <utility>
#include "yaucl/structures/gamma_context/ILinkedHashMapView.h"

namespace logds {
    namespace yaucl {
        namespace gamma_context {

            template <typename Key, typename Value>
            class FlatLinkedHashMapView : public ILinkedHashMapView<FlatLinkedHashMapView<Key,Value>, Key, Value> {
            private:
                std::list<std::pair<Key, Value>> linear_list;
                std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> lookup;

            public:
                FlatLinkedHashMapView() = default;

                // Costruttore di clonazione profonda esplicito per il branching della Factory
                FlatLinkedHashMapView(const FlatLinkedHashMapView& other) {
                    for (const auto& pair : other.linear_list) {
                        emplace(pair.first, pair.second);
                    }
                }

                void emplace(Key key, Value value)  {
                    linear_list.emplace_back(key, value);
                    auto it = linear_list.end();
                    --it;
                    lookup[key] = it;
                }

                void update_value(const Key& key, Value new_value)  {
                    auto it = lookup.find(key);
                    if (it == lookup.end()) return;
                    linear_list.erase(it->second);
                    linear_list.emplace_back(key, new_value);
                    auto new_it = linear_list.end();
                    --new_it;
                    lookup[key] = new_it;
                }

                std::pair<Value, bool> find_value(const Key& key)  {
                    auto it = lookup.find(key);
                    if (it == lookup.end()) return {Value(), false};
                    return {it->second->second, true};
                }

                void move_to_tail(const Key& key)  {
                    auto it = lookup.find(key);
                    if (it == lookup.end()) return;
                    Value val = it->second->second;
                    update_value(key, val); // Rimuove e riappende
                }

                std::size_t size() const  { return lookup.size(); }
            };
        } // gamma_context
    } // yaucl
} // logds

#endif //METATENSOR_FLATLINKEDHASHMAPVIEW_H
