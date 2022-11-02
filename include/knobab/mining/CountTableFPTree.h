//
// Created by giacomo on 15/09/22.
//

#ifndef KNOBAB_COUNTTABLEFPTREE_H
#define KNOBAB_COUNTTABLEFPTREE_H

#ifndef TUTORIALS_FPTREE_H
#define TUTORIALS_FPTREE_H

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include "knobab/server/tables/CountTemplate.h"

#define ForAll(T)       template <typename T>

using Transaction           = std::vector<act_t>;
using TransformedPrefixPath = std::pair<std::vector<act_t>, uint64_t>;
using Pattern               = std::pair<std::set<act_t>, uint64_t>;
ForAll(T) using GenPattern  = std::pair<std::set<T>, uint64_t>;

ForAll(Item) struct FPNode {
    const Item                           item;
    uint64_t                             frequency;
    std::shared_ptr<FPNode<Item>>        node_link;
    std::weak_ptr<FPNode<Item>>          parent;
    std::vector<std::shared_ptr<FPNode>> children;

    FPNode(const Item& item, const std::shared_ptr<FPNode<Item>>& parent) :
            item( item ), frequency( 1 ), node_link( nullptr ), parent( parent ), children() {
    }
};



struct FPTree {
    std::shared_ptr<FPNode<act_t>>                 root;
//    std::map<act_t, std::shared_ptr<FPNode<act_t>>> header_table;
    std::vector<std::shared_ptr<FPNode<act_t>>> header_table;
    uint64_t                                      minimum_support_threshold;
    size_t max_act_id;

    // order items by decreasing frequency
    struct frequency_comparator
    {
        bool operator()(const std::pair<size_t, uint64_t> &lhs, const std::pair<size_t, uint64_t> &rhs) const
        {
            return std::tie(lhs.second, lhs.first) > std::tie(rhs.second, rhs.first);
        }
    };


    FPTree(const std::vector<Transaction>& transactions,
           uint64_t minimum_support_threshold,
           uint64_t max_act_id) :
            root(std::make_shared<FPNode<act_t>>( 0, nullptr )), header_table(max_act_id, nullptr), max_act_id(max_act_id),
            minimum_support_threshold( minimum_support_threshold ) {
        // scan the transactions counting the frequency of each item
        std::map<act_t, uint64_t> frequency_by_item;
        for ( const Transaction& transaction : transactions ) {
            for ( const auto& item : transaction ) {
                ++frequency_by_item[item];
            }
        }

        // keep only items which have a frequency greater or equal than the minimum support threshold
        for ( auto it = frequency_by_item.cbegin(); it != frequency_by_item.cend(); ) {
            const uint64_t item_frequency = (*it).second;
            if ( item_frequency < minimum_support_threshold ) { frequency_by_item.erase( it++ ); }
            else { ++it; }
        }


        std::set<std::pair<act_t , uint64_t>, frequency_comparator> items_ordered_by_frequency(frequency_by_item.cbegin(), frequency_by_item.cend());

        // start tree construction

        // scan the transactions again
        for ( const Transaction& transaction : transactions ) {
            auto curr_fpnode = root;

            // select and sort the frequent items in transaction according to the order of items_ordered_by_frequency
            for ( const auto& pair : items_ordered_by_frequency ) {
                const auto& item = pair.first;

                // check if item is contained in the current transaction
                if ( std::find( transaction.cbegin(), transaction.cend(), item ) != transaction.cend() ) {
                    // insert item in the tree

                    // check if curr_fpnode has a child curr_fpnode_child such that curr_fpnode_child.item = item
                    const auto it = std::find_if(
                            curr_fpnode->children.cbegin(), curr_fpnode->children.cend(),  [item](const std::shared_ptr<FPNode<act_t>>& fpnode) {
                                return fpnode->item == item;
                            } );
                    if ( it == curr_fpnode->children.cend() ) {
                        // the child doesn't exist, create a new node
                        const auto curr_fpnode_new_child = std::make_shared<FPNode<act_t>>( item, curr_fpnode );

                        // add the new node to the tree
                        curr_fpnode->children.push_back( curr_fpnode_new_child );

                        // update the node-link structure
                        if ( header_table.at( curr_fpnode_new_child->item )  != nullptr ) {
                            auto prev_fpnode = header_table[curr_fpnode_new_child->item];
                            while ( prev_fpnode->node_link ) { prev_fpnode = prev_fpnode->node_link; }
                            prev_fpnode->node_link = curr_fpnode_new_child;
                        }
                        else {
                            header_table[curr_fpnode_new_child->item] = curr_fpnode_new_child;
                        }

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_new_child;
                    }
                    else {
                        // the child exist, increment its frequency
                        auto curr_fpnode_child = *it;
                        ++curr_fpnode_child->frequency;

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_child;
                    }
                }
            }
        }
    }

    FPTree(const CountTemplate& transactions,
           uint64_t minimum_support_threshold,
           uint64_t max_act_id) :
            root(std::make_shared<FPNode<act_t>>( 0, nullptr )),
            header_table(max_act_id, nullptr),
            max_act_id(max_act_id),
            minimum_support_threshold( minimum_support_threshold ) {
        // scan the transactions counting the frequency of each item

        std::map<act_t, uint64_t> frequency_by_item;
        for ( const auto& transaction_item : transactions.table ) {
            if (transaction_item.id.parts.event_id > 0) {
                ++frequency_by_item[transaction_item.id.parts.act];
            }
        }

        // keep only items which have a frequency greater or equal than the minimum support threshold
        for ( auto it = frequency_by_item.cbegin(); it != frequency_by_item.cend(); ) {
            const uint64_t item_frequency = (*it).second;
            if ( item_frequency < minimum_support_threshold ) { frequency_by_item.erase( it++ ); }
            else { ++it; }
        }


        std::set<std::pair<act_t, uint64_t>, frequency_comparator> items_ordered_by_frequency(frequency_by_item.cbegin(), frequency_by_item.cend());

        // start tree construction

        // scan the transactions again
        for (uint32_t transaction_id = 0, N = transactions.nTraces(); transaction_id<N; transaction_id++ ) {
            auto curr_fpnode = root;

            // select and sort the frequent items in transaction according to the order of items_ordered_by_frequency
            for ( const auto& pair : items_ordered_by_frequency ) {
                const act_t& item = pair.first;

                // check if item is contained in the current transaction
                if (transactions.resolve_length(item, transaction_id) > 0) {
                    // insert item in the tree

                    // check if curr_fpnode has a child curr_fpnode_child such that curr_fpnode_child.item = item
                    const auto it = std::find_if(
                            curr_fpnode->children.cbegin(), curr_fpnode->children.cend(),  [item](const std::shared_ptr<FPNode<act_t>>& fpnode) {
                                return fpnode->item == item;
                            } );
                    if ( it == curr_fpnode->children.cend() ) {
                        // the child doesn't exist, create a new node
                        const auto curr_fpnode_new_child = std::make_shared<FPNode<act_t>>( item, curr_fpnode );

                        // add the new node to the tree
                        curr_fpnode->children.push_back( curr_fpnode_new_child );

                        // update the node-link structure
                        if ( header_table.at( curr_fpnode_new_child->item ) != nullptr ) {
                            auto prev_fpnode = header_table[curr_fpnode_new_child->item];
                            while ( prev_fpnode->node_link ) { prev_fpnode = prev_fpnode->node_link; }
                            prev_fpnode->node_link = curr_fpnode_new_child;
                        }
                        else {
                            header_table[curr_fpnode_new_child->item] = curr_fpnode_new_child;
                        }

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_new_child;
                    }
                    else {
                        // the child exist, increment its frequency
                        auto curr_fpnode_child = *it;
                        ++curr_fpnode_child->frequency;

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_child;
                    }
                }
            }
        }
    }

    bool empty() const {
        assert( root );
        return root->children.size() == 0;
    }
};

ForAll(Item) bool contains_single_path(const std::shared_ptr<FPNode<Item>>& fpnode) {
    assert( fpnode );
    if ( fpnode->children.size() == 0 ) { return true; }
    if ( fpnode->children.size() > 1 ) { return false; }
    return contains_single_path( fpnode->children.front() );
}



std::set<Pattern> fptree_growth(const FPTree& fptree, size_t max_depth = 2);

#endif  // FPTREE_HPP


#endif //KNOBAB_COUNTTABLEFPTREE_H
