//
// Created by giacomo on 25/02/2022.
//

#define CTEST_COLOR_OK
#define CTEST_SEGFAULT

#include <vector>
#include <knobab/server/query_manager/Environment.h>
#include <gtest/gtest.h>
#include <knobab/server/operators/fast_ltlf_operators.h>


class basic_operators : public testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path curr = std::filesystem::current_path().parent_path().parent_path().parent_path().parent_path() / "data" / "testing" / "basic_operator_testing" / "log.txt";
        std::cout << curr.string() <<std::endl;
        std::ifstream file{curr};
        env.load_log(HUMAN_READABLE_YAUCL, true, curr.filename(), false, file);
    }

    Environment env;
};

class holidays_operators : public testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path curr = std::filesystem::current_path().parent_path().parent_path().parent_path().parent_path() / "data" / "testing" / "basic_operator_testing" / "holidays.txt";
        std::cout << curr.string() <<std::endl;
        std::ifstream file{curr};
        env.load_log(HUMAN_READABLE_YAUCL, true, curr.filename(), false, file);
    }

    Environment env;
};

class mdpi_operators : public testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path curr = std::filesystem::current_path().parent_path().parent_path().parent_path().parent_path() / "data" / "testing" / "declare" / "NegSuccession_test";
        std::cout << curr.string() <<std::endl;
        std::ifstream file{curr};
        env.load_log(TAB_SEPARATED_EVENTS, true, curr.filename(), false, file);
    }

    Environment env;
};

#define DATA_EMPLACE_BACK(l,trace,event,isMatch)    do { (l).emplace_back(std::make_pair((trace),(event)), std::make_pair((1.0),MarkedEventsVector{})); if (isMatch) (l).back().second.second.emplace_back(marked_event::activation(event));} while (false)
#define DATA_DECREMENT_EMPLACE_BACK(l,trace,event,isMatch)    do { (l).emplace_back(std::make_pair((trace),(event)-1), std::make_pair((1.0),MarkedEventsVector{})); if (isMatch) (l).back().second.second.emplace_back(marked_event::activation(event));} while (false)

TEST_F(mdpi_operators, ag) {
    auto a = env.db.timed_dataless_exists("a", ActivationLeaf);
    auto c = env.db.timed_dataless_exists("c", TargetLeaf);
    Result not_b, ag1, ag2, G;
    or_fast_timed(a, c, not_b, nullptr, env.db.act_table_by_act_id.trace_length);

    aAndGloballyB_timed_variant_2(a, not_b, ag1, nullptr,env.db.act_table_by_act_id.trace_length);
    aAndGloballyB_timed_variant_1(a, not_b, ag2, nullptr,env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(ag1, ag2);
}

TEST_F(mdpi_operators, ag2) {
    auto a = env.db.timed_dataless_exists("a", ActivationLeaf);
    auto b = env.db.timed_dataless_exists("b", TargetLeaf);
    Result not_b, ag1, ag2, G;

    aAndGloballyB_timed_variant_2(a, b, ag1, nullptr,env.db.act_table_by_act_id.trace_length);
    aAndGloballyB_timed_variant_1(a, b, ag2, nullptr,env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(ag1, ag2);
}

TEST_F(mdpi_operators, af) {
    auto a = env.db.timed_dataless_exists("a", ActivationLeaf);
    auto b = env.db.timed_dataless_exists("b", TargetLeaf);
    Result not_b, ag1, ag2, G;

    aAndFutureB_timed_variant_2(a, not_b, ag1, nullptr,env.db.act_table_by_act_id.trace_length);
    aAndFutureB_timed_variant_1(a, not_b, ag2, nullptr,env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(ag1, ag2);
}

TEST_F(holidays_operators, exists) {
    auto a = env.db.timed_dataless_exists("A", ActivationLeaf);
    auto b = env.db.timed_dataless_exists("B", TargetLeaf);
//    std::cout << a << std::endl;
//    std::cout << b << std::endl;
    Result result_slow_timed_b, result_slow_timed_a;
    Result and_slow_timed, andF_fast_timed, FaFB_result, FaFB_result_fast;

    future_logic_timed(b, result_slow_timed_b, env.db.act_table_by_act_id.trace_length);
    future_logic_timed(a, result_slow_timed_a, env.db.act_table_by_act_id.trace_length);

    and_fast_timed(a, result_slow_timed_b, and_slow_timed, nullptr, env.db.act_table_by_act_id.trace_length);
    aAndFutureB_timed_variant_1(a, b, andF_fast_timed, nullptr, env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(and_slow_timed, andF_fast_timed);

    and_logic_untimed(result_slow_timed_a, result_slow_timed_b, FaFB_result, nullptr, env.db.act_table_by_act_id.trace_length);
    and_fast_untimed(a, b, FaFB_result_fast,  nullptr, env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(FaFB_result, FaFB_result_fast);

    FaFB_result.clear();
    FaFB_result_fast.clear();
    or_logic_untimed(result_slow_timed_a, result_slow_timed_b, FaFB_result, nullptr, env.db.act_table_by_act_id.trace_length);
    or_fast_untimed(a, b, FaFB_result_fast,  nullptr, env.db.act_table_by_act_id.trace_length);
    EXPECT_EQ(FaFB_result, FaFB_result_fast);
}

TEST_F(basic_operators, A) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);
        auto resultA = env.db.timed_dataless_exists("A", value ? ActivationLeaf : NoneLeaf);

        Result A;
        DATA_EMPLACE_BACK(A, 0, 0, value);
        DATA_EMPLACE_BACK(A, 3, 0, value);
        DATA_EMPLACE_BACK(A, 4, 0, value);
        DATA_EMPLACE_BACK(A, 5, 0, value);
        DATA_EMPLACE_BACK(A, 6, 0, value);
        DATA_EMPLACE_BACK(A, 7, 0, value);
        DATA_EMPLACE_BACK(A, 7, 3, value);
        DATA_EMPLACE_BACK(A, 8, 0, value);
        DATA_EMPLACE_BACK(A, 8, 3, value);
        DATA_EMPLACE_BACK(A, 9, 0, value);
        DATA_EMPLACE_BACK(A, 9, 3, value);
        DATA_EMPLACE_BACK(A, 10, 0, value);
        DATA_EMPLACE_BACK(A, 10, 2, value);
        DATA_EMPLACE_BACK(A, 11, 0, value);
        DATA_EMPLACE_BACK(A, 11, 3, value);
        DATA_EMPLACE_BACK(A, 12, 0, value);
        DATA_EMPLACE_BACK(A, 12, 2, value);

        EXPECT_EQ(resultA, A);
    }
}

TEST_F(basic_operators, B) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);
        auto resultA = env.db.timed_dataless_exists("B", (i == 0) ? ActivationLeaf : NoneLeaf);

        Result A;
        DATA_EMPLACE_BACK(A, 1, 0, value);
        DATA_EMPLACE_BACK(A, 3, 1, value);
        DATA_EMPLACE_BACK(A, 5, 1, value);
        DATA_EMPLACE_BACK(A, 7, 1, value);
        DATA_EMPLACE_BACK(A, 7, 2, value);
        DATA_EMPLACE_BACK(A, 8, 2, value);
        DATA_EMPLACE_BACK(A, 9, 1, value);
        DATA_EMPLACE_BACK(A, 9, 2, value);
        DATA_EMPLACE_BACK(A, 9, 4, value);
        DATA_EMPLACE_BACK(A, 9, 5, value);
        DATA_EMPLACE_BACK(A, 10, 1, value);
        DATA_EMPLACE_BACK(A, 10, 3, value);
        DATA_EMPLACE_BACK(A, 10, 4, value);
        DATA_EMPLACE_BACK(A, 11, 1, value);
        DATA_EMPLACE_BACK(A, 11, 2, value);
        DATA_EMPLACE_BACK(A, 11, 4, value);
        DATA_EMPLACE_BACK(A, 12, 1, value);
        DATA_EMPLACE_BACK(A, 12, 3, value);

        EXPECT_EQ(resultA, A);
    }
}

TEST_F(basic_operators, C) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);
        auto resultA = env.db.timed_dataless_exists("C", (i == 0) ? ActivationLeaf : NoneLeaf);

        Result A;
        DATA_EMPLACE_BACK(A, 2, 0, value);
        DATA_EMPLACE_BACK(A, 4, 1, value);
        DATA_EMPLACE_BACK(A, 6, 1, value);
        DATA_EMPLACE_BACK(A, 8, 1, value);

        EXPECT_TRUE(resultA == A);
    }
}

TEST_F(basic_operators, init_act) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);

        {
            auto resultA = env.db.init("A", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 0, 0, value);
            DATA_EMPLACE_BACK(A, 3, 0, value);
            DATA_EMPLACE_BACK(A, 4, 0, value);
            DATA_EMPLACE_BACK(A, 5, 0, value);
            DATA_EMPLACE_BACK(A, 6, 0, value);
            DATA_EMPLACE_BACK(A, 7, 0, value);
            DATA_EMPLACE_BACK(A, 8, 0, value);
            DATA_EMPLACE_BACK(A, 9, 0, value);
            DATA_EMPLACE_BACK(A, 10, 0, value);
            DATA_EMPLACE_BACK(A, 11, 0, value);
            DATA_EMPLACE_BACK(A, 12, 0, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA = env.db.init("B", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 1, 0, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA = env.db.init("C", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 2, 0, value);
            EXPECT_EQ(resultA, A);
        }
    }
}

TEST_F(basic_operators, end_act) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);

        {
            auto resultA = env.db.ends("A", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 0, 0, value);
            DATA_EMPLACE_BACK(A, 7, 3, value);
            DATA_EMPLACE_BACK(A, 8, 3, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA =  env.db.ends("B", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 1, 0, value);
            DATA_EMPLACE_BACK(A, 3, 1, value);
            DATA_EMPLACE_BACK(A, 5, 1, value);
            DATA_EMPLACE_BACK(A, 9, 5, value);
            DATA_EMPLACE_BACK(A, 10, 4, value);
            DATA_EMPLACE_BACK(A, 11, 4, value);
            DATA_EMPLACE_BACK(A, 12, 3, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA =  env.db.ends("C", (i == 0) ? ActivationLeaf :  NoneLeaf);
            Result A;
            DATA_EMPLACE_BACK(A, 2, 0, value);
            DATA_EMPLACE_BACK(A, 4, 1, value);
            DATA_EMPLACE_BACK(A, 6, 1, value);
            EXPECT_EQ(resultA, A);
        }
    }
}

TEST_F(basic_operators, next_act) {
    for (int i = 0; i<2; i++) {
        bool value = (i == 0);

        {
            auto resultA = rnext(env.db.timed_dataless_exists("A", (i == 0) ? ActivationLeaf : NoneLeaf));
            Result A;
            DATA_DECREMENT_EMPLACE_BACK(A, 7, 3, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 8, 3, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 9, 3, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 10, 2, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 11, 3, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 12, 2, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA = rnext(env.db.timed_dataless_exists("B", (i == 0) ? ActivationLeaf : NoneLeaf));
            Result A;
            DATA_DECREMENT_EMPLACE_BACK(A, 3, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 5, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 7, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 7, 2, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 8, 2, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 9, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 9, 2, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 9, 4, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 9, 5, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 10, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 10, 3, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 10, 4, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 11, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 11, 2, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 11, 4, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 12, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 12, 3, value);
            EXPECT_EQ(resultA, A);
        }

        {
            auto resultA = rnext(env.db.timed_dataless_exists("C", (i == 0) ? ActivationLeaf : NoneLeaf));
            Result A;
            DATA_DECREMENT_EMPLACE_BACK(A, 4, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 6, 1, value);
            DATA_DECREMENT_EMPLACE_BACK(A, 8, 1, value);
            EXPECT_EQ(resultA, A);
        }
    }
}

TEST_F(basic_operators, no_multiple_labels) {
    {
// A and B shall never appear at the same time
        Result result;
        auto resultA = (env.db.timed_dataless_exists("A", ActivationLeaf));
        auto resultB = (env.db.timed_dataless_exists("B", TargetLeaf));
        setIntersection(resultA.begin(), resultA.end(), resultB.begin(), resultB.end(), std::back_inserter(result), Aggregators::maxSimilarity<double,double,double>, nullptr);
        EXPECT_TRUE(result.empty());

        Result alResult;
        and_logic_timed(resultA, resultB, alResult, nullptr);
        EXPECT_TRUE(alResult.empty());
    }

    {
// A and B shall never appear in the next time
        Result result;
        auto resultA = rnext(env.db.timed_dataless_exists("A", ActivationLeaf));
        auto resultB = rnext(env.db.timed_dataless_exists("B", TargetLeaf));
        setIntersection(resultA.begin(), resultA.end(), resultB.begin(), resultB.end(), std::back_inserter(result), Aggregators::maxSimilarity<double,double,double>, nullptr);
        EXPECT_TRUE(result.empty());

        Result alResult;
        and_logic_timed(resultA, resultB, alResult, nullptr);
        EXPECT_TRUE(alResult.empty());
    }

    {
// On the other hand, predicates should have an intersection
        auto higherRange = env.db.exact_range_query({"A", "x", -10, 2000});
        auto narrowRange = env.db.exact_range_query({"A", "x", 0, 2000});
        std::vector<std::pair<trace_t, event_t>> result;
        std::set_intersection(higherRange.begin(), higherRange.end(), narrowRange.begin(), narrowRange.end(), std::back_inserter(result));
        EXPECT_EQ(narrowRange, result);
    }

    {
// A and_{where x<y} next B
        Result result, alResult, expected;
        PredicateManager pm{{{{"x", "y", LT}}}, &env.db};
        auto resultA = (env.db.timed_dataless_exists("A", ActivationLeaf));
        auto resultB = rnext(env.db.timed_dataless_exists("B", TargetLeaf));
        setIntersection(resultA.begin(), resultA.end(), resultB.begin(), resultB.end(), std::back_inserter(result), Aggregators::maxSimilarity<double,double,double>, &pm);

        DATA_EMPLACE_BACK(expected, 3, 0, false);
        expected.back().second.second.emplace_back(marked_event::join(0,1));
        DATA_EMPLACE_BACK(expected, 5, 0, false);
        expected.back().second.second.emplace_back(marked_event::join(0,1));
        DATA_EMPLACE_BACK(expected, 7, 0, false);
        expected.back().second.second.emplace_back(marked_event::join(0,1));
        EXPECT_EQ(result, expected);

        and_logic_timed(resultA, resultB, alResult, &pm);
        EXPECT_EQ(alResult, expected);
    }
}

TEST_F(basic_operators, globally_untimed) {
    {
        auto result = global(env.db.timed_dataless_exists("A", ActivationLeaf), env.db.act_table_by_act_id.getTraceLengths());
        EXPECT_EQ(result, Result {ResultRecord (std::make_pair(0, 0), std::make_pair(1.0, MarkedEventsVector {marked_event::activation(0)}))});
    }
    {
        auto result = global(env.db.timed_dataless_exists("B", ActivationLeaf), env.db.act_table_by_act_id.getTraceLengths());
        EXPECT_EQ(result, Result {ResultRecord (std::make_pair(1, 0), std::make_pair(1.0, MarkedEventsVector{marked_event::activation(0)}))});
    }
    {
        auto result = global(env.db.timed_dataless_exists("C", ActivationLeaf), env.db.act_table_by_act_id.getTraceLengths());
        EXPECT_EQ(result, Result {ResultRecord (std::make_pair(2, 0), std::make_pair(1.0, MarkedEventsVector{marked_event::activation(0)}))});
    }
}

TEST_F(basic_operators, intersections_untimed_matches) {

    for (size_t i = 0; i<4; i++) {
        auto a = env.db.timed_dataless_exists("A", i / 2 ? ActivationLeaf : NoneLeaf),
                b = env.db.timed_dataless_exists("B", i % 2 ? TargetLeaf : NoneLeaf);
        Result result, resultLogic, resultFast;

        setIntersectionUntimed(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result),  Aggregators::maxSimilarity<double,double,double>, nullptr);
        std::unordered_set<uint32_t> traces{3,5,7,8,9,10,11,12};
        EXPECT_TRUE(!result.empty());
        for (const auto& ref : result)
            EXPECT_TRUE(traces.contains(ref.first.first));


        long long t1,t2;
        {
            auto start = std::chrono::high_resolution_clock::now();
            and_logic_untimed(a, b, resultLogic, nullptr);
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            t1 = std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
        }
        EXPECT_EQ(result, resultLogic);

        {
            auto start = std::chrono::high_resolution_clock::now();
            and_fast_untimed(a, b, resultFast, nullptr);
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            t2 = std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
        }
        EXPECT_EQ(result, resultFast);
        EXPECT_GE(t1,t2); // benchmark assumption
    }

}

TEST_F(basic_operators, unions_untimed_matches) {
    for (size_t i = 0; i<4; i++) {
        auto a = env.db.timed_dataless_exists("A", i / 2 ? ActivationLeaf : NoneLeaf),
                b = env.db.timed_dataless_exists("B", i % 2 ? TargetLeaf : NoneLeaf);
        Result result, resultLogic, resultFast;

        setUnionUntimed(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result),  Aggregators::maxSimilarity<double,double,double>, nullptr);
        std::unordered_set<uint32_t> traces{0,1,3,4,5,6,7,8,9,10,11,12};
        EXPECT_TRUE(!result.empty());
        for (const auto& ref : result)
            EXPECT_TRUE(traces.contains(ref.first.first));

        long long t1,t2;
        {
            auto start = std::chrono::high_resolution_clock::now();
            or_logic_untimed(a, b, resultLogic, nullptr);
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            t1 = std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
        }
        EXPECT_EQ(result, resultLogic);

        {
            auto start = std::chrono::high_resolution_clock::now();
            or_fast_untimed(a, b, resultFast, nullptr);
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            t2 = std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
        }
        EXPECT_EQ(result, resultFast);
//        EXPECT_GE(t1,t2); // benchmark assumption
    }

}


TEST_F(basic_operators, negatedUntimed) {
    std::unordered_map<std::string, Result> resultSets;
    resultSets["A"] = env.db.timed_dataless_exists("A", NoneLeaf);
    resultSets["B"] = env.db.timed_dataless_exists("B", NoneLeaf);
    resultSets["C"] = env.db.timed_dataless_exists("C", NoneLeaf);

    std::unordered_map<std::string, std::set<trace_t>> traceSets;
    for (const auto& x : resultSets["A"])
        traceSets["A"].insert(x.first.first);
    for (const auto& x : resultSets["B"])
        traceSets["B"].insert(x.first.first);
    for (const auto& x : resultSets["C"])
        traceSets["C"].insert(x.first.first);
    std::set<trace_t> universe;
    for (trace_t i = 0; i<env.db.act_table_by_act_id.trace_length.size(); i++)
        universe.insert(i);

    std::cout << " A = " <<  traceSets["A"] << std::endl;
    std::cout << " B = " <<  traceSets["B"] << std::endl;
    std::cout << " C = " <<  traceSets["C"] << std::endl;

    Result result;
    Result negated = negateUntimed(result, env.db.act_table_by_act_id.trace_length, true);
    EXPECT_EQ(negated.size(), env.db.noTraces);

    std::unordered_set<std::string> set{"A", "B", "C"};
    for (const auto& v : powerset(set)) {
        std::cout << v << std::endl;
        Result input;
        std::set<trace_t> inputTraces;
        if (!v.empty()) {
            auto it = v.begin();
            while (it != v.end()) {
                Result local;
                std::set<trace_t> local2;
                setUnion(input.begin(), input.end(), resultSets.at(*it).begin(), resultSets.at(*it).end(), std::back_inserter(local), Aggregators::maxSimilarity<double,double,double>);
                std::set_union(inputTraces.begin(), inputTraces.end(), traceSets.at(*it).begin(), traceSets.at(*it).end(), std::inserter(local2, local2.begin()));
                std::swap(local, input);
                std::swap(local2, inputTraces);
                it++;
            }
        }

        std::set<trace_t> expectedOutput, linearized;
        std::set_difference(universe.begin(), universe.end(), inputTraces.begin(), inputTraces.end(), std::inserter(expectedOutput, expectedOutput.begin()));

        auto res = negateUntimed(input, env.db.act_table_by_act_id.trace_length, false);
        for (const auto& ref : res) {
            linearized.insert(ref.first.first);
        }

        EXPECT_EQ(expectedOutput, linearized);
    }
}