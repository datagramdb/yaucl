//
// Created by giacomo on 23/12/22.
//

#ifndef AIRLINE_CSV2_H
#define AIRLINE_CSV2_H

#include <rapidcsv.h>
#include <variant>

class CSV2 {
    rapidcsv::Document document;
    std::vector<std::pair<std::string, bool>> actual_name_with_is_string_oth_double;
    std::unordered_map<std::string, size_t>   actual_name_to_pos;

public:
    using row = std::unordered_map<std::string, std::variant<std::string, double>>;
    CSV2(const std::string& path);
    std::string getColumnHeader(size_t i) const;
    bool isColumnNumeric(size_t i) const;
    bool isColumnString(size_t i) const;
    bool isColumnNumeric(const std::string& name) const;
    bool isColumnString(const std::string& name) const;
    std::vector<std::string> getStringColumn(const std::string& actual_name) const;
    std::vector<double> getDoubleColumn(const std::string& actual_name) const;
    size_t getNColumns() const;
    size_t getNRows() const;
    row getRow(size_t id) const;
};


#endif //AIRLINE_CSV2_H
