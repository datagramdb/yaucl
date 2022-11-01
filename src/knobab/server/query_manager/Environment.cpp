//
// Created by giacomo on 16/04/2022.
//

#include <knobab/server/query_manager/Environment.h>

semantic_atom_set Environment::getSigmaAll() const {
    semantic_atom_set S = ap.act_atoms;
    for (const auto& ref : ap.interval_map) {
        std::pair<std::string, size_t> cp;
        cp.first = ref.first;
        for (size_t i = 0, N = ap.max_ctam_iteration.at(cp.first); i<N; i++) {
            cp.second = i;
            S.insert(ap.clause_to_atomization_map.at(cp));
        }
    }
    for (auto & ref : db.act_table_by_act_id.secondary_index) {
        auto ptr = ref.first;
        while (ptr) {
            auto lx = db.event_label_mapper.get(ptr->entry.id.parts.act);
            if (!ap.interval_map.contains(lx))
                S.insert(lx);
            ptr = ptr->next;
        };
    }
    return S;
}

void Environment::clear() {
    db.clear();
    clearModel();
}

#include <filesystem>
#include <yaucl/graphs/algorithms/minimizeDFA.h>
#include <yaucl/graphs/graph_join_pm_conversion.h>
#include <yaucl/functional/assert.h>


void Environment::load_model(const std::filesystem::path &model_file) {
    throw std::runtime_error("ERROR: stale method");
#if 0
    conjunctive_model.clear();
    if (!std::filesystem::exists((model_file))) {
        std::cerr << "ERROR: model file does not exist: " << model_file << std::endl;
        exit(1);
    }
    std::ifstream file{model_file};
    conjunctive_model = dmp.load(file, true);
    experiment_logger.model_parsing_ms = dmp.parsing_time_ms;
    experiment_logger.model_size = conjunctive_model.size();
    experiment_logger.model_filename = model_file;
#endif
}

void Environment::load_model(const std::string &model) {
    throw std::runtime_error("ERROR: stale method");
#if 0
    conjunctive_model.clear();
    std::stringstream file{model};
    conjunctive_model = dmp.load(file, true);
    experiment_logger.model_parsing_ms = dmp.parsing_time_ms;
    experiment_logger.model_size = conjunctive_model.size();
    experiment_logger.model_filename = model;
#endif
}

void Environment::load_log(log_data_format format, bool loadData, const std::string &filename, bool setMaximumStrLen,
                           std::istream &input_stream) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    experiment_logger.log_filename = filename;

    {
        //log_data_format format, bool loadData, std::istream &stream, KnowledgeBase &output,
        //                              std::string &filename
        auto t1 = high_resolution_clock::now();
        load_into_knowledge_base(format, loadData, input_stream, db, filename);
        auto t2 = high_resolution_clock::now();

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        experiment_logger.log_loading_and_parsing_ms = ms_double.count();
        //std::cout << "Loading and parsing time = " << ms_double.count() << std::endl;
    }

    {
        auto t1 = high_resolution_clock::now();
        db.index_data_structures(index_missing_data);
        auto t2 = high_resolution_clock::now();

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        experiment_logger.log_indexing_ms = ms_double.count();
        //std::cout << "Indexing time = " << ms_double.count() << std::endl;
    }
    if (setMaximumStrLen) {
        auto tmp = db.getMaximumStringLength();
        ap.s_max = std::string(tmp, std::numeric_limits<char>::max());
        DataPredicate::MAX_STRING = ap.s_max;
        DataPredicate::msl = tmp;
    }


    experiment_logger.n_traces = db.noTraces;
    experiment_logger.n_acts = db.actId;
    // Compute some more trace statistics

    double trace_avg, trace_pow2, N;
    N = db.act_table_by_act_id.trace_length.size();
    size_t frequency_of_trace_length = 0;
    size_t previousLength = 0;
    std::multiset<size_t> O;

    if (doStats) {
        for (const size_t i : db.act_table_by_act_id.trace_length) {
            trace_avg += i;
            trace_pow2 += std::pow(i, 2);
            O.insert(i);
        }
        for (size_t len : O) {
            size_t currFreq = O.count(len);
            if (currFreq > frequency_of_trace_length) {
                frequency_of_trace_length = currFreq;
                previousLength = len;
            }
        }
        trace_avg = trace_avg / N;

        experiment_logger.log_trace_average_length = trace_avg;
        experiment_logger.log_trace_variance = (trace_pow2 / N) - std::pow(trace_avg, 2);
        experiment_logger.most_frequent_trace_length = previousLength;
        experiment_logger.trace_length_frequency = frequency_of_trace_length;
    }
}

void Environment::set_atomization_parameters(const std::string &fresh_atom_label, size_t mslength, AtomizationStrategy strategy) {
    ap.fresh_atom_label = fresh_atom_label;
    ap.s_max = std::string(mslength, std::numeric_limits<char>::max());
    ap.strategy = strategy;
    DataPredicate::MAX_STRING = ap.s_max;
    DataPredicate::msl = mslength;
}

void
Environment::set_grounding_parameters(bool doPreliminaryFill, bool ignoreActForAttributes, bool creamOffSingleValues,
                                      GroundingStrategyConf::pruning_strategy ps) {
    grounding_conf.doPreliminaryFill = doPreliminaryFill;
    grounding_conf.ignoreActForAttributes = ignoreActForAttributes;
    grounding_conf.creamOffSingleValues = creamOffSingleValues;
    grounding_conf.strategy1 = ps;
}

void Environment::init_atomize_tables() {
    ap.clear();
    experiment_logger.model_data_decomposition_time = collect_data_from_declare_disjunctive_model(db.event_label_mapper, ap, grounding);
}

void Environment::doGrounding() {
    grounding = GroundWhereStrategy(grounding_conf,
                                    db,
                                    conjunctive_model);
}

void Environment::print_model(std::ostream &os) const {
    os << "Declarative Model: " << std::endl;
    os << "----------------------------------------" << std::endl;
    for (const auto& def : conjunctive_model)
        os << def << std::endl;

    os << "----------------------------------------" << std::endl;
    os << std::endl;
}

void Environment::print_grounded_model(std::ostream &os) const {
    os << "Grounded Model: " << std::endl;
    os << "----------------------------------------" << std::endl;
    os << grounding << std::endl;
    os << "----------------------------------------" << std::endl;
    os << std::endl;
}

void Environment::print_knowledge_base(std::ostream &os) const {
    os << "Knowledge Base: " << std::endl;
    os << "----------------------------------------" << std::endl;
    db.reconstruct_trace_with_data(os);
    os << "----------------------------------------" << std::endl;
    os << std::endl;
}

void Environment::print_grounding_tables(std::ostream &os) {
    os << "Grounding Tables: " << std::endl;
    os << "----------------------------------------" << std::endl;
    os << ap << std::endl;
    os << "----------------------------------------" << std::endl;
    os << std::endl;
}

void Environment::first_atomize_model() {
    experiment_logger.model_atomization_time = atomize_model(ap, grounding);
}



semantic_atom_set Environment::evaluate_easy_prop_to_atoms(const easy_prop &prop,
        //const std::unordered_map<std::string, std::string> &bogus_act_to_atom,
                                                           const std::unordered_map<std::string, semantic_atom_set> &bogus_act_to_set) {
    return ::evaluate_easy_prop_to_atoms(prop, bogus_act_to_set, getSigmaAll());
}




TemplateCollectResult Environment::compute_declare_for_conjunctive(bool doPrune) {
    std::vector<graph_join_pm> distinct_graph_model;
    graph_join_pm              joined_graph_model;
    size_t M = grounding.singleElementOfConjunction.size();
    return {};
#if 0
    if (M == 0) {
        return {};
    } else {
        auto& zeroModel = grounding.singleElementOfConjunction.at(0);
        ///assert(allTemplates.contains(std::make_pair(zeroModel.casusu, zeroModel.n)));
        ///assert(allTemplates[std::make_pair(zeroModel.casusu, zeroModel.n)].contains(zeroModel));
        graph_join_pm currGraph;
        compute_declare_for_disjunctive(zeroModel, currGraph); // // = template_to_graph.at(zeroModel);
        TemplateCollectResult result;
        conditionalPruningGraph(doPrune, true, result, currGraph);
        /**{
            auto g = convert_to_dfa_graph(currGraph).makeDFAAsInTheory(getSigmaAll());
            {
                std::ofstream output_el_model{std::to_string(0)+"_test1.g"};
                dot(currGraph, output_el_model, false);
            }
            {
                std::ofstream output_el_model{std::to_string(0)+"_test2.g"};
                g.dot(output_el_model);
            }

            // TODO: merge sink un-accepting nodes in makeDFAAsInTheory
            auto DFA = minimizeDFA(g);
        }*/
        for (size_t j = 1; j<M; j++) {
            auto& zeroModelJ = grounding.singleElementOfConjunction.at(j);
            ///assert(allTemplates.contains(std::make_pair(zeroModelJ.casusu, zeroModelJ.n)));
            ///assert(allTemplates[std::make_pair(zeroModelJ.casusu, zeroModelJ.n)].contains(zeroModelJ));
            graph_join_pm currGraph2;
            compute_declare_for_disjunctive(zeroModelJ, currGraph2);
            ///auto& currGraph2 = template_to_graph.at(zeroModelJ);
            conditionalPruningGraph(doPrune, false, result, currGraph2);
            {
                ///auto g = convert_to_dfa_graph(currGraph2).makeDFAAsInTheory(getSigmaAll());

                /**std::ofstream output_el_model{std::to_string(j)+"_testNOW.g"};
                g.dot(output_el_model);*/

                // TODO: merge sink un-accepting nodes in makeDFAAsInTheory
                ///auto DFA = minimizeDFA(g);

                //dot(currGraph2, output_el_model, false);
                //DFA.dot(output_el_model);
            }
            {
                ///auto g = convert_to_dfa_graph(result.joined_graph_model).makeDFAAsInTheory(getSigmaAll());

                // TODO: merge sink un-accepting nodes in makeDFAAsInTheory
                ///auto DFA = minimizeDFA(g);

                ///std::ofstream output_el_model{std::to_string(j)+"_jointmp.g"};
                //dot(result.joined_graph_model, output_el_model, false);
                //DFA.dot(output_el_model);
            }
        }
        /*if (doPrune) {
            graph_join_pm result_;
            remove_unaccepting_states(result.joined_graph_model, result_);
            result.joined_graph_model = result_;
        }*/
        return result;
    }
#endif
}

void Environment::print_count_table(std::ostream &os) const {
    db.print_count_table(os);
}

void Environment::print_act_table(std::ostream &os) const {
    db.print_act_table(os);
}

void Environment::print_attribute_tables(std::ostream &os) const {
    db.print_attribute_tables(os);
}

void Environment::cache_declare_templates_as_graphs() {
    DEBUG_ASSERT(false);
#if 0
    for (declare_templates t : magic_enum::enum_values<declare_templates>()) {
        ///std::cout << "INIT: " << magic_enum::enum_name(t) << std::endl;
        if (isUnaryPredicate(t)) {
            for (size_t i = 1; i<3; i++) {
                declare_to_graph.getDeclareTemplate(t, i);
            }
        } else {
            declare_to_graph.getDeclareTemplate(t, 2);
        }
    }
#endif
}


//#include <httplib.h>

void Environment::server() {

#if 0
    using namespace httplib;
    Server svr;

    // Representing the count table as a csv file, with headers
    svr.Get("/count_table.csv", [this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        db.print_count_table(ss);
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/count.html", [this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/count_table.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });

    // Representing the ACT Table
    svr.Get("/act_table.csv", [this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        db.print_act_table(ss);
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/act.html", [this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/act_table.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });

    // Returning the attribute tables
    svr.Get("/att_table_names.csv",[this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        for (auto it = db.attribute_name_to_table.begin(), en = db.attribute_name_to_table.end(); it != en; ) {
            ss << it->first;
            it++;
            if (it != en) ss << std::endl;
        }
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/att.csv",[this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        auto it = db.attribute_name_to_table.find(req.get_param_value("f",0));
        if (it != db.attribute_name_to_table.end())
            ss << it->second;
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/atts.html",[this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/att_tables.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });

    svr.Get("/quit", [&svr](const httplib::Request& req, httplib::Response& res) {
        svr.stop();
    });
    svr.listen("localhost", 8080);
#endif
}

void Environment::server(MAXSatPipeline& pipeline) {

#if 0
    using namespace httplib;
    Server svr;

    // Representing the count table as a csv file, with headers
    svr.Get("/count_table.csv", [this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        db.print_count_table(ss);
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/count.html", [this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/count_table.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });

    // Representing the ACT Table
    svr.Get("/act_table.csv", [this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        db.print_act_table(ss);
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/act.html", [this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/act_table.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });

    // Returning the attribute tables
    svr.Get("/att_table_names.csv",[this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        for (auto it = db.attribute_name_to_table.begin(), en = db.attribute_name_to_table.end(); it != en; ) {
            ss << it->first;
            it++;
            if (it != en) ss << std::endl;
        }
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/att.csv",[this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        auto it = db.attribute_name_to_table.find(req.get_param_value("f",0));
        if (it != db.attribute_name_to_table.end())
            ss << it->second;
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/atts.html",[this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/att_tables.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });


    svr.Get("/query_plan.json",[this, &pipeline](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        ss << pipeline.generateGraph();
        res.set_content(ss.str(), "text/json");
    });
    svr.Get("/graph.html",[this](const httplib::Request& req, httplib::Response& res) {
        std::ifstream t("client/graph.html");
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        std::string buffer(size, ' ');
        t.seekg(0);
        t.read(&buffer[0], size);
        res.set_content(buffer, "text/html");
    });
    svr.Get("/pipeline_data.csv",[this](const httplib::Request& req, httplib::Response& res) {
        std::stringstream ss;
        auto it = std::stoull(req.get_param_value("f",0));
        if (it != 0) {
            ss << "TraceId,EventId,Sim,{Events}" << std::endl;
            for (const auto& ref : ((LTLfQuery*)   it)->result) {
                ss << ref.first.first << "," << ref.first.second << "," << ref.second << ",{TODO}" << std::endl;
            }
        }
        res.set_content(ss.str(), "text/csv");
    });
    svr.Get("/quit", [&svr](const httplib::Request& req, httplib::Response& res) {
        svr.stop();
    });
    svr.listen("localhost", 8080);
#endif
}

void Environment::set_grounding_parameters(const std::string &grounding_strategy) {
    GroundingStrategyConf::pruning_strategy ps = GroundingStrategyConf::ALWAYS_EXPAND_LESS_TOTAL_VALUES;
    size_t msl = 10;
    bool doPreliminaryFill = true;
    bool ignoreActForAttributes = false;
    bool creamOffSingleValues = true;

    set_grounding_parameters(doPreliminaryFill,
                             ignoreActForAttributes,
                             creamOffSingleValues,
                             ps);
#if 0
    if (std::filesystem::exists(std::filesystem::path(grounding_strategy))) {
        std::cout << "Loading the grounding_conf strategy configuration file: " << grounding_strategy << std::endl;
        YAML::Node n = YAML::LoadFile(grounding_strategy);

        if (n["strategy"]) {
            auto x = n["strategy"].Scalar();
            auto v = magic_enum::enum_cast<GroundingStrategyConf::pruning_strategy>(x);
            if (v.has_value()) {
                ps = v.value();
            }
        }

        if (n["doPreliminaryFill"]) {
            auto x = n["doPreliminaryFill"].Scalar();
            doPreliminaryFill = (x == "1") || (x == "T") || (x == "true");
        }

        if (n["ignoreActForAttributes"]) {
            auto x = n["ignoreActForAttributes"].Scalar();
            ignoreActForAttributes = (x == "1") || (x == "T") || (x == "true");
        }

        if (n["creamOffSingleValues"]) {
            auto x = n["creamOffSingleValues"].Scalar();
            creamOffSingleValues = (x == "1") || (x == "T") || (x == "true");
        }

        set_grounding_parameters(doPreliminaryFill,
                                 ignoreActForAttributes,
                                 creamOffSingleValues,
                                 ps);
    }
#endif
}

void Environment::set_maxsat_parameters(const std::filesystem::path &atomization_conf) {
    script_for_decomposition = "scripts/logic_plan.queryplan";
    preferred_plan = "efficient";
    experiment_logger.no_threads = noThreads = 1;
    operators = AbidingLogic;

#if 0
    if (std::filesystem::exists((atomization_conf))) {
        experiment_logger.atomization_conf = atomization_conf;
        std::cout << "Loading the atomization configuration file: " << atomization_conf << std::endl;
        YAML::Node n = YAML::LoadFile((atomization_conf).string());
        if (n["script_plan"]) {
            experiment_logger.queries_plan = preferred_plan = n["script_plan"].Scalar();
        }
        if (n["threads"]) {
            experiment_logger.no_threads = noThreads = n["threads"].as<size_t>();
        }
        if (n["ensemble"]) {
            strategy = magic_enum::enum_cast<EnsembleMethods>(n["ensemble"].Scalar()).value_or(strategy);
        }
        if (n["operators"]) {
            experiment_logger.operators_version = n["operators"].Scalar();
            operators = magic_enum::enum_cast<OperatorQueryPlan>(experiment_logger.operators_version).value_or(operators);
        }
        if (n["decomposition_script"]) {
            script_for_decomposition = n["decomposition_script"].Scalar();
        }
    }
#endif
}

void Environment::set_atomization_parameters(const std::filesystem::path &atomization_conf) {
    std::string fresh_atom_label{"p"};
    size_t msl = 10;
    AtomizationStrategy strategy = AtomizeEverythingIfAnyDataPredicate;

    set_atomization_parameters(fresh_atom_label, msl, strategy);
#if 0
    if (std::filesystem::exists((atomization_conf))) {
        std::cout << "Loading the atomization configuration file: " << atomization_conf << std::endl;
        YAML::Node n = YAML::LoadFile((atomization_conf).string());
        if (n["fresh_atom_label"]) {
            fresh_atom_label = n["fresh_atom_label"].Scalar();
        }
        if (n["MAXIMUM_STRING_LENGTH"]) {
            msl = n["MAXIMUM_STRING_LENGTH"].as<size_t>();
        }
        if (n["strategy"]) {
            strategy = magic_enum::enum_cast<AtomizationStrategy>(n["strategy"].Scalar()).value_or(strategy);
        }
        set_atomization_parameters(fresh_atom_label, msl, strategy);
    }
#endif
}

MAXSatPipeline Environment::query_model() {
    throw std::runtime_error("stale method");
    return {nullptr, 0};
#if 0

    MAXSatPipeline maxsat_pipeline(script_for_decomposition, preferred_plan, noThreads);
    maxsat_pipeline.final_ensemble = strategy;
    maxsat_pipeline.operators = operators;
    maxsat_pipeline.pipeline(&grounding, ap, db);
    experiment_logger.model_declare_to_ltlf = maxsat_pipeline.declare_to_ltlf_time;
    experiment_logger.model_ltlf_query_time = maxsat_pipeline.ltlf_query_time;
#ifdef MAXSatPipeline_PARALLEL
    experiment_logger.is_multithreaded = true;
        experiment_logger.no_threads = noThreads;
#else
    experiment_logger.is_multithreaded = false;
    experiment_logger.no_threads = 1;
#endif
    return maxsat_pipeline;
#endif
}

void Environment::load_log(log_data_format format, bool loadData, const std::string &filename, bool setMaximumStrLen) {
    std::ifstream f{filename};
    load_log(format, loadData, filename, setMaximumStrLen,f);
}

void Environment::dump_log_for_sqlminer(const std::string &basicString) {
    namespace fs = std::filesystem;
    fs::path parent = basicString;
    if (!fs::is_directory(parent) || !fs::exists(parent)) { // Check if src folder exists
        fs::create_directories(parent); // create src folder
    }

    std::ofstream log_table{parent / "log.tab"},
            schema{parent / "payloadschema.tab"},
            payload{parent / "payload.tab"};

    db.dump_for_sqlminer(log_table, payload, schema);
}

void Environment::clearModel() {
    grounding.singleElementOfConjunction.clear();
    ap.clear();
    conjunctive_model.clear();
}

void Environment::set_maxsat_parameters(size_t nThreads, EnsembleMethods e, OperatorQueryPlan ops) {
    script_for_decomposition = "scripts/logic_plan.queryplan";
    preferred_plan = "efficient";
    experiment_logger.no_threads = noThreads = nThreads;
    operators = AbidingLogic;
    experiment_logger.no_threads = noThreads = nThreads;
    strategy = e;
    operators = ops;
}

