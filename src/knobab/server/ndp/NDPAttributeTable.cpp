//
// Created by giacomo on 08/06/2022.
//

#include "yaucl/data/mmapFile.h"
#include "knobab/server/ndp/NDPAttributeTable.h"


union_minimal resolveUnionMinimal(const NDPAttributeTable &table, const attribute_table_record &x) {
    switch (table.type) {
        case DoubleAtt:
            return *(double*)(&x.value);
        case LongAtt:
            return (double)(*(long long*)(&x.value));
        case StringAtt:
            return table.ptr.get(x.value);
        case BoolAtt:
            return (x.value != 0 ? 0.0 : 1.0);
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return (double)x.value;
    }
}

NDPAttributeTable::NDPAttributeTable(const std::filesystem::path &p, const std::string &attr, AttributeTableType type)
        : attr_name(attr), type{type}, p{p/ "attributes" / attr},
          isRead{false}, isWrite{false}, table{p/ "attributes" / attr / "table.bin", std::filesystem::temp_directory_path()} {
    if (! std::filesystem::exists(this->p)) {
        std::filesystem::create_directories(this->p);
        if (type == StringAtt)
            ptr.open(p / "attributes" / attr);
        prepareWrite();
    } else {
        if (type == StringAtt)
            ptr.open(p / "attributes" / attr);
        prepareRead(true);
    }
}

size_t NDPAttributeTable::value_to_storage_format(const std::variant<double, size_t, long long int, std::string, bool> &x) {
    switch (type) {
        case DoubleAtt: {
            double tmp = std::get<double>(x);
            return *(size_t*)(&tmp);
        }

        case LongAtt: {
            long long tmp = std::get<long long>(x);
            return *(size_t*)(&tmp);
        }

        case StringAtt: {
            std::string tmp = std::get<std::string>(x);
            return ptr.put(tmp).first;
        }
        case BoolAtt:
            return std::get<bool>(x) ? 1 : 0;
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return std::get<size_t>(x);
    }
}

bool NDPAttributeTable::assertVariant(const std::variant<double, size_t, long long int, std::string, bool> &val) {
    switch (type) {
        case DoubleAtt:
            return std::holds_alternative<double>(val);
        case SizeTAtt:
            return std::holds_alternative<size_t>(val);
        case LongAtt:
            return std::holds_alternative<long long>(val);
        case StringAtt:
            return std::holds_alternative<std::string>(val);
        case BoolAtt:
            return std::holds_alternative<bool>(val);
        default:
            return false;
    }
}

union_type NDPAttributeTable::resolve(const attribute_table_record &x) const {
    switch (type) {
        case DoubleAtt:
            return *(double*)(&x.value);
        case LongAtt:
            return *(long long*)(&x.value);
        case StringAtt:
            return ptr.get(x.value);
        case BoolAtt:
            return x.value != 0;
            //case SizeTAtt:
        default:
            // TODO: hierarchical types!, https://dl.acm.org/doi/10.1145/3410566.3410583
            return x.value;
    }
}

void NDPAttributeTable::record_load(size_t act_id, const union_type &val, size_t totalEventId) {
    prepareRead(true);
    static attribute_table_record record;
    DEBUG_ASSERT(assertVariant(val));
    record.act = act_id;
    record.value = value_to_storage_format(val);
    record.act_table_offset = totalEventId;
    table.put(record);
}

void NDPAttributeTable::index(const std::vector<size_t>& total_event_to_offset) {
    comparator comp{*this};
    table.sort<comparator>(availableMemory()/4, comp); // Sorting the tables

    primary_act_index.open(p / "table.bin_primaryAct", std::filesystem::temp_directory_path());
    primary_act_value_index.open(p / "table.bin_primaryActValue", std::filesystem::temp_directory_path());
    std::pair<size_t, size_t> actOffset;
    attribute_table_record actValueOffset;
    // TODO: even better, use a clustered index! This will reduce the
    // scan time over duplicated data!
    for (size_t i = 0, N = table.size(); i<N; i++) {
        if (i == 0) {
            auto& curr = table.get(i);
            // In order to avoid accessing multiple files in memory
            // mapping, we might change the third component so to
            // directly jump to the offset where the event is stored
            // in the act table
            curr.act_table_offset = total_event_to_offset.at(i);
            actOffset = {curr.act, 0};
            actValueOffset = {curr.act, curr.value, 0};
        } else {
            auto& curr = table.get(i);
            // In order to avoid accessing multiple files in memory
            // mapping, we might change the third component so to
            // directly jump to the offset where the event is stored
            // in the act table
            curr.act_table_offset = total_event_to_offset.at(i);
            if (curr.act != actOffset.first) {
                primary_act_index.put(actOffset);
                actOffset = {curr.act, i};
                primary_act_value_index.put(actValueOffset);
                actValueOffset.act = curr.act;
                actValueOffset.value = curr.value;
                actValueOffset.act_table_offset = i;
            } else if (curr.value != actValueOffset.value) {
                primary_act_value_index.put(actValueOffset);
                actValueOffset.value = curr.value;
                actValueOffset.act_table_offset = i;
            }
        }
    }
    primary_act_index.put(actOffset);
    primary_act_value_index.put(actValueOffset);
    primary_act_index.close();
    primary_act_value_index.close();

}

NDPAttributeTable::NDPAttributeTable(const std::filesystem::path &p, const std::string &attr) : attr_name(attr), ptr{p / "attributes" / attr}, p{p/ "attributes" / attr},
                                                                                                isRead{false}, isWrite{false}, table{p/ "attributes" / attr / "table.bin", std::filesystem::temp_directory_path()} { prepareRead(false); }

attribute_table_record *NDPAttributeTable::resolve_record_if_exists(size_t i) {
    return table.size() >= i ? nullptr : &table.get(i);
}

std::optional<attribute_table_record> NDPAttributeTable::resolve_record_if_exists2(size_t actTableOffset) {
    auto ptr = resolve_record_if_exists(actTableOffset);
    if (!ptr) return {};
    else return {*ptr};
}

//////////////////////// attribute_table_record

attribute_table_record::attribute_table_record(size_t act, size_t value, size_t actTableOffset) : act(act), value(value),
                                                                  act_table_offset(actTableOffset) {}

bool attribute_table_record::operator<(const attribute_table_record &rhs) const {
    if (act < rhs.act)
        return true;
    if (rhs.act < act)
        return false;
    if (value < rhs.value)
        return true;
    if (rhs.value < value)
        return false;
    return act_table_offset < rhs.act_table_offset;
}

bool attribute_table_record::operator>(const attribute_table_record &rhs) const {
    return rhs < *this;
}

bool attribute_table_record::operator<=(const attribute_table_record &rhs) const {
    return !(rhs < *this);
}

bool attribute_table_record::operator>=(const attribute_table_record &rhs) const {
    return !(*this < rhs);
}

//////////////////////// Comparator

NDPAttributeTable::comparator::comparator(NDPAttributeTable &reference) : reference{reference} {}
bool
NDPAttributeTable::comparator::operator()(const attribute_table_record &lhs, const attribute_table_record &rhs) const {
    if (lhs.act < rhs.act)
        return true;
    if (lhs.act > rhs.act)
        return false;
    auto L = reference.resolve(lhs);
    auto R = reference.resolve(rhs);
    if (L < R)
        return true;
    if (L > R)
        return false;
    return lhs.act_table_offset < rhs.act_table_offset;
}
