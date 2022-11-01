package it.giacomobergami.knobab.records;

public class LoggerInformation {
    String log_filename;
    boolean with_data;
    double log_loading_and_parsing_ms = -1.0;
    double log_indexing_ms = -1.0;
    long n_traces = 0;
    long n_acts = 0;
    long no_threads;
    double log_trace_average_length;
    double log_trace_variance;
    long most_frequent_trace_length;
    long trace_length_frequency;
    String atomization_conf;
    String model_filename;
    String queries_plan;
    String operators_version;
    double model_parsing_ms = -1.0;
    long model_size = 0;
    double model_data_decomposition_time = -1.0;
    double model_atomization_time = -1.0;
    double model_declare_to_ltlf = -1.0;
    double model_ltlf_query_time = -1.0;
    boolean is_multithreaded = false;

    public String getLog_filename() {
        return log_filename;
    }

    public void setLog_filename(String log_filename) {
        this.log_filename = log_filename;
    }

    public boolean isWith_data() {
        return with_data;
    }

    public void setWith_data(boolean with_data) {
        this.with_data = with_data;
    }

    public double getLog_loading_and_parsing_ms() {
        return log_loading_and_parsing_ms;
    }

    public void setLog_loading_and_parsing_ms(double log_loading_and_parsing_ms) {
        this.log_loading_and_parsing_ms = log_loading_and_parsing_ms;
    }

    public double getLog_indexing_ms() {
        return log_indexing_ms;
    }

    public void setLog_indexing_ms(double log_indexing_ms) {
        this.log_indexing_ms = log_indexing_ms;
    }

    public long getN_traces() {
        return n_traces;
    }

    public void setN_traces(long n_traces) {
        this.n_traces = n_traces;
    }

    public long getN_acts() {
        return n_acts;
    }

    public void setN_acts(long n_acts) {
        this.n_acts = n_acts;
    }

    public long getNo_threads() {
        return no_threads;
    }

    public void setNo_threads(long no_threads) {
        this.no_threads = no_threads;
    }

    public double getLog_trace_average_length() {
        return log_trace_average_length;
    }

    public void setLog_trace_average_length(double log_trace_average_length) {
        this.log_trace_average_length = log_trace_average_length;
    }

    public double getLog_trace_variance() {
        return log_trace_variance;
    }

    public void setLog_trace_variance(double log_trace_variance) {
        this.log_trace_variance = log_trace_variance;
    }

    public long getMost_frequent_trace_length() {
        return most_frequent_trace_length;
    }

    public void setMost_frequent_trace_length(long most_frequent_trace_length) {
        this.most_frequent_trace_length = most_frequent_trace_length;
    }

    public long getTrace_length_frequency() {
        return trace_length_frequency;
    }

    public void setTrace_length_frequency(long trace_length_frequency) {
        this.trace_length_frequency = trace_length_frequency;
    }

    public String getAtomization_conf() {
        return atomization_conf;
    }

    public void setAtomization_conf(String atomization_conf) {
        this.atomization_conf = atomization_conf;
    }

    public String getModel_filename() {
        return model_filename;
    }

    public void setModel_filename(String model_filename) {
        this.model_filename = model_filename;
    }

    public String getQueries_plan() {
        return queries_plan;
    }

    public void setQueries_plan(String queries_plan) {
        this.queries_plan = queries_plan;
    }

    public String getOperators_version() {
        return operators_version;
    }

    public void setOperators_version(String operators_version) {
        this.operators_version = operators_version;
    }

    public double getModel_parsing_ms() {
        return model_parsing_ms;
    }

    public void setModel_parsing_ms(double model_parsing_ms) {
        this.model_parsing_ms = model_parsing_ms;
    }

    public long getModel_size() {
        return model_size;
    }

    public void setModel_size(long model_size) {
        this.model_size = model_size;
    }

    public double getModel_data_decomposition_time() {
        return model_data_decomposition_time;
    }

    public void setModel_data_decomposition_time(double model_data_decomposition_time) {
        this.model_data_decomposition_time = model_data_decomposition_time;
    }

    public double getModel_atomization_time() {
        return model_atomization_time;
    }

    public void setModel_atomization_time(double model_atomization_time) {
        this.model_atomization_time = model_atomization_time;
    }

    public double getModel_declare_to_ltlf() {
        return model_declare_to_ltlf;
    }

    public void setModel_declare_to_ltlf(double model_declare_to_ltlf) {
        this.model_declare_to_ltlf = model_declare_to_ltlf;
    }

    public double getModel_ltlf_query_time() {
        return model_ltlf_query_time;
    }

    public void setModel_ltlf_query_time(double model_ltlf_query_time) {
        this.model_ltlf_query_time = model_ltlf_query_time;
    }

    public boolean isIs_multithreaded() {
        return is_multithreaded;
    }

    public void setIs_multithreaded(boolean is_multithreaded) {
        this.is_multithreaded = is_multithreaded;
    }
}
