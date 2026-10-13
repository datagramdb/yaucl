//
// Created by gyankos on 13/10/26.
//

#ifndef METATENSOR_ILINKEDHASHMAPVIEW_H
#define METATENSOR_ILINKEDHASHMAPVIEW_H


#include <memory>
#include <yaucl/structures/gamma_context/MultiNode.h>

namespace logds::yaucl::gamma_context {
    // Interfaccia basata su CRTP (Polimorfismo statico a costo zero)
    template <typename Derived, typename Key, typename Value>
    class ILinkedHashMapView {
    public:
        void emplace(Key key, Value value) {
            static_cast<Derived*>(this)->emplace(std::move(key), std::move(value));
        }
        void update_value(const Key& key, Value new_value) {
            static_cast<Derived*>(this)->update_value(key, std::move(new_value));
        }
        std::pair<Value, bool> find_value(const Key& key) {
            return static_cast<Derived*>(this)->find_value(key);
        }
        void move_to_tail(const Key& key) {
            static_cast<Derived*>(this)->move_to_tail(key);
        }
        std::size_t size() const {
            return static_cast<const Derived*>(this)->size();
        }
    };
}
#endif //METATENSOR_ILINKEDHASHMAPVIEW_H
