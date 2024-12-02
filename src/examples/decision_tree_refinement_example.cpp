//
// Created by giacomo on 31/07/23.
//

#if 0

#include <yaucl/learning/DecisionTree.h>

struct atoms {
    double a;
    double b;
    double c;
    DEFAULT_CONSTRUCTORS(atoms);
    atoms(bool a, bool b, bool c) :a{a ? 1.0 : -1.0}, b{b ? 1.0 : -1.0},c{c ? 1.0 : -1.0} {};
    friend std::ostream& operator<<(std::ostream& os, const atoms& x)  {
        return os << "<a=" << x.a << ",b=" << x.b << ",c=" <<x.c << ">";
    }
};

namespace std {
    template <>
    struct hash<atoms> {
        std::size_t operator()(const atoms& k) const {
            size_t val = 0;
            if (k.a == 1.0) val+=1;
            if (k.b == 1.0) val+=2;
            if (k.c == 1.0) val+=4;
            return val;
        }
    };
}


int main()
{
    atoms a1_c1{true,true,false};
    atoms a2_c1{true,false,false};
    atoms a7_c1{true,false,false};
    atoms a8_c1{true,true,true};

#ifdef DIFFICULT
    atoms a4_c2{true,true,true};
    atoms a0_c2{true,false,true};
#else
    atoms a4_c2{false,true,true};
    atoms a0_c2{false,false,true};
#endif
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    atoms a3_c2{false,false,true};
    atoms a5_c2{false,true,true};
    atoms a9_c2{false,true,true};

    atoms a6_c3{false,true,false};
    auto selector = [](atoms x, const std::string& key) -> double {
        if (key == "a")
            return x.a;
        else if (key == "b")
            return x.b;
        else if (key == "c")
            return x.c;
        return 0.0;
    };
    std::vector<std::pair<atoms,int>> V{{a1_c1,0}, {a2_c1,0}, {a3_c2,1}, {a4_c2,1}, {a5_c2,1}, {a6_c3,2}};
    std::vector<std::pair<atoms,int>> W{{a1_c1,0}, {a7_c1,0}, {a8_c1,0}, {a9_c2,1}, {a0_c2,1}, {a5_c2,1}, {a6_c3,2}};

    auto it = V.begin(), en = V.end();
    auto it2 = W.begin(), en2 = W.end();
    std::unordered_set<std::string> fields{"a","b","c"};

    std::vector<DecisionTree<atoms>> w;
    decision_tree_collection decisions;
    w.reserve(2);

    auto& dt = w.emplace_back(it,
                              en,
                              2,
                              selector,
                              std::unordered_set<std::string>{},
                              fields,
                              ForTheWin::gain_measures::Entropy,
                              0.9,
                              1,
                              1,
                              1,
                              false);
    auto& dt2 = w.emplace_back(it2,
                               en2,
                               2,
                               selector,
                               std::unordered_set<std::string>{},
                               fields,
                               ForTheWin::gain_measures::Entropy,
                               0.9,
                               1,
                               1,
                               1,
                               false);
    size_t count = 0;
    std::unordered_set<std::string> all_fields; // All the classifiers that are trained, represented as a string for their id
    std::unordered_set<int> S{0,1,2}; // All the classes that are expected to be returned
    std::unordered_map<std::string, std::unordered_set<int>> meta_map_detection; // Mapping each classifier to each possible turnout
    // Initialisation
    for (const auto& dt_curr : w) {
        auto s = std::to_string(count++);
        all_fields.emplace(s);
        meta_map_detection[s] = S;
    }
    // Providing a serialisable representation of the decision, as a tabular function
    for (const auto& dt_curr : w) {
        std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> map;
        dt_curr.populate_children_predicates(map);
        decisions.emplace_back(concrete_dt_table_from_class_result(map,  false, meta_map_detection));
        std::cout << decisions.back() << std::endl;
    }

    // This use case specific, when disparate data was trained
    std::vector<std::pair<atoms,int>> union_data;
    union_data.insert(union_data.begin(), V.begin(), V.end());
    union_data.insert(union_data.begin(), W.begin(), W.end());

    std::vector<int> atom_to_decisions;
    std::vector<std::pair<std::vector<int>,int>> meta_classification;

    // After training the classifier, setting up the enseble method, and determining what is the expected outcome
    // from each classifier
    for (const auto& atom : union_data) {
        atom_to_decisions.clear();
        for (const auto& decision : decisions) {
            // adding the expected classification outcome from one classifier
            atom_to_decisions.emplace_back(classification_outcome<atoms>(decision, selector, atom.first));
        }
        // Associating each classifier's outcome in atom_to_decisions to the expected outcome of the classifier
        meta_classification.emplace_back(atom_to_decisions, atom.second);
    }

    auto itM = meta_classification.begin(), enM = meta_classification.end();
    // Return the class predicted by the classifier, as stored in the row
    auto meta_selector = [](const std::vector<int>& x, const std::string& key) -> double {
        try {
            size_t k = std::stoull(key);
            if (k>=x.size())
                return 0.0;
            else
                return x.at(k);
        } catch (...) {
            return 0.0;
        }

    };

    // Training the meta-classifier, deciding upon the other classifiers
    DecisionTree<std::vector<int>> meta(itM,
                                        enM,
                                        2,
                                        meta_selector,
                                        std::unordered_set<std::string>{},
                                        all_fields,
                                        ForTheWin::gain_measures::Entropy,
                                        1.0,
                                        1,
                                        1,
                                        1,
                                        false);
//    meta.print_rec(std::cout, 4);
    // Providing a tabular representation of the classification outcome for the ensemble method
    std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> meta_map;
    meta.populate_children_predicates2(meta_map);
    std::cout << meta_map << std::endl << std::endl;
//    exit(4);
    // Using the predicted way to combine the classifiers to combine the data associated to each classifier
    auto meta_classification_specific = instantiate_meta_classifier_result(meta_map, decisions, semantics_ignore, meta_map_detection);

    std::unordered_set<std::string> meta_fields;
    for (const auto& [clazz, alteranatives] : meta_classification_specific) {
        std::unordered_set<dt_predicate> predicate_build_up;
        for (const auto& alternative : alteranatives)
            for (const auto& [k,v] : alternative) {
                predicate_build_up.emplace((double)v, k);
                meta_fields.insert(k);
            }


        std::vector<std::pair<atoms,int>> class_ova;
        class_ova.reserve(union_data.size());
        for (const auto& x : union_data) {
            class_ova.emplace_back(x.first, x.second == clazz ? 1.0 : 0.0);
        }

        auto itL = class_ova.begin(), enL = class_ova.end();
        DecisionTree<atoms> local(itL,
                                  enL,
                                  1,
                                  selector,
                                  predicate_build_up,
                                  ForTheWin::gain_measures::Entropy,
                                  1.0,
                                  1,
                                  1,
                                  1,
                                  false);
        std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> local_map;
        local.populate_children_predicates2(local_map);
        auto exact_class = concrete_dt_table_from_class_result(local_map,  false, meta_map_detection)[1];
        concrete_dt_table_for_classes resulted;
        oplus_concrete_dt_tables(meta_classification_specific[clazz], exact_class, resulted, semantics_discard, true);
        std::cout << clazz << " ::= " << resulted << " vs. "<< exact_class << std::endl;
    }

    std::cout << meta_classification_specific[2] << std::endl;
    std::cout << meta_classification_specific[1] << std::endl;
    std::cout << meta_classification_specific[0] << std::endl;
    // At this stage, we have some classifications for each task,


//    DecisionTree<atoms> dt(it,
//                            en,
//                            3,
//                            selector,
//                            std::unordered_set<std::string>{},
//                           fields,
//                            ForTheWin::gain_measures::Entropy,
//                            0.9,
//                            1,
//                            1,
//                            1,
//                            false);
//    std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> map;
//    dt.print_rec(std::cout, 4);
//    dt.populate_children_predicates(map);
//    auto result1 = concrete_dt_table_from_class_result(map, fields, false);

//    for (const auto&x : V)
//        std::cout << x.first << "=" << classification_outcome<atoms>(result1, selector, x.first) << std::endl;

//    std::cout << std::endl << std::endl << std::endl;

//    DecisionTree<atoms> dt2(it2,
//                           en2,
//                           3,
//                           selector,
//                           std::unordered_set<std::string>{},
//                           fields,
//                           ForTheWin::gain_measures::Entropy,
//                           0.9,
//                           1,
//                           1,
//                           1,
//                           false);
//    std::unordered_map<int, std::vector<std::pair<double,std::vector<dt_predicate>>>> map2;
//    dt2.print_rec(std::cout, 4);
//    dt2.populate_children_predicates(map2);
//    auto result2 = concrete_dt_table_from_class_result(map2, fields, false);


}

#endif