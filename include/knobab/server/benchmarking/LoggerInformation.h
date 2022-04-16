//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_LOGGERINFORMATION_H
#define KNOBAB_SERVER_LOGGERINFORMATION_H


#include <string>
#include <yaucl/structures/default_constructors.h>
#include <ostream>

struct LoggerInformation {
    std::string log_filename;
    bool with_data;
    double log_loading_and_parsing_ms = -1.0;
    double log_indexing_ms = -1.0;
    size_t n_traces = 0;
    size_t n_acts = 0;
    size_t no_threads;
    double log_trace_average_length;
    double log_trace_variance;
    size_t most_frequent_trace_length;
    size_t trace_length_frequency;

    std::string atomization_conf;

    std::string model_filename;
    std::string queries_plan;
    std::string operators_version;
    double model_parsing_ms = -1.0;
    size_t model_size = 0;
    double model_data_decomposition_time = -1.0;
    double model_atomization_time = -1.0;
    double model_declare_to_ltlf = -1.0;
    double model_ltlf_query_time = -1.0;

#ifdef MAXSatPipeline_PARALLEL
    bool is_multithreaded = true;
#else
    bool is_multithreaded = false;
#endif

    DEFAULT_CONSTRUCTORS(LoggerInformation)

    void clear();

    friend std::ostream& operator<<(std::ostream& os, const LoggerInformation &information);
    void log_csv_file_header(std::ostream &csv_log);
    void log_csv_file(std::ostream& csv_log);
};



#endif //KNOBAB_SERVER_LOGGERINFORMATION_H
