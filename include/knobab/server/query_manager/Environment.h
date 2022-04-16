//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_ENVIRONMENT_H
#define KNOBAB_SERVER_ENVIRONMENT_H

#include <filesystem>
#include <yaucl/functional/assert.h>
#include <knobab/server/tables/KnowledgeBase.h>
#include <knobab/server/algorithms/grounding/kb_grounding.h>
#include <yaucl/bpm/structures/commons/easy_prop.h>
#include <yaucl/bpm/structures/log/data_loader.h>
#include <yaucl/bpm/algos/transformations/declare_to_dfa/TemplateCollectResult.h>
#include <knobab/server/dataStructures/atomization_structures.h>
#include <knobab/server/algorithms/atomization/AtomizingPipeline.h>
#include <knobab/server/benchmarking/LoggerInformation.h>
#include <knobab/server/algorithms/querymanager/MAXSatPipeline.h>
#include <fstream>

inline
semantic_atom_set evaluate_easy_prop_to_atoms(const easy_prop &prop,
                                              const std::unordered_map<std::string, semantic_atom_set> &bogus_act_to_set,
                                              const std::unordered_set<std::string>& sigma) {
    switch (prop.casusu) {
        case easy_prop::E_P_AND:
            DEBUG_ASSERT(prop.args.size() == 2);
            DEBUG_ASSERT(!prop.isAtomNegated);
            return unordered_intersection(evaluate_easy_prop_to_atoms( prop.args.at(0)/*, bogus_act_to_atom*/, bogus_act_to_set, sigma),
                                          evaluate_easy_prop_to_atoms( prop.args.at(1)/*, bogus_act_to_atom*/, bogus_act_to_set, sigma) );
        case easy_prop::E_P_OR: {
            DEBUG_ASSERT(prop.args.size() == 2);
            DEBUG_ASSERT(!prop.isAtomNegated);
            semantic_atom_set S = evaluate_easy_prop_to_atoms( prop.args.at(0)/*, bogus_act_to_atom*/, bogus_act_to_set, sigma);
            auto tmp = evaluate_easy_prop_to_atoms( prop.args.at(1)/*, bogus_act_to_atom*/, bogus_act_to_set, sigma);
            S.insert(tmp.begin(), tmp.end());
            return S;
        }
        case easy_prop::E_P_ATOM:
            DEBUG_ASSERT(prop.args.empty());
            DEBUG_ASSERT(bogus_act_to_set.contains(prop.single_atom_if_any));
            if (prop.isAtomNegated) {
                return unordered_difference(sigma, bogus_act_to_set.at(prop.single_atom_if_any));
            } else {
                return bogus_act_to_set.at(prop.single_atom_if_any);
            }
        case easy_prop::E_P_TRUE:
            return sigma;
        default: //case easy_prop::E_P_FALSE:
            return {};
    }
}

struct Environment {
    /// Creating an instance of the knowledge base, that is going to store all the traces in the log!

    bool doStats = true;
    KnowledgeBase db;
    AtomizingPipeline ap;
    GroundingStrategyConf grounding_conf;
    CNFDeclareDataAware grounding;
    std::vector<DeclareDataAware> conjunctive_model;

    EnsembleMethods strategy = PerDeclareSupport;
    OperatorQueryPlan operators = AbidingLogic;
    std::string script_for_decomposition;
    std::string preferred_plan;
    size_t noThreads;

    //std::unordered_map<DeclareDataAware, FlexibleFA<size_t, std::string>> pattern_graph;

    void server(MAXSatPipeline& pipeline);
    void server();

    void dump_log_for_sqlminer(const std::string &basicString);

public:

    size_t getTraceNo() const {
        return db.noTraces;
    }

    LoggerInformation experiment_logger;
    double min_threshold = 1.0;
    double c = 2.0;
    bool   index_missing_data = false;

    /// Data Range Queries
    std::vector<std::pair<std::pair<trace_t, event_t>, double>> range_query(DataPredicate prop) const {
        return db.range_query(prop, min_threshold, c);
    }

    std::vector<DeclareDataAware> generateTopBinaryClauses(const std::string& template_name, size_t topN = 0, const std::string& writeFile = "") {
        auto n = db.doActCounting();
        std::vector<std::string> cpy;
        std::vector<DeclareDataAware> toGen;
        size_t max = std::ceil(std::sqrt(n.size())), count = 0;

        if(max < topN){
            topN = max;
        }

        for (auto it = n.rbegin(), en = n.rend(); it != en; it++) {
            if (count >= topN) break;
            cpy.emplace_back(it->second);
            count++;
        }

        for (const auto& x : cpy) {
            for (const auto& y : cpy) {
                toGen.emplace_back(DeclareDataAware::binary_for_testing(template_name, x, y));
            }
        }
        if(!writeFile.empty()){
            std::ofstream outF(writeFile, std::ofstream::trunc);
            for(const DeclareDataAware& dec : toGen){
                outF <<  dec.left_act << "\t" << dec.right_act << std::endl;
            }
            outF.close();
        }

        std::cout << toGen << std::endl;
        return toGen;
    }

    void exact_range_query(const std::vector<DataQuery>& Q) {
        std::vector<std::pair<DataQuery, PartialResult>> Qs;
        std::unordered_map<std::string, std::vector<size_t>> actToPredId;
        for (const auto& q : Q)
            Qs.emplace_back(q, PartialResult{});

    }

    void exact_range_query(const std::string &var,
                           const std::unordered_map<std::string, std::vector<size_t>> &actToPredId,
                           std::vector<std::pair<DataQuery, PartialResult>> &Qs,
                           const std::optional<uint16_t> &temporalTimeMatch = {},
                           double approxConstant = 1.0) const {
        db.exact_range_query(var, actToPredId, Qs, temporalTimeMatch, approxConstant);
    }

    Environment() {}

    /**
     * Returns the atoms associated to both the declare model and the current knowledge base!
     * @return
     */
    semantic_atom_set  getSigmaAll() const;

    /**
     * Performs a query over the setted model. The whole pipeline is returned as a result (e.g., debugging and server)
     */
    MAXSatPipeline query_model();

    /**
     * Clears all of the bits and pieces, thus preparing into a novel test
     */
    void clear();

    void clearModel();


    /**
     * Loads the data traces from the log
     * @param format        Format of the log
     * @param loadData      Whether to load the trace and event payloads
     * @param filename      Filename associated to the log
     * @param setMaximumStrLen Whether to exploit the knowledge base information to infer the maximum string length
     */
    void load_log(log_data_format format, bool loadData, const std::string &filename, bool setMaximumStrLen,
                  std::istream &input_stream);
    void load_log(log_data_format format, bool loadData, const std::string &filename, bool setMaximumStrLen);


    void cache_declare_templates_as_graphs();

    /**
     * Loading the Declare model in the Extended format
     * @param model_file
     */
    void load_model(const std::filesystem::path &model_file);
    void load_model(const std::string &model);
    template <typename T> void load_model(T begin, T end, const std::string& generatedName = "Generated") {
        conjunctive_model.clear();
        for (auto it = begin; it != end; it++) {
            conjunctive_model.emplace_back(*it);
        }
        experiment_logger.model_parsing_ms = 0;
        experiment_logger.model_size = conjunctive_model.size();
        experiment_logger.model_filename = generatedName;
    }

    void set_atomization_parameters(const std::filesystem::path& atomization_conf);
    void set_atomization_parameters(const std::string &fresh_atom_label = "p",
                                    size_t mslength = MAXIMUM_STRING_LENGTH,
                                    AtomizationStrategy strategy = AtomizeEverythingIfAnyDataPredicate);

    void set_grounding_parameters(const std::string& grounding_strategy);
    void set_grounding_parameters(bool doPreliminaryFill = true,
                                  bool ignoreActForAttributes = false,
                                  bool creamOffSingleValues = true,
                                  GroundingStrategyConf::pruning_strategy ps = GroundingStrategyConf::ALWAYS_EXPAND_LESS_TOTAL_VALUES
    );

    void set_maxsat_parameters(const std::filesystem::path &atomization_conf);

    /**
     * Extracting the atomization tables from the recently loaded declare model
     */
    void init_atomize_tables();

    /**
     * Grounding the Declare model containing join clauses
     */
    void doGrounding();

    /**
     * Associating to both the premise and the consequence of the declare statement the numerical
     * atoms associated to them
     */
    void first_atomize_model();

    /**
     * Represents the declare pattern inot a graph over which the graph conjunctive join can be run efficiently
     * @param decl          Declare statement
     * @param out_result    Resulting Merged/Union graph
     */
    void compute_declare_to_graph_for_joins(const DeclareDataAware& decl, graph_join_pm &out_result);

    /**
     * Given the atomized model, it generates the final conjunctive graph out of it
     * @param doPrune
     * @return      Joined graph, with some intermediate operands
     */
    TemplateCollectResult compute_declare_for_conjunctive(bool doRemoveUnacceptingStates = false);

    /**
     * Given a propositional formula stating the actions allowed by the formula, where some bogus atom
     * identify a placeholder for other atoms, replaces those with the semantic ones, and uses the
     * environment information for computing the difference (given the universe set, sigmaAll).
     *
     * @param prop
     * @param bogus_act_to_set
     * @return
     */
    semantic_atom_set evaluate_easy_prop_to_atoms(const easy_prop &prop,
                                                  const std::unordered_map<std::string, semantic_atom_set>& bogus_act_to_set);

    void print_model(std::ostream& os) const ;
    void print_grounded_model(std::ostream& os) const;
    void print_knowledge_base(std::ostream& os) const;
    void print_count_table(std::ostream& os) const;
    void print_act_table(std::ostream& os) const;
    void print_attribute_tables(std::ostream& os) const;
    void print_grounding_tables(std::ostream& os);

private:
    /**
     * Generating the disjunction among the different possible graphs, by generating the union graph.
     * Such union graphs creates a DFA, where no node is a \varepsilon label, and all of the nodes
     * are clustered by label and collected numbers
     * @param decl          Disjunction of clauses
     * @param out_result    Output graph join graph
     */
    void compute_declare_for_disjunctive(const DisjunctiveDeclareDataAware& decl, graph_join_pm &out_result);

};


#endif //KNOBAB_SERVER_ENVIRONMENT_H
