//
// Created by sam on 11/10/22.
//
#include <iostream>
#include <filesystem>
#include "knobab/Environment.h"
#include "knobab/operators/simple_ltlf_operators.h"
#include "knobab/operators/fast_ltlf_operators.h"
#include <fstream>

const std::string file_path = "data/testing/results/benchmarking/custom_operators2.csv";

Environment setup(const std::filesystem::path f){
    Environment env;
    std::ifstream file{f};
    env.load_log(TAB_SEPARATED_EVENTS, true, f.filename(), false, file);
    return env;
}


void write_to_file(const Environment& env, const std::string& test_name, const std::string& test_type, const float& time){
    const std::filesystem::path F(file_path);
    const bool& write_header = !std::filesystem::exists(F);
    std::ofstream outF{file_path, std::ios_base::app};
    if (write_header){
        outF << "Test" << ","
             << "Type" << ","
             << "Log_Size" << ","
             << "Max_Trace_Length" << ","
             << "Alphabet" << ","
             << "Time" << std::endl;
    }

    std::string alphabet;
    for (const auto &piece : env.db.event_label_mapper.int_to_T) alphabet += piece;

    outF << test_name << ","
         << test_type << ","
         << env.db.noTraces<< ","
         << *max_element(std::begin(env.db.act_table_by_act_id.trace_length), std::end(env.db.act_table_by_act_id.trace_length)) << ","
         << alphabet << ","
         << time << std::endl;
}

void test_or(const Environment& environment){
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_or_fast_timed, r_or_logic_timed;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    t1 = std::chrono::high_resolution_clock::now();
    or_fast_untimed(a, b, r_or_fast_timed, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> or_fast_untimed_time = t2 - t1;
    write_to_file(environment, "Or", "fast", or_fast_untimed_time.count());

    t1 = std::chrono::high_resolution_clock::now();
    or_logic_untimed(a, b, r_or_logic_timed, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> or_logic_untimed_time = t2 - t1;
    write_to_file(environment, "Or", "logic", or_logic_untimed_time.count());
}

void test_and(const Environment& environment) {
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_and_fast_timed, r_and_logic_timed;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    t1 = std::chrono::high_resolution_clock::now();
    and_fast_untimed(a, b, r_and_logic_timed, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> and_fast_untimed_time = t2 - t1;
    write_to_file(environment, "And", "fast", and_fast_untimed_time.count());

    t1 = std::chrono::high_resolution_clock::now();
    and_logic_untimed(a, b, r_and_fast_timed, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> and_logic_untimed_time = t2 - t1;
    write_to_file(environment, "And", "logic", and_logic_untimed_time.count());
}

void test_choice(const Environment& environment) {
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_future_logic_a, r_future_logic_b, r_future_logic_a_or_future_logic_b, r_a_or_b;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    t1 = std::chrono::high_resolution_clock::now();
    or_fast_untimed(a, b, r_a_or_b, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_or_b_time = t2 - t1;
    write_to_file(environment, "Choice", "fast", r_a_or_b_time.count());

    t1 = std::chrono::high_resolution_clock::now();
    future_logic_untimed(a, r_future_logic_a, environment.db.act_table_by_act_id.trace_length);
    future_logic_untimed(b, r_future_logic_b, environment.db.act_table_by_act_id.trace_length);
    or_fast_untimed(a, b, r_future_logic_a_or_future_logic_b, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_future_logic_a_or_future_logic_b_time = t2 - t1;
    write_to_file(environment, "Choice", "logic", r_future_logic_a_or_future_logic_b_time.count());
}

void test_aAndFutureb(const Environment& environment){
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_aAndfb, r_future_t_logic_b, r_future_t_a_and_t_future_t_b;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    // A&Ft_new(B)
    t1 = std::chrono::high_resolution_clock::now();
    aAndFutureB_timed_variant_2(a, b, r_aAndfb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_aAndfb_time = t2 - t1;
    write_to_file(environment, "And_Future", "fast", r_aAndfb_time.count());

    // A&Ft_old(B)
    t1 = std::chrono::high_resolution_clock::now();
    aAndFutureB_timed_variant_1(a, b, r_aAndfb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_aAndfb_old_time = t2 - t1;
    write_to_file(environment, "And_Future", "fast_old", r_aAndfb_old_time.count());

    // A AND Ft(B)
    t1 = std::chrono::high_resolution_clock::now();
    future_logic_timed(b, r_future_t_logic_b, environment.db.act_table_by_act_id.trace_length);
    and_fast_timed(a, r_future_t_logic_b, r_future_t_a_and_t_future_t_b, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_future_t_a_and_t_future_t_b_time = t2 - t1;
    write_to_file(environment, "And_Future", "logic", r_future_t_a_and_t_future_t_b_time.count());
}

void test_aAndGloballyb(const Environment& environment){
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_aAndgb, r_global_logic_b, r_future_t_a_and_t_globally_t_b;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    // A&G_new(B)
    t1 = std::chrono::high_resolution_clock::now();
    aAndGloballyB_timed_variant_2(a, b, r_aAndgb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_and_globally_b_ut_time = t2 - t1;
    write_to_file(environment, "And_Globally", "fast", r_a_and_globally_b_ut_time.count());

    // A&G_old(B)
    t1 = std::chrono::high_resolution_clock::now();
    aAndGloballyB_timed_variant_1(a, b, r_aAndgb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_and_globally_b_ut_time2 = t2 - t1;
    write_to_file(environment, "And_Globally", "fast_old", r_a_and_globally_b_ut_time2.count());

    // A AND Gt(B)
    t1 = std::chrono::high_resolution_clock::now();
    global_fast_timed(b, r_global_logic_b, environment.db.act_table_by_act_id.trace_length);
    and_fast_timed(a, r_global_logic_b, r_future_t_a_and_t_globally_t_b, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_and_t_globally_t_b_time = t2 - t1;
    write_to_file(environment, "And_Globally", "logic", r_a_and_t_globally_t_b_time.count());
}

void test_aAndNextGloballyb(const Environment& environment){
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_aAndxgb, r_global_logic_b, r_next_logic_global_logic_b, r_a_and_t_next_t_globally_t_b;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    // A&XG(B)
    t1 = std::chrono::high_resolution_clock::now();
    aAndNextGloballyB_timed(a, b, r_aAndxgb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_and_next_globally_b_ut_time = t2 - t1;
    write_to_file(environment, "And_Next_Globally", "fast", r_a_and_next_globally_b_ut_time.count());

    // A AND XtGt(B)
    t1 = std::chrono::high_resolution_clock::now();
    global_logic_timed(b, r_global_logic_b, environment.db.act_table_by_act_id.trace_length);
    next_logical(r_global_logic_b, r_next_logic_global_logic_b, environment.db.act_table_by_act_id.trace_length);
    and_fast_timed(a, r_next_logic_global_logic_b, r_a_and_t_next_t_globally_t_b, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_a_and_t_next_t_globally_t_b_time = t2 - t1;
    write_to_file(environment, "And_Next_Globally", "logic", r_a_and_t_next_t_globally_t_b_time.count());
}

void test_until(const Environment& environment){
    const Result& a = environment.db.timed_dataless_exists("A", ActivationLeaf);
    const Result& b = environment.db.timed_dataless_exists("B", TargetLeaf);

    Result r_aufb, r_aulb;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    // A U B fast
    t1 = std::chrono::high_resolution_clock::now();
    until_fast_untimed(a, b, r_aufb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_aufb_time = t2 - t1;
    write_to_file(environment, "Until", "fast", r_aufb_time.count());

    // A U B
    t1 = std::chrono::high_resolution_clock::now();
    until_logic_untimed(a, b, r_aulb, nullptr, environment.db.act_table_by_act_id.trace_length);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> r_aulb_time = t2 - t1;
    write_to_file(environment, "Until", "logic", r_aulb_time.count());
}

int main(){
    const uint16_t iters = 5;

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("data/benchmarking/completed")){
        if (dirEntry.path().string().ends_with(".zip")) continue;
        std::cout << dirEntry << std::endl;
        std::filesystem::path curr = (dirEntry);
        Environment environment = setup(curr);

        for(uint16_t i = 0; i < iters; ++i){
            test_and(environment);
            test_or(environment);
            test_aAndFutureb(environment);
            test_aAndGloballyb(environment);
            test_aAndNextGloballyb(environment);
            test_until(environment);
            test_choice(environment);
        }
    }
}