//
// Created by giacomo on 08/06/2022.
//

#ifndef KNOBAB_SERVER_NDPATTRIBUTETABLE_H
#define KNOBAB_SERVER_NDPATTRIBUTETABLE_H


#include <filesystem>
#include <string>
#include <knobab/server/query_manager/NDPFuzzyStringMatching.h>
#include <knobab/server/tables/AttributeTable.h>
#include <magic_enum.hpp>
#include <variant>

struct attribute_table_record {
    size_t act;
    size_t value;
    size_t act_table_offset;

    attribute_table_record() : act{0}, value{0}, act_table_offset{0} {}
    attribute_table_record(size_t act, size_t value, size_t actTableOffset);
    attribute_table_record(const attribute_table_record&) = default;
    attribute_table_record(attribute_table_record&&) = default;
    attribute_table_record& operator=(const attribute_table_record&) = default;
    attribute_table_record& operator=(attribute_table_record&&) = default;
    bool operator<(const attribute_table_record &rhs) const;
    bool operator>(const attribute_table_record &rhs) const;
    bool operator<=(const attribute_table_record &rhs) const;
    bool operator>=(const attribute_table_record &rhs) const;
};

struct NDPAttributeTable {
    std::string attr_name;
    NDPFuzzyStringMatching ptr;
    AttributeTableType type;
    std::filesystem::path p;
    FixedSizeReaderWriter<attribute_table_record> table;
    FixedSizeReaderWriter<std::pair<size_t, size_t>> primary_act_index;
    FixedSizeReaderWriter<attribute_table_record> primary_act_value_index;
    bool isRead, isWrite;

    struct comparator {
        NDPAttributeTable& reference;
        comparator(NDPAttributeTable& reference);
        bool operator()(const attribute_table_record &lhs, const attribute_table_record &rhs) const;
    };

    size_t storeLoad(const std::variant<double, size_t, long long int, std::string, bool> &x);
    union_type resolve(const attribute_table_record &x) const;

    void prepareRead(bool doTestLoadOtherwise) {
        if (!isRead) {
            {
                if (!doTestLoadOtherwise) {
                    attr_name = p.filename();
                } else if (p.filename() != attr_name) {
                    std::cerr << "ERROR: folder name does not match with the initialization filename!"<< std::endl;
                    exit(1);
                }
            }

            // Ensuring that the table has the same type to the one that has been stored before
            std::ifstream file{p / "type.txt"};
            {
                std::string string_type;
                file >> string_type;
                auto val = magic_enum::enum_cast<AttributeTableType>(string_type);
                if (!doTestLoadOtherwise) {
                    if (!val.has_value()) {
                        std::cerr << "ERROR: no expected stored type was available!"<< std::endl;
                        exit(1);
                    }
                    if (val.value() != type) {
                        std::cerr << "ERROR: stored type (" << magic_enum::enum_name(type) << " differs from the stored type: " << string_type << std::endl;
                        exit(1);
                    }
                } else {
                    type = val.value();
                }
            }
            isRead = true;
        }
    }
    void prepareWrite() {
        if (!isWrite) {
            // Writing the type to the data
            {
                std::ofstream file{p / "type.txt"};
                file << magic_enum::enum_name(type);
            }
        }

    }

    /**
     * Initializing the table with the purpose of writing the data, or initializing the table.
     * For this reason, I need to provide the path where the database is held, alongside the attribute
     * name and the types that such a table is going to store.
     *
     * @param p
     * @param attr
     * @param type
     */
    NDPAttributeTable(const std::filesystem::path& p, const std::string &attr, AttributeTableType type);

    /**
     * This table has the aim of predominantly load the data that was aleady written in secondary memory
     * @param p
     * @param attr
     */
    NDPAttributeTable(const std::filesystem::path& p, const std::string &attr);
    NDPAttributeTable(const NDPAttributeTable&) = default;
    NDPAttributeTable(NDPAttributeTable&&) = default;
    NDPAttributeTable& operator=(const NDPAttributeTable&) = default;
    NDPAttributeTable& operator=(NDPAttributeTable&&) = default;

    bool assertVariant(const std::variant<double, size_t, long long int, std::string, bool> &val);
    void record_load(size_t act_id, const union_type &val, size_t totalEventId);
    void index();
};


#endif //KNOBAB_SERVER_NDPATTRIBUTETABLE_H
