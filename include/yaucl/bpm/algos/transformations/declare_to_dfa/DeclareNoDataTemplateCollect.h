//
// Created by giacomo on 26/07/21.
//

#ifndef BPM21_TEMPLATECOLLECT_H
#define BPM21_TEMPLATECOLLECT_H

#include <unordered_map>
#include <yaucl/bpm/structures/declare/DeclareDataAware.h>
#include <yaucl/graphs/graph_join_pm.h>
//#include <graphs/third-party-wrappers/FLLOATScriptRunner.h>
#include <yaucl/bpm/algos/transformations/declare_to_dfa/TemplateCollectResult.h>
#include <filesystem>
#include <sstream>
#include <magic_enum.hpp>
#include <fstream>
#include <yaucl/functional/assert.h>
//#include <yaucl/bpm/structures/ltlf/ltlf.h>


graph_join_pm ReplaceABWithProperLabels(const std::string& dot,
                                        std::unordered_map<std::string, std::string> *ptr,
                                        const std::unordered_set<std::string>& SigmaAll);

/**
 * Given a set of declare templates, it generates it, and initializes the non-data aware representation of the
 * original ones. In fact, please bear in mind that each dataaware template should be already atomized into a
 * non data-aware representation
 */

struct [[deprecated]] DeclareNoDataTemplateCollect {
private:
    std::unordered_map<std::pair<declare_templates, size_t>,
            std::unordered_set<DeclareDataAware>> allTemplates;
    std::unordered_map<std::pair<declare_templates, size_t>, graph_join_pm> graph_map;
    std::unordered_map<DeclareDataAware, graph_join_pm> template_to_graph;
    bool isAdding;
    ///FLLOATScriptRunner pyscript;
    std::filesystem::path base_serialization_path;
    bool doPrune;
public:

    /**
     *
     * @param base_serialization_path   Folder where to serialize the intermediate graphs
     */
    DeclareNoDataTemplateCollect(bool doPrune = true, const std::string& base_serialization_path = ".");

    bool isPruningAllowed() const  { return doPrune; }

    /**
     * Adds a template to the current collector
     * @param left
     */
    void add(const DeclareDataAware& left);

    /**
     * Given a subset of the templates provided in the add, generates a model with graphs from it
     *
     * @param Model     Model defined as a conjunction of templates
     * @return          Single graphs associated to the model, as well as the joined graphs
     */
    TemplateCollectResult buildUpModel(const std::vector<DeclareDataAware>& Model);
};


#endif //BPM21_TEMPLATECOLLECT_H
