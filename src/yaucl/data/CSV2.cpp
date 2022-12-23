//
// Created by giacomo on 23/12/22.
//

#include <yaucl/data/CSV2.h>
#include <regex>

CSV2::CSV2(const std::string &path) : document{path} {
    std::regex rgx("(.*)\\:<([^>]*)>");
    std::smatch matches;
    size_t counter = 0;
    actual_name_with_is_string_oth_double.reserve(document.GetColumnCount());
    for (const std::string& colName : document.GetColumnNames()) {
        if((std::regex_search(colName, matches, rgx)) && (matches.size() == 3)) {
            std::string actualColName = matches[1].str();
            std::string colType = matches[2].str();
            bool isString = false;
            auto it = actual_name_to_pos.find(actualColName);
            if (it != actual_name_to_pos.end())
                throw std::runtime_error(actualColName+": error, aleady existing at position "+std::to_string(it->second));
            if (colType == "number") {
                isString = false;
            } else if (colType == "string") {
                isString = true;
            } else {
                throw std::runtime_error(colType+": error, this should be either `number` or `string`");
            }
            actual_name_with_is_string_oth_double.emplace_back(actualColName, isString);
            actual_name_to_pos[actualColName] = counter;
        } else {
            throw std::runtime_error("ERROR: each column should satisfy the regex `^(.*)\\:<([^>]*)>$`");
        }
        counter++;
    }

}

std::vector<std::string> CSV2::getStringColumn(const std::string &actual_name) const {
    auto it = actual_name_to_pos.find(actual_name);
    if ((it != actual_name_to_pos.end()) && (actual_name_with_is_string_oth_double.at(it->second).second)) {
        document.GetColumn<std::string>(it->second);
    }
    return {};
}

std::vector<double> CSV2::getDoubleColumn(const std::string &actual_name) const {
    auto it = actual_name_to_pos.find(actual_name);
    if ((it != actual_name_to_pos.end()) && (actual_name_with_is_string_oth_double.at(it->second).second)) {
        document.GetColumn<double>(it->second);
    }
    return {};
}

size_t CSV2::getNColumns() const { return document.GetColumnCount(); }

size_t CSV2::getNRows() const { return document.GetRowCount(); }

std::unordered_map<std::string, std::variant<std::string, double>> CSV2::getRow(size_t id) const {
    std::unordered_map<std::string, std::variant<std::string, double>> map;
    size_t N = getNColumns();
    if (id < N) {
        for (size_t i=0; i<N; i++) {
            auto& ref = actual_name_with_is_string_oth_double.at(i);
            if (ref.second) {
                map[ref.first] = document.GetCell<std::string>(i, id);
            } else {
                map[ref.first] = document.GetCell<double>(i, id);
            }
        }
    }
    return map;
}

std::string CSV2::getColumnHeader(size_t i) const {
    if (i < getNColumns())
        return actual_name_with_is_string_oth_double.at(i).first;
    else
        return "";
}

bool CSV2::isColumnNumeric(size_t i) const {
    if (i< getNColumns())
        return false;
    else
        return !actual_name_with_is_string_oth_double.at(i).second;
}

bool CSV2::isColumnString(size_t i) const {
    if (i< getNColumns())
        return false;
    else
        return actual_name_with_is_string_oth_double.at(i).second;
}
