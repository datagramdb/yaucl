


#include <dt/DecisionTree.h>

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

