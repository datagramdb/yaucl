//
// Created by giacomo on 15/12/22.
//

#ifndef AIRLINE_BLOCKOFRECORDSINDEXEDTABLE_H
#define AIRLINE_BLOCKOFRECORDSINDEXEDTABLE_H

#include <vector>
#include <map>
#include <yaucl/structures/any_to_uint_bimap.h>

namespace yaucl::structures {
    /**
     * This data structure ensures that exact same values are only persisted at most once,
     * while they might be associated to several different index values. For this, we are
     * using the block of records strategy, where unique values are just referenced by
     * their unique instance id associated to the given table
     *
     *
     * @tparam Key          Type to be associated to the index value
     * @tparam Value        Type to be associated to the value to be indexed
     */
    template<typename Key, typename Value> struct BlockOfRecordsIndexedTable {
        any_to_uint_bimap<Value> unique_values;
        std::vector<std::vector<Key>> value_id_to_associated_indices;
        std::map<Key, std::vector<size_t>> key_to_values;

        /**
         * Associating a value to a given index, while gua
         * @param key       index value to be associated to the current value
         * @param value     Value to be associated to an index
         * @return          A pair, where the first component returns the unique value
         *                  associated to the value, while the second component returns
         *                  whether this specific value was insterted for the first time,
         *                  indepenently from the index value/key that was currently
         *                  associated to.
         */
        std::pair<size_t, bool> put(const Key& key, const Value& value) {
            std::vector<size_t>& values = unique_values[key];
            std::pair<size_t, bool> cp{unique_values.put(value)};
            if (cp.second) {
                auto& ref = value_id_to_associated_indices.emplace_back();
                ref.emplace_back(key);
                values.emplace_back(cp.first);
            } else {
                if (std::lower_bound(values.begin(), values.end(),cp.first) == values.end()) {
                    values.emplace_back(cp.first);
                    value_id_to_associated_indices[cp.first].emplace_back(key);
                }
            }
            return cp;
        }

        ssize_t valueId(const Value& val) const {
            return unique_values.signed_get(val);
        }

        const Value& getValueFromId(size_t id) const {
            return unique_values.get(id);
        }

        typename std::map<Key, std::vector<size_t>>::iterator noValueForKeys() const {
            return key_to_values.end();
        }

        typename std::map<Key, std::vector<size_t>>::iterator valuesIdForKeys(const Key& k) const {
            return key_to_values.find(k);
        }

        const std::vector<Key>& valueIdToAssociatedKeys(size_t id) const {
            return value_id_to_associated_indices.at(id);
        }
    };
}

#endif //AIRLINE_BLOCKOFRECORDSINDEXEDTABLE_H
