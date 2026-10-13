//
// Created by gyankos on 13/10/26.
//

#include "yaucl/structures/gamma_context/ViewFactory.h"
#include "yaucl/structures/gamma_context/FlatLinkedHashMapView.h"
#include "yaucl/structures/gamma_context/HierarchicalListView.h"

#ifndef METATENSOR_GAMMA_CONTEXT_H
#define METATENSOR_GAMMA_CONTEXT_H

// namespace logds::yaucl::gamma_context {
//     template <typename Key, typename Value>
// GammaViewVariant<Key, Value> ViewFactory<Key, Value>::create_root_view(std::size_t stimati) {
//         if (stimati <= HARDWARE_BREAKEVEN_THRESHOLD) {
//             return FlatLinkedHashMapView<Key, Value>();
//         }
//         return HierarchicalListView<Key, Value>(generate_id(), *this);
//     }
//
//     template <typename Key, typename Value>
//     GammaViewVariant<Key, Value> ViewFactory<Key, Value>::create_child_view(GammaViewVariant<Key, Value>& parent) {
//         std::size_t parent_size = std::visit([](const auto& view) { return view.size(); }, parent);
//
//         if (parent_size <= HARDWARE_BREAKEVEN_THRESHOLD) {
//             if (auto* flat_parent = std::get_if<FlatLinkedHashMapView<Key, Value>>(&parent)) {
//                 return FlatLinkedHashMapView<Key, Value>(*flat_parent);
//             }
//         }
//
//         if (auto* lazy_parent = std::get_if<HierarchicalListView<Key, Value>>(&parent)) {
//             return HierarchicalListView<Key, Value>(generate_id(), *this, lazy_parent);
//         }
//
//         return FlatLinkedHashMapView<Key, Value>();
//     }
// };

#endif //METATENSOR_GAMMA_CONTEXT_H
