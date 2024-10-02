
#include <map>
#include <iostream>
#include <variant>
#include <vector>
#include <stack>

#include <unordered_set>
#include <dt/commons.h>




#include <limits>
#include <set>
constexpr double epsilonepsilon = 1e-220;

static inline
std::vector<int> getOnLocations(int a) {
    std::vector<int> result;
    int place = 0;
    while (a != 0) {
        if (a & 1) {
            result.push_back(place);
        }
        ++place;
        a >>= 1;
    }
    return result;
}

#include <cmath>

template<typename T>
std::vector<std::unordered_set<T> > powerSet(const std::vector<T>& set, size_t maxSize) {
    std::vector<std::unordered_set<T> > result;
    int numPowerSets = static_cast<int>(std::pow(2.0, static_cast<double>(set.size())));
    for (size_t i = 0; i < numPowerSets; ++i) {
        std::vector<int> onLocations = getOnLocations(i);
        if (onLocations.size() > maxSize) continue;
        if (onLocations.empty()) continue;
        std::unordered_set<T> subSet;
        for (size_t j = 0; j < onLocations.size(); ++j) {
            subSet.insert(set.at(onLocations.at(j)));
        }
        result.push_back(subSet);
    }
    return result;
}


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
             ForTheWin::gain_measures measure = ForTheWin::gain_measures::Gini) : measure{measure}, records{records}, records_classes{records_classes}, l{l}, eta{eta}, maxPrec{maxPrec}, max_height{max_height} {
        max_record_size = std::min(records_classes.size(), records.size());
        offsets.resize(max_record_size);
        fieldOffset.resize(max_record_size, 0);
        for (size_t i = 0; i<max_record_size; i++) {
            offsets[i] = i;
            max_class_id = std::max(max_class_id, records_classes[i]);
        }
        nv.init(max_class_id);
        forthegain.init(max_class_id);
    }

    inline void init_offsets(const std::string& key, size_t begin, size_t end) {
        for (size_t idx = begin; idx<end; idx++) {
            size_t i = offsets[idx];
            fieldOffset[i] = fa.find_offset(records[i], key);
        }
    }

    inline
    std::pair<dt_predicate,double> sortOnSelectedCategoricalField(size_t begin, size_t end) {
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

    inline std::pair<dt_predicate, double> sortOnSelectedNumericField(size_t begin, size_t end) {
        std::sort(offsets.begin()+begin,
                  offsets.begin()+end,
                  [this](size_t idxl, size_t idxr) {
                        auto val = find_argument::findNumeric(records[idxl], fieldOffset[idxl]);
                        auto var = find_argument::findNumeric(records[idxr], fieldOffset[idxr]);
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
            auto valx = find_argument::findNumeric(records[i], fieldOffset[i]);
            auto varx = find_argument::findNumeric(records[ip1], fieldOffset[ip1]);
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

};

#include <algorithm>
#include <dt/utils.h>


struct DecisionTree {
    DataRepo dr;
    std::vector<Nodes> children;
    double goodness;
    double total_weights;
    const std::unordered_set<std::string>& numerical;
    const std::unordered_set<std::string>&categorical;

    DecisionTree(const data_record& records,
             const data_clazzes& records_classes,
                 const std::unordered_set<std::string>& numerical,
                 const std::unordered_set<std::string>& categorical,
                 size_t max_height = std::numeric_limits<size_t>::max(),
                 double maxPrec = 1.0,
                 size_t l = 1,
                 size_t eta = 1) : dr{records,records_classes,max_height,maxPrec,l,eta}, numerical{numerical}, categorical{categorical} {
        goodness = total_weights = 0;
    }


    void splitTree() {
        std::stack<size_t> currentNode;
        children.emplace_back(0, dr.max_record_size, 0);
        currentNode.push(0);
        size_t curr = 0;
        while (!currentNode.empty()) {
            curr = currentNode.top();
            auto& node = children[curr];
            dr.forthegain.reset();

            if ((node.split == 0) && (node.rightOffset == 0)) {
                size_t N = node.end - node.begin;
//                std::cout << node.begin << "---"<<node.end << std::endl;

                double purity = 0.0;
                int clazz = -1;

                for (std::size_t i = node.begin; i<node.end; i++) {
                    int curr_clazz = dr.records_classes[dr.offsets[i]];
                    auto tmp = (double)(dr.forthegain.countClass(curr_clazz));
                    if (tmp > purity) {
                        purity = tmp;
                        clazz = curr_clazz;
                    }
                }
                dr.forthegain.normalizeCountClass();
                const double current_weight = purity / ((double)dr.max_record_size);
                purity = purity/((double) N);
                bool terminate = false;
                if ((N<=dr.eta) || (purity >= dr.maxPrec) || (node.max_height == dr.max_height)) {
                    terminate = true;
                } else {
                    node.candidate.second = -1;

                    for (const auto& var : numerical) {
                        dr.init_offsets(var, node.begin, node.end);
                        auto result = dr.sortOnSelectedNumericField(node.begin, node.end);
                        if (result.second > node.candidate.second) {
                            result.first.field = var;
                            std::swap(node.candidate, result);
                        }
                    }
                    for (const auto& var : categorical) {
                        dr.init_offsets(var, node.begin, node.end);
                        auto result = dr.sortOnSelectedCategoricalField(node.begin, node.end);
                        if (result.second > node.candidate.second) {
                            result.first.field = var;
                            std::swap(node.candidate, result);
                        }
                    }

                    if (node.candidate.second == -1) {
                        terminate = true;
                    } else {
                        dr.init_offsets(node.candidate.first.field, node.begin, node.end);
                        auto beg = dr.offsets.begin()+node.begin;
                        auto end = dr.offsets.begin()+node.end;
                        auto it2 = std::stable_partition(beg,
                                                         end,
                                                         [&node,this](const size_t& obj) {
                                                             if (!(this->dr.records[obj][this->dr.fieldOffset[obj]].first == node.candidate.first.field))
                                                                 exit(199);
                                                             return node.candidate.first(this->dr.records[obj][this->dr.fieldOffset[obj]].second);
                                                         });
                        if ((it2 == beg) || (it2 == end)) {
                            terminate = true;
                        }
                        node.split = (size_t)((ptrdiff_t)(it2-dr.offsets.begin()));
                        currentNode.push(children.size());
                        children.emplace_back(node.begin, node.split, node.max_height+1);
                    }
                }

                if (terminate) {
                    node.isLeaf = true;
                    node.majority_class = clazz;
                    node.majority_class_precision = dr.forthegain.getClassPrecision(clazz);
                    total_weights += current_weight;
                    goodness += (node.majority_class_precision * current_weight);
                    currentNode.pop();
                    continue;
                }
            } else if (node.rightOffset == 0) {
                node.rightOffset = children.size();
                currentNode.push(node.rightOffset);
                children.emplace_back(node.split, node.end, node.max_height+1);
            } else {
                currentNode.pop();
            }
        }

        if (total_weights > 0.0) {
            goodness = goodness / total_weights;
        }
    }

    void populate_children_predicates2(std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred) const {
        std::vector<dt_predicate> memo;
        populate_children_predicates2(0, decision_to_pred, memo);
    }

    void populate_children_predicates2(size_t nodeid,
            std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> &decision_to_pred,
                                       std::vector<dt_predicate>& current_stack) const {
        auto& node = children[nodeid];
        if (!node.isLeaf) {
            const auto sat_ptr = children[nodeid+1];
            const auto viol_ptr = children[node.rightOffset];

            {
                auto unsat_copy = current_stack;
                auto& cpy = unsat_copy.emplace_back(node.candidate.first);
                switch(cpy.pred){
                    case dt_predicate::LEQ_THAN:
                        cpy.pred = dt_predicate::G_THAN;
                        break;
                    case dt_predicate::IN_SET:
                        cpy.pred = dt_predicate::NOT_IN_SET;
                        break;
                    case dt_predicate::L_THAN:
                    case dt_predicate::G_THAN:
                    case dt_predicate::GEQ_THAN:
                    case dt_predicate::NOT_IN_SET:
                        /* We should never get here, assign copies only */
                        break;
                }
                populate_children_predicates2(node.rightOffset, decision_to_pred, unsat_copy);
            }
            {
                current_stack.emplace_back(node.candidate.first);
                populate_children_predicates2(nodeid+1,decision_to_pred, current_stack);
            }
        } else {
            decision_to_pred[children[nodeid].majority_class].emplace_back(children[nodeid].majority_class_precision, current_stack);
        }
    }
};


/*def sigmoid(z):
# because that -z is too big will arise runtimeWarning in np.exp()
if isinstance(z, float) and (z < -500):
z = -500
elif not (isinstance(z, float)):
z[z < -500] = (-500) * np.ones(sum(z < -500))

return 1 / (np.exp(- z) + 1)*/

static inline double sigmoid(double z) {
    if (z<-500) z=-500;
    return 1/(std::exp(-z)+1);
}

#include "lbfgsb.h"

Eigen::IOFormat HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

class WODT : public optimize::Function
{
private:
    static constexpr optimize::Scalar b = 100;
    const optimize::Matrix& subFeatures_X;
    const optimize::Vector& Y;
    int maxClass;

public:
    WODT(const optimize::Matrix& subFeatures_X,
         const optimize::Vector& Y,
         int maxClass) : subFeatures_X{subFeatures_X}, Y{Y}, maxClass{maxClass} {}


    optimize::Scalar computeValue(const optimize::Vector& a) override
    {
        Eigen::VectorXd paras = a.tail(a.size() - 1);
        double threshold = a(0);
//        VectorXd p = sigmoid((subFeatures_X * paras) - threshold);
        optimize::Vector p = ((subFeatures_X * paras).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
        auto w_R = p;
        auto w_L = Eigen::VectorXd::Ones(w_R.size()) - w_R;
        double w_R_sum = w_R.sum();
        double w_L_sum = w_L.sum();

        Eigen::VectorXd w_R_eachClass = Eigen::VectorXd::Zero(maxClass);
        Eigen::VectorXd w_L_eachClass = Eigen::VectorXd::Zero(maxClass);
        for (int k = 0; k < maxClass; ++k) {
            w_R_eachClass(k) = (Y.array() == k).select(w_R, 0).sum();//(Y.array() == k).cast<double>().dot(w_R);
            w_L_eachClass(k) = (Y.array() == k).select(w_L, 0).sum(); //(Y.array() == k).cast<double>().dot(w_L);
        }

        double fun = w_L_sum * std::log2(w_L_sum + epsilonepsilon) +
                     w_R_sum * std::log2(w_R_sum + epsilonepsilon) -
                     (w_R_eachClass.array() * (w_R_eachClass.array() + epsilonepsilon).log2()).sum() -
                     (w_L_eachClass.array() * (w_L_eachClass.array() + epsilonepsilon).log2()).sum();

        return fun;
//        optimize::Vector paras = x.segment(1, x.size()-1);
//        auto threshold = x.coeffRef(0);
//        auto w_R = p;
//        auto w_L = 1 - w_R.array();
//        auto w_R_sum = w_R.sum();
//        auto w_L_sum = w_L.sum();
//        optimize::Scalar fun = 0;
//        fun += w_L_sum * std::log2(w_L_sum + epsilonepsilon);
//        fun += w_R_sum * std::log2(w_R_sum + epsilonepsilon);
//        for (int i = 0; i<=maxClass; i++) {
//            double w_R_eachClass_i = (Y.array() == i).select(w_R, 0).sum();
//            fun -= (w_R_eachClass_i * std::log2(w_R_eachClass_i + epsilonepsilon));
//            double w_L_eachClass_i = (Y.array() == i).select(w_L, 0).sum();
//            fun -= (w_L_eachClass_i * std::log2(w_L_eachClass_i + epsilonepsilon));
//        }
//        return fun;
    }

    optimize::Vector computeGradient(const optimize::Vector& a) override
    {

        Eigen::VectorXd paras = a.tail(a.size() - 1);
        double threshold = a(0);

        optimize::Vector p = ((subFeatures_X * paras).array() - a.coeffRef(0)).unaryExpr(&sigmoid);
//        VectorXd p = sigmoid((subFeatures_X * paras) - threshold);
        auto w_R = p;
        auto w_L = Eigen::VectorXd::Ones(w_R.size()) - w_R;

        Eigen::VectorXd w_R_eachClass = Eigen::VectorXd::Zero(maxClass);
        Eigen::VectorXd w_L_eachClass = Eigen::VectorXd::Zero(maxClass);
        for (int k = 0; k < maxClass; ++k) {
            w_R_eachClass(k) = (Y.array() == k).select(w_R, 0).sum();//(Y.array() == k).cast<double>().dot(w_R);
            w_L_eachClass(k) = (Y.array() == k).select(w_L, 0).sum(); //(Y.array() == k).cast<double>().dot(w_L);
        }

        Eigen::VectorXd la = (w_L_eachClass.array() * w_R.sum() + epsilonepsilon).log2() -
                      (w_R_eachClass.array() * w_L.sum() + epsilonepsilon).log2();
        Eigen::VectorXd beta = la.array() * p.array() * (1 - p.array());
        Eigen::VectorXd jac = Eigen::VectorXd::Zero(a.size());
        jac(0) = -beta.sum();
        jac.tail(a.size() - 1) = subFeatures_X.transpose() * beta;
        return jac;
//
//        for (optimize::Index i = 0; i < x.size() - 1; ++i) {
//            if (i == 0) {
//                g(i) = 4*b*(x(i)*x(i)*x(i) - x(i)*x(i+1)) + 2*x(0) - 2;
//            }
//            if (i > 0 && i < x.size() - 1) {
//                g(i) = 4*b*(x(i)*x(i)*x(i) - x(i)*x(i+1)) + 2*b*(x(i) - x(i-1)*x(i-1)) + 2*x(i) - 2;
//            }
//            if (i+1 == x.size() - 1) {
//                g(i+1) = 2*b*(x(i+1) - x(i)*x(i));
//            }
//        }
//        return g;
    }
};

bool computeclass(const optimize::State& s, const optimize::Vector& a,
                    const Eigen::VectorXi& attrIDs) {
    Eigen::VectorXd paras = a.tail(a.size() - 1);
    double threshold = a(0);
    auto proj = a(attrIDs);
    return proj.dot(paras.transpose()) <= threshold;
}

#include <random>

int main() {
    std::string file = "/home/giacomo/Scaricati/twigy-master/test/testdata/titanic_data.csv";
    // Read the example data and split it into features and labels
    table data_frame = ReadCsv(file);
    std::vector<int> y;
    std::string clazz = "Survived";
    std::vector<std::vector<std::pair<std::string,union_minimal>>> X;
    Split_X_y(data_frame, X, y, "Survived");
    auto h = data_frame.headers_;
    h.erase(std::remove(h.begin(), h.end(), clazz), h.end());
    h.erase(std::remove(h.begin(), h.end(), "Labelo"), h.end());
    std::unordered_set<std::string> N{h.begin(), h.end()};
    std::unordered_set<std::string> cat{"Labelo"};

    Eigen::MatrixXi m(1, 5), t(1,5), e(1,5);
    m << 1, 2, 3, 4, 5;
    t << 6,7,8,9,10;
    e << 16,17,18,19,20;
    (m.array() == 3).select(t, 0).sum();
    std::cout << (m.array() == 3).select(t, 0).sum() << std::endl;

    size_t feature_num = 8; // number of numerical attributes in the matrix
    size_t subset_feature_num = 2; // subset of the aforementioned features
    std::vector<int> feature_ids(feature_num);
    std::iota(feature_ids.begin(), feature_ids.end(), 0); // Fill feature_ids with range 0 to feature_num-1

    std::vector<int> subset_feature_ids(subset_feature_num);
    std::sample(feature_ids.begin(), feature_ids.end(), subset_feature_ids.begin(),
                subset_feature_num, std::mt19937{std::random_device{}()}); // Get random subset of features

    const optimize::Matrix subFeatures_X;
    const optimize::Vector Y;
    int maxClass;
//    Eigen::MatrixXd X = self.X; // Assuming self.X is an Eigen matrix
//    Eigen::MatrixXd subFeatures_X = X(self.sample_ids.col(0), subset_feature_ids.transpose());
//    Eigen::VectorXd Y = self.Y(self.sample_ids);
//    int class_num = self.class_num;
    Eigen::VectorXd initial_a = Eigen::VectorXd::Random(subset_feature_num + 1).array() - 0.5;

    // With C++17 or greater we can omit the "<>"
    WODT f{subFeatures_X, Y, maxClass};
    optimize::Lbfgsb<> solver;                            // Solver using default stopping conditions
    solver.setFunctionEvalsMax(10);
    optimize::State state = solver.minimize(f, initial_a);  // Solve with constraints (solution is f = 7.75 at x = [0.5, 0.5, 0.35])


    std::cout << "f = " << state.f() << std::endl;             // Minimum of f(x)
    std::cout << "x = " << state.x().transpose() << std::endl; // Argmin x of f(x)

    // preview of the data
//    data_frame.Print(10);
//
//    DecisionTree DT{X,y, N, cat, 5};
//    DT.splitTree();
//
//    std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> results;
//    DT.populate_children_predicates2(results);
    return 0;
}
