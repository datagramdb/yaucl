//
// Created by giacomo on 16/04/2022.
//

#include <knobab/server/benchmarking/LoggerInformation.h>


void LoggerInformation::clear() {
    log_filename.clear();
    model_ltlf_query_time = model_declare_to_ltlf = log_loading_and_parsing_ms = model_atomization_time = model_data_decomposition_time = log_indexing_ms = model_parsing_ms = log_trace_average_length = log_trace_variance = -1.0;
    model_size = n_traces = n_acts = most_frequent_trace_length  = no_threads = 0;
    model_filename.clear();
    queries_plan.clear();
    operators_version.clear();
    atomization_conf.clear();
    is_multithreaded = with_data = false;
}

std::ostream& operator<<(std::ostream& os, const LoggerInformation &information) {
    os << "Log Filename: " << information.log_filename << std::endl <<  "\t - Loading and parsing (ms): "
       << information.log_loading_and_parsing_ms  << std::endl <<  "\t - Indexing time (ms): " << information.log_indexing_ms
       << std::endl <<  "\t - #traces: "
       << information.n_traces  << std::endl <<  "\t - #acts: " << information.n_acts  << std::endl <<
       "\t - Avg. Trace Length: "
       << information.log_trace_average_length  << std::endl <<  "\t - Trace Variance: " << information.log_trace_variance
       << std::endl << "\t - Most Frequent Trace Length: " << information.most_frequent_trace_length
       << std::endl <<   "\t - Most Frequent Trace Length's Frequency: "
       << information.trace_length_frequency << std::endl << "\t - Atomisation Conf  " << information.atomization_conf << std::endl << "Model Filename: " << information.model_filename
       << std::endl <<"\t - Queries Plan  " << information.queries_plan << std::endl << "\t - Operators Version  " << information.operators_version << std::endl <<"\t - Parsing (ms) " << information.model_parsing_ms << std::endl << "\t - #clauses: " << information.model_size
       << std::endl << "\t - Data/Interval Decomposition (ms): " << information.model_data_decomposition_time << std::endl <<"\t - Declare Atomization (ms): "
       << information.model_atomization_time << std::endl << "\t - Declare->LTLf Transformation (ms):  " << information.model_declare_to_ltlf
       << std::endl << "\t - Actual Query Time (ms):  " << information.model_ltlf_query_time << std::endl
       << "Is Multithreaded: " << (information.is_multithreaded ? "Yes" : "No") << std::endl
       << "\t - #Threads: " << information.no_threads;
    return os;
}

void LoggerInformation::log_csv_file(std::ostream &csv_log) const {
    csv_log << log_filename << ","
            << (with_data ? "T," : "F,")
            << log_loading_and_parsing_ms  << ","
            << log_indexing_ms  << ","
            << n_traces  << ","
            << n_acts  << ","
            << log_trace_average_length  << ","
            << log_trace_variance << ","
            << most_frequent_trace_length << ","
            << trace_length_frequency << ","
            << atomization_conf << ","
            << model_filename<< ","
            << queries_plan<< ","
            << operators_version<< ","
            << model_parsing_ms << ","
            << model_size<< ","
            << model_data_decomposition_time << ","
            << model_atomization_time << ","
            << model_declare_to_ltlf<< ","
            << model_ltlf_query_time << ","
            << (is_multithreaded ? "1" : "No") << ","
            << no_threads << std::endl;
}

void LoggerInformation::log_csv_file_header(std::ostream &csv_log) {
    csv_log << "log_filename" << ","
            << "with_data" << ","
            << "log_loading_and_parsing_ms"  << ","
            << "log_indexing_ms"  << ","
            << "n_traces"  << ","
            << "n_acts"  << ","
            << "log_trace_average_length"  << ","
            << "log_trace_variance" << ","
            << "most_frequent_trace_length" << ","
            << "trace_length_frequency" << ","
            << "atomization_conf" << ","
            << "model_filename" << ","
            << "queries_plan" << ","
            << "operators_version" << ","
            << "model_parsing_ms" << ","
            << "model_size" << ","
            << "model_data_decomposition_time" << ","
            << "model_atomization_time" << ","
            << "model_declare_to_ltlf" << ","
            << "model_ltlf_query_time" << ","
            << ("is_multithreaded") << ","
            << "no_threads" << std::endl;
}

#include <nlohmann/json.hpp>


void LoggerInformation::log_json_file(nlohmann::json& object) const {
    object["log_filename"] = log_filename;
    object["with_data"] = with_data;
    object["log_loading_and_parsing_ms"] = log_loading_and_parsing_ms;
    object["log_indexing_ms"] = log_indexing_ms;
    object["n_traces"] = n_traces;
    object["n_acts"] = n_acts;
    object["log_trace_average_length"] = log_trace_average_length;
    object["log_trace_variance"] = log_trace_variance;
    object["most_frequent_trace_length"] = most_frequent_trace_length;
    object["trace_length_frequency"] = trace_length_frequency;
    object["atomization_conf"] = atomization_conf;
    object["model_filename"] = model_filename;
    object["queries_plan"] = queries_plan;
    object["operators_version"] = operators_version;
    object["model_parsing_ms"] = model_parsing_ms;
    object["model_size"] = model_size;
    object["model_data_decomposition_time"] = model_data_decomposition_time;
    object["model_atomization_time"] = model_atomization_time;
    object["model_declare_to_ltlf"] = model_declare_to_ltlf;
    object["model_ltlf_query_time"] = model_ltlf_query_time;
    object["is_multithreaded"] = is_multithreaded;
    object["no_threads"] = no_threads;
}

void LoggerInformation::log_json_file(std::ostream& json_log) const {
    nlohmann::json object;
    log_json_file(object);
    json_log << object.dump();
}