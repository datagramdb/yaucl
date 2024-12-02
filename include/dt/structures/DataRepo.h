

//
// Created by giacomo on 02/10/24.
//

#ifndef DT_DATAREPO_H
#define DT_DATAREPO_H


#include <dt/commons.h>
#include "dt/wodt/WODT.h"
#include <set>
#include <map>
#include "dt/utils.h"

struct DataRepo {
    const data_record& records;
    const data_clazzes& records_classes;
    std::vector<size_t> offsets;
    std::vector<int> fieldOffset;
    double maxPrec;
    size_t l;
    size_t eta;
    int max_class_id = -1;
    size_t max_record_size;
    size_t max_height;
    n_vector nv;
    find_argument fa;
    ForTheWin forthegain;
    ForTheWin::gain_measures measure;


    DataRepo(const data_record& records,
             const data_clazzes& records_classes,
             size_t max_height = std::numeric_limits<size_t>::max(),
             double maxPrec = 1.0,
             size_t l = 1,
             size_t eta = 1,
             ForTheWin::gain_measures measure = ForTheWin::gain_measures::Gini);

    inline void init_offsets(const std::string& key, size_t begin, size_t end) {
        for (size_t idx = begin; idx<end; idx++) {
            size_t i = offsets[idx];
            fieldOffset[i] = fa.find_offset(records[i], key);
        }
    }



    inline void sortOnSelectedObliquity(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice, size_t iterations = 1) {
        // This was inspired by decision trees provided here: https://dl.acm.org/doi/abs/10.1145/3365365.3382220
        // This method is a direct C++ translation of the following python code:
        // https://github.com/TorshaMajumder/Ensembles_of_Oblique_Decision_Trees/blob/master/Decision_trees/WODT.py
        if (iterations == 0)
            iterations = 1;
        std::vector<std::string> W{vars.begin(), vars.end()};
        optimize::Matrix subFeatures_X;
        optimize::Vector Y;
        int maxClass = max_class_id;

        auto it = offsets.begin(), beg = it+begin, en = it+end;
        std::set<size_t> elements;
//        elements.reserve((ptrdiff_t )(en-beg));
        for (auto it2 = beg; it2 != en; it2++) {
            auto offset = *it2;
            elements.emplace(offset);
        }
        init_matrix(elements, W, subFeatures_X, Y);
        for (auto iteration = 0; iteration<iterations; iteration++ ) {
            Eigen::VectorXd initial_a = Eigen::VectorXd::Random(vars.size() + 1).array() - 0.5;
//            std::cout << subFeatures_X << std::endl;
            // With C++17 or greater we can omit the "<>"
            WODT f{subFeatures_X, Y, maxClass};
            optimize::Lbfgsb<> solver;                            // Solver using default stopping conditions
//    solver.setFunctionEvalsMax(10);
            optimize::State state = solver.minimize(f, initial_a);  // Solve with constraints (solution is f = 7.75 at x = [0.5, 0.5, 0.35])


//        std::cout << "f = " << state.f() << std::endl;             // Minimum of f(x)
//        std::cout << "x = " << state.x().transpose() << std::endl; // Argmin x of f(x)

            auto a = state.x();
            Eigen::VectorXd paras = a.tail(a.size() - 1);
            auto weight = a.coeffRef(0);

            std::unordered_map<std::string,double> w;
            for (size_t idx = 0, N = paras.size(); idx<N; idx++) {
                w[W.at(idx)] = paras(idx);
            }
            auto result = ((subFeatures_X * paras).array() -a.coeffRef(0) <= 0.0);
//        ForTheWin forthegain;
            double forthepos = 0.0, fortheneg = 0.0;
            std::vector<std::pair<double,double>> results(maxClass+1, {0,0});
            dt_predicate prepad(w, weight);
            auto it2 = elements.begin();
            for (size_t i = 0, N = result.size(); i<N; i++) {
                if (result(i)) {
                    if (!prepad(records[*it2]))
                        exit(20);
                    forthepos++;
                    results[Y(i)].first++;
                } else {
                    if (prepad(records[*it2]))
                        exit(30);
                    fortheneg++;
                    results[Y(i)].second++;
                }
                it2++;
            }

            forthegain.goodBad(forthepos, fortheneg);
            for (size_t clazz = 0; clazz<=maxClass; clazz++ ){
                forthegain.setP(clazz, results[clazz].first, results[clazz].second);
            }
            auto score = forthegain.getGain(measure);
            if (score > choice.second) {
                std::swap(prepad, choice.first);
                choice.second = score;
            }
        }
    }

    inline void sortOnSelectedCategoricalField(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice) {
        for (const auto& var : vars) {
            auto result = sortOnSelectedCategoricalField(var, begin, end);
            if (result.second > choice.second) {
                result.first.field = var;
                std::swap(choice, result);
            }
        }
    }

    inline void sortOnSelectedNumericField(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice) {
        for (const auto& var : vars) {
            // The extention for these operand types comes from the paper
            // of dtControl: https://dl.acm.org/doi/abs/10.1145/3365365.3382220
            for (size_t i = 0, N = (size_t)operand_type::LN; i<N; i++) {
                auto obj = (operand_type)i;
                auto result = sortOnSelectedNumericField(var, begin, end, obj);
                if (result.second > choice.second) {
                    result.first.field = var;
                    std::swap(choice, result);
                }
            }
        }
    }

private:
    inline
    std::pair<dt_predicate,double> sortOnSelectedCategoricalField(const std::string& var, size_t begin, size_t end) {
        init_offsets(var, begin, end);
        std::sort(offsets.begin()+begin,
                  offsets.begin()+end,
                  [this](size_t idxl, size_t idxr) {
                      auto val = find_argument::findString(records[idxl], fieldOffset[idxl]);
                      auto var = find_argument::findString(records[idxr], fieldOffset[idxr]);
                      return (val <  var) || ((val == var) && (records_classes.at(idxl) < records_classes.at(idxr)));
                  });
        nv.reset();
        std::map<std::pair<std::string,int>, size_t> N;
        std::pair<std::string,int> val;
        std::vector<std::string> M;
        for (auto it = begin; it != end; it++) {
            auto off = offsets[it];
            M.emplace_back(std::get<std::string>(records[off][fieldOffset[off]].second));
        }
        std::sort( M.begin(), M.end() );
        M.erase( unique( M.begin(), M.end() ), M.end() );
        for (int i = 0; i<=max_class_id; i++) {
            val.second = i;
            for (const auto& ref : M) {
                val.first = ref;
                N[val] = 0;
            }
        }
        for (auto it = begin; it != end; it++) {
            auto off = offsets[it];
            val.first = std::get<std::string>(records[off][fieldOffset[off]].second);
            val.second = records_classes[off];
            N[val]++;
        }

        std::pair<dt_predicate, double> predicate_w_score;
        predicate_w_score.second = -std::numeric_limits<double>::max();
        predicate_w_score.first.pred = dt_predicate::IN_SET;
        double forthepos, fortheneg;
        double localPos, localNeg;
        for (const std::unordered_set<std::string>& V : powerSet(M, l)) {
            fortheneg = 0.0;
            forthepos = 0.0;
            std::unordered_set<std::string> VCompl;
            for (int i = 0; i<=max_class_id; i++) {
                val.second = i;
                for (const auto& item : M) {
                    val.first = item;
                    if (V.contains(item)) {
                        forthepos += N[val];
                    } else {
                        VCompl.insert(item);
                        fortheneg += N[val];
                    }
                }
            }
            forthegain.goodBad(forthepos, fortheneg);
            for (int i= 0; i<=max_class_id; i++) {
                localPos= 0.0; localNeg = 0.0;
                val.second = i;
                for (const auto& item :V) {
                    val.first = item;
                    localPos += N[val];
                }
                for (const auto& item: VCompl) {
                    val.first = item;
                    localNeg += N[val];
                }
                forthegain.setP(i, localPos, localNeg);
                double local_score = forthegain.getGain(measure);
                if (local_score > predicate_w_score.second) {
                    predicate_w_score.second = local_score;
                    predicate_w_score.first.categoric_set = V;
                }
            }
        }
        return predicate_w_score;
    }



    inline std::pair<dt_predicate, double> sortOnSelectedNumericField(const std::string& var, size_t begin, size_t end, operand_type ot = operand_type::SAME) {
        init_offsets(var, begin, end);
        std::sort(offsets.begin()+begin,
                  offsets.begin()+end,
                  [this,ot](size_t idxl, size_t idxr) {
                      auto val = do_operation(find_argument::findNumeric(records[idxl], fieldOffset[idxl]),ot);
                      auto var = do_operation(find_argument::findNumeric(records[idxr], fieldOffset[idxr]),ot);
                      return (val <  var) || ((val == var) && (records_classes.at(idxl) < records_classes.at(idxr)));
                  });
        nv.reset();
        std::map<std::pair<double,int>, size_t> N;
        std::pair<double,int> val;
        std::set<double> M;
        for (size_t idx = begin; idx<end-1; idx++) {
            size_t i = offsets[idx];
            size_t ip1 = offsets[idx+1];
            nv.increment(records_classes[i]);
            auto valx = do_operation(find_argument::findNumeric(records[i], fieldOffset[i]),ot);
            auto varx = do_operation(find_argument::findNumeric(records[ip1], fieldOffset[ip1]),ot);
            if (valx != varx) {
                val.first = (valx+varx)/2.0;
                for (int i = 0; i<=max_class_id; i++) {
                    val.second = i;
                    N[val] = nv.count(i);
                }
                M.insert(val.first);
            }
        }
        size_t i = offsets[end-1];
        nv.increment(records_classes[i]);

        std::pair<dt_predicate, double> predicate_w_score;
        predicate_w_score.second = -std::numeric_limits<double>::max();
        predicate_w_score.first.pred = dt_predicate::LEQ_THAN;
        predicate_w_score.first.argfun = ot;
        for (const auto& x : M) {
            val.first = x;
            double forthepos = 0.0, fortheneg = 0.0;
            for (int i = 0; i<=max_class_id; i++) {
                val.second = i;
                auto nvv = N[val];
                forthepos += (double)nvv;
                fortheneg += (double)(nv.count(i)-nvv);
            }
            forthegain.goodBad(forthepos, fortheneg);
            for (int i= 0; i<=max_class_id; i++) {
                val.second = i;
                forthegain.setP(i, (double)N[val], (double)(nv.count(i)-N[val]));
            }
            double local_score = forthegain.getGain(measure);
            if (local_score > predicate_w_score.second) {
                predicate_w_score.second = local_score;
                predicate_w_score.first.value = x;
            }
        }
        return predicate_w_score;
    }

    inline void init_matrix(const std::vector<std::string>& numerical_dimensions,
                            optimize::Matrix& X_out, optimize::Vector& Y_out) {
        size_t colSize = numerical_dimensions.size();
        size_t N = records.size();
        X_out = Eigen::MatrixXd::Zero(N, colSize);
        Y_out = optimize::Vector::Zero(N);
//        forthegain.init(max_class_id);
        for (size_t i = 0; i<N; i++) {
            int clazz = records_classes[i];
//            forthegain.countClass(clazz);
            Y_out(i) = clazz;
            for (size_t j = 0; j<colSize; j++) {
                const std::string& dimName = numerical_dimensions.at(j);
                X_out(i,j) = std::get<double>(fa.find(records[i], dimName));
            }
        }
//        std::cout << X_out << std::endl;
    }

    inline void init_matrix(const std::set<size_t>& selected_records,
                            const std::vector<std::string>& numerical_dimensions,
                            optimize::Matrix& X_out, optimize::Vector& Y_out) {
        if (selected_records.empty()) {
            init_matrix(numerical_dimensions, X_out, Y_out);
        } else {
            size_t colSize = numerical_dimensions.size();
            size_t N = selected_records.size();
            X_out = Eigen::MatrixXd::Zero(N, colSize);
            Y_out = optimize::Vector::Zero(N);
            size_t offsetElem = 0;
//            forthegain.init(max_class_id);
            for (size_t i : selected_records) {
//                if (i >= records.size())
//                    exit(101);
//                i = offsets[i];
                int clazz = records_classes[i];
//                forthegain.countClass(clazz);
                Y_out(offsetElem) = clazz;
                for (size_t j = 0; j<colSize; j++) {
                    const std::string& dimName = numerical_dimensions.at(j);
                    X_out(offsetElem,j) = std::get<double>(fa.find(records[i], dimName));
                }
                offsetElem++;
            }
        }

    }
};


#endif //DT_DATAREPO_H
