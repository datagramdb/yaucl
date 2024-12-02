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

#if 0

#include <yaucl/structures/RelationalTables.h>
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

void weather_outlook_example()  {
        io::CSVReader<7> in("outlook.csv");
    in.read_header(io::ignore_extra_column, "outlook","temperature","tnominal","humidity","hnominal","windy","play");
    std::pair<record, int> obj;
    std::vector<std::pair<record,int>> V;
    while(in.read_row(obj.first.outlook, obj.first.temperature, obj.first.tnominal, obj.first.humidity, obj.first.hnominal, obj.first.windy, obj.second)){
        V.emplace_back(obj);
    }
}


void doubles_example() {
    auto selector = [](double x, const std::string& key) -> double {
        return x;
    };
    std::vector<std::pair<double,int>> V{{2.0,0}, {5.0,0}, {-2.0,1}, {-8.0,1}};
    auto it = V.begin(), en = V.end();
    DecisionTree<double> dt(it,
                            en,
                            1,
                            selector,
                            std::unordered_set<std::string>{"y"},
                            std::unordered_set<std::string>{},
                            ForTheWin::gain_measures::Entropy,
                            0.9,
                            1,
                            1);
}


int main()
{
//    weather_outlook_example();
    doubles_example();
}

#endif




#include <yaucl/learning/DecisionTree.h>

int main() {
//    std::string file = "/home/giacomo/Scaricati/twigy-master/test/testdata/titanic_data.csv";
    std::string file = "/home/giacomo/Scaricati/convertcsv.csv";
    // Read the example data and split it into features and labels
    table data_frame = ReadCsv(file);
    std::vector<int> y;
    std::string clazz = "L";
    std::vector<std::vector<std::pair<std::string,union_minimal>>> X;
    Split_X_y(data_frame, X, y, clazz);
    auto h = data_frame.headers_;
    h.erase(std::remove(h.begin(), h.end(), clazz), h.end());
//    h.erase(std::remove(h.begin(), h.end(), "Labelo"), h.end());
    std::unordered_set<std::string> N{h.begin(), h.end()};
//    std::unordered_set<std::string> cat{"Labelo"};
//    DataRepo dr{X, y};
//
//    optimize::Matrix subFeatures_X;
//    optimize::Vector Y;
//    int maxClass = dr.max_class_id;
//
//    dr.init_matrix(h, subFeatures_X, Y);
////    Eigen::MatrixXd X = self.X; // Assuming self.X is an Eigen matrix
////    Eigen::MatrixXd subFeatures_X = X(self.sample_ids.col(0), subset_feature_ids.transpose());
////    Eigen::VectorXd Y = self.Y(self.sample_ids);
////    int class_num = self.class_num;
//    Eigen::VectorXd initial_a = Eigen::VectorXd::Random(h.size() + 1).array() - 0.5;
//
//    // With C++17 or greater we can omit the "<>"
//    WODT f{subFeatures_X, Y, maxClass};
//    optimize::Lbfgsb<> solver;                            // Solver using default stopping conditions
////    solver.setFunctionEvalsMax(10);
//    optimize::State state = solver.minimize(f, initial_a);  // Solve with constraints (solution is f = 7.75 at x = [0.5, 0.5, 0.35])
//
//
//    std::cout << "f = " << state.f() << std::endl;             // Minimum of f(x)
//    std::cout << "x = " << state.x().transpose() << std::endl; // Argmin x of f(x)
//
//    auto a = state.x();
//    Eigen::VectorXd paras = a.tail(a.size() - 1);
//    auto weight = a.coeffRef(0);
//
//    std::unordered_map<std::string,double> w;
//    for (size_t idx = 0, N = paras.size(); idx<N; idx++) {
//        w[h.at(idx)] = paras(idx);
//    }
//    auto result = ((subFeatures_X * paras).array() -a.coeffRef(0) <= 0.0);
//    ForTheWin forthegain;
//    double forthepos = 0.0, fortheneg = 0.0;
//    std::vector<std::pair<double,double>> results(maxClass+1, {0,0});
//    dt_predicate prepad(w, weight);
//    for (size_t i = 0, N = result.size(); i<N; i++) {
//        if (result(i)) {
////            if (!prepad(dr.records[dr.offsets[i]]))
////                exit(200);
//            forthepos++;
//            results[Y(i)].first++;
//        } else {
////            if (prepad(dr.records[dr.offsets[i]]))
////                exit(300);
//            fortheneg++;
//            results[Y(i)].second++;
//        }
//    }
//    forthegain.init(maxClass);
//    forthegain.goodBad(forthepos, fortheneg);
//    for (size_t clazz = 0; clazz<=maxClass; clazz++ ){
//        forthegain.setP(clazz, results[clazz].first, results[clazz].second);
//    }
//
//    std::cout << forthegain.getGain(ForTheWin::gain_measures::Gini) << std::endl;

    // preview of the data
//    data_frame.Print(10);
//
    DecisionTree DT{X,y, N, {}, 5};
    DT.splitTree();
//
    std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> results;
    DT.populate_children_predicates2(results);
    return 0;
}

