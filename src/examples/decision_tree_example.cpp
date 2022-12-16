/*
 * main.cpp
 * This file is part of yaucl-elarning
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl-elarning is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl-elarning is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl-elarning. If not, see <http://www.gnu.org/licenses/>.
 */


#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include "../submodules/csv.h"
#include <yaucl/learning/DecisionTree.h>

struct record {
    std::string outlook, tnominal, hnominal, windy;
    double temperature, humidity;
    record() = default;
    record(const record&) = default;
    record(record&&) = default;
    record& operator=(const record&) = default;
    record& operator=(record&&) = default;
};

namespace std {
    template <>
    struct hash<record> {
        std::hash<std::string> hT;
        std::hash<double> hK;
        std::size_t operator()(const record& k) const {
            size_t val = hT(k.outlook) ^ hT(k.tnominal) ^ hT(k.hnominal);
            if (k.windy == "yes") val*=2;
            return val;
        }
    };

}

int main()
{
    io::CSVReader<7> in("outlook.csv");
    in.read_header(io::ignore_extra_column, "outlook","temperature","tnominal","humidity","hnominal","windy","play");
    std::pair<record, int> obj;
    std::vector<std::pair<record,int>> V;
    while(in.read_row(obj.first.outlook, obj.first.temperature, obj.first.tnominal, obj.first.humidity, obj.first.hnominal, obj.first.windy, obj.second)){
        V.emplace_back(obj);
    }

    std::function<simple_data(const record&, const std::string&)> selector = [](const record& x, const std::string& key) -> simple_data {
        if (key == "outlook")
            return x.outlook;
        else if (key == "temperature")
            return x.temperature;
        else if (key == "tnominal")
            return x.tnominal;
        else if (key == "humidity")
            return x.humidity;
        else if (key == "hnominal")
            return x.hnominal;
        else if (key == "windy")
            return x.windy;
        else
            return 0.0;
    };

    auto it = V.begin(), en = V.end();
    DecisionTree<record> dt(it,
                            en,
                            1,
                            selector,
                            std::unordered_set<std::string>{"temperature","humidity"},
                            std::unordered_set<std::string>{"outlook","tnominal","hnominal","windy"},
                            ForTheWin::gain_measures::Entropy,
                            0.9,
                            1,
                            1);
    dt.print_rec(std::cout, 1);
}

