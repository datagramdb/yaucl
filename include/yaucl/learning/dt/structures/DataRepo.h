

//
// Created by giacomo on 02/10/24.
//

#ifndef DT_DATAREPO_H
#define DT_DATAREPO_H
#define DEBUG
#include <unordered_set>
#include <yaucl/functional/assert.h>
#include <deque>


#include <yaucl/learning/dt/commons.h>
#include "yaucl/learning/dt/wodt/WODT.h"
#include <set>
#include <map>
#include "yaucl/data/csv/csv.h"

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

    INLINE void init_offsets(const std::string& key, size_t begin, size_t end) {
        for (size_t idx = begin; idx<end; idx++) {
            size_t i = offsets[idx];
            size_t offset = fa.find_offset(records[i], key);
            DEBUG_ASSERT((offset == -1) || (records[i][offset].first == key));
            fieldOffset[i] = fa.find_offset(records[i], key);
        }
    }



    INLINE void sortOnSelectedObliquity(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice, size_t iterations = 1) {
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
            auto initial_a = optimize::Vector::Random(vars.size() + 1).array() - 0.5;
//            std::cout << subFeatures_X << std::endl;
            // With C++17 or greater we can omit the "<>"
            WODT f{subFeatures_X, Y, maxClass};
            optimize::Lbfgsb<> solver;                            // Solver using default stopping conditions
//    solver.setFunctionEvalsMax(10);
            optimize::State state = solver.minimize(f, initial_a);  // Solve with constraints (solution is f = 7.75 at x = [0.5, 0.5, 0.35])


//        std::cout << "f = " << state.f() << std::endl;             // Minimum of f(x)
//        std::cout << "x = " << state.x().transpose() << std::endl; // Argmin x of f(x)

            auto a = state.x();
            auto paras = a.tail(a.size() - 1);
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

    INLINE void sortOnSelectedCategoricalField(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice, const std::deque<size_t>& Q, const std::vector<Nodes>& children) {
        for (const auto& var : vars) {
            std::unordered_set<std::string> values_to_avoid;
            for (const auto& idx : Q) {
                if (children[idx].candidate.first.field == var) {
                    values_to_avoid.emplace(std::get<std::string>(children[idx].candidate.first.value));
                }
            }
            auto result = sortOnSelectedCategoricalField(var, begin, end, values_to_avoid);
            if (result.second > choice.second) {
                bool found = false;
                for (const auto& ref : Q) {
                    if ((result.first.field == children[ref].candidate.first.field) &&
                        (result.first.value == children[ref].candidate.first.value)) {
                        found=true;
                        break;
                    }
                }
                if (!found) {
                    result.first.field = var;
                    std::swap(choice, result);
                }
            }
        }
    }

    INLINE void sortOnSelectedNumericField(const std::unordered_set<std::string>& vars, size_t begin, size_t end, std::pair<dt_predicate, double>& choice, bool justSame, const std::deque<size_t>& Q, const std::vector<Nodes>& children) {
        for (const auto& var : vars) {
            // The extention for these operand types comes from the paper
            // of dtControl: https://dl.acm.org/doi/abs/10.1145/3365365.3382220
            std::unordered_set<double> values_to_avoid;
            for (const auto& idx : Q) {
                if (children[idx].candidate.first.field == var) {
                    values_to_avoid.emplace(std::get<double>(children[idx].candidate.first.value));
                }
            }
            size_t N = justSame ? 0 : (size_t)operand_type::LN;
            for (size_t i = 0; i<=N; i++) {
                auto obj = (operand_type)i;
                auto result = sortOnSelectedNumericField(var, begin, end, obj, values_to_avoid);

                if (result.second > choice.second) {
                    bool found = false;
                    for (const auto& ref : Q) {
                        if ((result.first.field == children[ref].candidate.first.field) &&
                                (result.first.value == children[ref].candidate.first.value)) {
                            found=true;
                            break;
                        }
                    }
                    if (!found) {
                        result.first.field = var;
                        std::swap(choice, result);
                    }
                }
            }
        }
    }

private:
    INLINE
    std::pair<dt_predicate,double> sortOnSelectedCategoricalField(const std::string& var, size_t begin, size_t end, const std::unordered_set<std::string>& avoidable_strings) {
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
            auto str = find_argument::findString(records[off], fieldOffset[off]);
            if (!avoidable_strings.contains(str)) {
                M.emplace_back(str);
            }
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
            val.first = find_argument::findString(records[off], fieldOffset[off]);
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



    INLINE std::pair<dt_predicate, double> sortOnSelectedNumericField(const std::string& var, size_t begin, size_t end, operand_type ot = operand_type::SAME, const std::unordered_set<double>&avoidables = {}) {
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
                double const avg = (valx+varx)/2.0;
                val.first = avg;
                if (!avoidables.contains(avg)) {
                    for (int i = 0; i<=max_class_id; i++) {
                        val.second = i;
                        N[val] = nv.count(i);
                    }
                    M.insert(val.first);
                }
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

    INLINE void init_matrix(const std::vector<std::string>& numerical_dimensions,
                            optimize::Matrix& X_out, optimize::Vector& Y_out) {
        size_t colSize = numerical_dimensions.size();
        size_t N = records.size();
        X_out = optimize::Matrix::Zero(N, colSize);
        Y_out = optimize::Vector::Zero(N);
//        forthegain.init(max_class_id);
        for (size_t i = 0; i<N; i++) {
            int clazz = records_classes[i];
//            forthegain.countClass(clazz);
            Y_out(i) = clazz;
            for (size_t j = 0; j<colSize; j++) {
                const std::string& dimName = numerical_dimensions.at(j);
                X_out(i,j) = (fa.findNumeric(records[i], dimName));
            }
        }
//        std::cout << X_out << std::endl;
    }

    INLINE void init_matrix(const std::set<size_t>& selected_records,
                            const std::vector<std::string>& numerical_dimensions,
                            optimize::Matrix& X_out, optimize::Vector& Y_out) {
        if (selected_records.empty()) {
            init_matrix(numerical_dimensions, X_out, Y_out);
        } else {
            size_t colSize = numerical_dimensions.size();
            size_t N = selected_records.size();
            X_out = optimize::Matrix::Zero(N, colSize);
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
                    X_out(offsetElem,j) = (fa.findNumeric(records[i], dimName));
                }
                offsetElem++;
            }
        }

    }
};


#endif //DT_DATAREPO_H
