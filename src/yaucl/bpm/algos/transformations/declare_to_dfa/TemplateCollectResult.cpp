//
// Created by giacomo on 26/07/21.
//

#include "yaucl/bpm/algos/transformations/declare_to_dfa/DeclareNoDataTemplateCollect.h"
#include <magic_enum.hpp>
#include <yaucl/graphs/graph_join_pm_conversion.h>

void conditionalPruningGraph(bool doPrune, bool firstInsertion, TemplateCollectResult& result, graph_join_pm& currGraph) {
    if (!doPrune) {
        result.distinct_graph_model.emplace_back(currGraph);
        if (firstInsertion) {
            result.joined_graph_model = currGraph;
        } else {
            result.joined_graph_model = graph_join(result.joined_graph_model, currGraph);
        }
    } else {
        graph_join_pm result_;
        //remove_unaccepting_states(currGraph, result_);
        result.distinct_graph_model.emplace_back(result_);
        if (firstInsertion) {
            result.joined_graph_model = result_;
        } else {
            result.joined_graph_model = graph_join(result.joined_graph_model, result_);
        }
    }
}




#include <fstream>
//#include <yaucl/graphs/automata_join.h>

/*
void DeclareNoDataTemplateCollect::run(const std::unordered_set<std::string> &SigmaAll) {
    if (!isAdding) return;
    for (const auto& template_cp : allTemplates) {
        std::pair<declare_templates, size_t> template_ = template_cp.first;
        std::string_view w = magic_enum::enum_name(template_.first);
        std::string name{w.data(), w.size()};
        name = name + "_" + std::to_string(template_.second) ;
        ltlf input_forumla;

        std::vector<DeclareDataAware>                             Sequentialised{template_cp.second.begin(), template_cp.second.end()};
        std::vector<std::unordered_map<std::string, std::string>> VM;
        std::vector<std::string>                                  Names;
        std::unordered_set<std::string> tmpSigmaAll{"a"};

        switch (template_.first) {
            case Existence:
            case Absence:
            case Absence2:
            case Exactly:
            case End:
            case Init:
                input_forumla = toFiniteSemantics(template_.first, template_.second, "a");
                for (const auto& template_actual: Sequentialised) {
                    std::unordered_map<std::string, std::string> M;
                    M["a"] = template_actual.left_act;
                    VM.emplace_back(M);
                    Names.emplace_back(name+"["+template_actual.left_act+"]");
                }
                break;

            default:
                input_forumla = toFiniteSemantics(template_.first, template_.second, "a", "b");
                for (const auto& template_actual: Sequentialised) {
                    std::unordered_map<std::string, std::string> M;
                    M["a"] = template_actual.left_act;
                    M["b"] = template_actual.right_act;
                    Names.emplace_back(name+"["+template_actual.left_act+", "+template_actual.right_act+"]");
                    VM.emplace_back(M);
                }
                tmpSigmaAll.insert("b");
                break;
        }


        std::cout << name << std::endl;
        parseToGraph(graph_map[template_], input_forumla, pyscript, (base_serialization_path/ (name+".ltlf")).string(), nullptr, tmpSigmaAll);
        {
            std::ofstream file{(base_serialization_path/ (name+".dot")).string()};
            dot(graph_map.at(template_), file, false);
        }
        for (size_t i = 0, N = Sequentialised.size(); i<N; i++) {
            auto str = Names.at(i);
            std::cout << "\t * " << str << std::endl;
            auto& ref = template_to_graph[Sequentialised.at(i)];
            parseToGraph(ref, input_forumla, pyscript, (base_serialization_path / (str + ".ltlf")).string(), &VM[i], SigmaAll);
            std::ofstream file{(base_serialization_path/str).string()};
            dot(ref, file, false);
        }
    }
    isAdding = false;
}

#include <yaucl/graphs/graph_join_pm_algorithms.h>

static inline void conditionalPruningGraph(bool doPrune, bool firstInsertion, TemplateCollectResult& result, graph_join_pm& currGraph) {
    if (!doPrune) {
        result.distinct_graph_model.emplace_back(currGraph);
        if (firstInsertion) {
            result.joined_graph_model = currGraph;
        } else {
            result.joined_graph_model = graph_join(result.joined_graph_model, currGraph);
        }
    } else {
        graph_join_pm result_;
        remove_unaccepting_states(currGraph, result_);
        result.distinct_graph_model.emplace_back(result_);
        if (firstInsertion) {
            result.joined_graph_model = result_;
        } else {
            result.joined_graph_model = graph_join(result.joined_graph_model, result_);
        }
    }
}

TemplateCollectResult DeclareNoDataTemplateCollect::buildUpModel(const std::vector<DeclareDataAware> &Model) {
    std::vector<graph_join_pm> distinct_graph_model;
    graph_join_pm              joined_graph_model;
    size_t M = Model.size();
    if (M == 0) {
        return {};
    } else {
        auto& zeroModel = Model.at(0);
        assert(allTemplates.contains(std::make_pair(zeroModel.casusu, zeroModel.n)));
        assert(allTemplates[std::make_pair(zeroModel.casusu, zeroModel.n)].contains(zeroModel));
        auto& currGraph = template_to_graph.at(zeroModel);
        TemplateCollectResult result;
        conditionalPruningGraph(doPrune, true, result, currGraph);
        for (size_t j = 1; j<M; j++) {
            auto& zeroModelJ = Model.at(j);
            assert(allTemplates.contains(std::make_pair(zeroModelJ.casusu, zeroModelJ.n)));
            assert(allTemplates[std::make_pair(zeroModelJ.casusu, zeroModelJ.n)].contains(zeroModelJ));
            auto& currGraph2 = template_to_graph.at(zeroModelJ);
            conditionalPruningGraph(doPrune, false, result, currGraph2);
        }
        if (doPrune) {
            graph_join_pm result_;
            remove_unaccepting_states(result.joined_graph_model, result_);
            result.joined_graph_model = result_;
        }
        return result;
    }
}
*/
void TemplateCollectResult::minimize() {
    graph_join_pm result;
    ::minimize(joined_graph_model, result);
    joined_graph_model = result;
}
