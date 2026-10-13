//
// Created by gyankos on 13/10/26.
//

#ifndef METATENSOR_VIEWFACTORY_H
#define METATENSOR_VIEWFACTORY_H

#include <memory>
#include <vector>
#include <cstddef>
#include <variant>
#include <yaucl/structures/gamma_context/MultiNode.h>

namespace logds::yaucl::gamma_context {


    template <typename Key, typename Value> class HierarchicalListView;

    template <typename Key, typename Value>
    class ViewFactory {
        ListId id_counter{0};

        // --- OPTIMIZATION: ARENA MEMORY POOL ---
        // Dimensione del blocco contiguo pre-allocato (es. 4096 nodi per blocco)
        static constexpr std::size_t BLOCK_SIZE = 4096;

        struct ArenaBlock {
            MultiNodeV2<Key, Value> storage[BLOCK_SIZE];
            std::size_t used = 0;
        };

        std::vector<std::unique_ptr<ArenaBlock>> arena;

        // Crea un nuovo blocco contiguo se l'attuale è pieno
        void allocate_new_block() {
            arena.push_back(std::make_unique<ArenaBlock>());
        }

    public:
        ViewFactory() {
            allocate_new_block(); // Pre-alloca il primo blocco contiguo
        }

        ViewFactory(const ViewFactory&) = delete;
        ViewFactory& operator=(const ViewFactory&) = delete;

        // BUMP ALLOCATION: Costo O(1) reale e memoria 100% contigua
        template <typename... Args>
        MultiNodeV2<Key, Value>* allocate_node(Args&&... args) {
            if (arena.back()->used >= BLOCK_SIZE) {
                allocate_new_block();
            }

            ArenaBlock* current_block = arena.back().get();
            // Sfrutta il placement new per posizionare l'oggetto nella memoria contigua dell'arena
            MultiNodeV2<Key, Value>* node = &current_block->storage[current_block->used++];
            new (node) MultiNodeV2<Key, Value>(std::forward<Args>(args)...);
            return node;
        }

        // Unregister veloce: non deve deallocare, pulisce solo i link logici
        void unregister_view(ListId id) {
            // Ottimizzazione: non facciamo Pointer Chasing alla morte della vista.
            // I link obsoleti verranno ignorati dalle altre viste e la memoria dell'intera
            // Arena verrà rilasciata in un colpo solo alla distruzione della Factory.
            // for (auto& block : arena) {
            //     for (std::size_t i = 0; i < block->used; ++i) {
            //         block->storage[i].erase_list_id(id);
            //     }
            // }
        }

        ListId generate_id() { return ++id_counter; }


        std::unique_ptr<HierarchicalListView<Key, Value>> create_root_view();
        std::unique_ptr<HierarchicalListView<Key, Value>> create_child_view(HierarchicalListView<Key, Value>& parent);
    };

}

#endif //METATENSOR_VIEWFACTORY_H
