//
// Created by giacomo on 24/02/2022.
//

#define CTEST_COLOR_OK
#define CTEST_SEGFAULT

#include <SimplifiedFuzzyStringMatching.h>
#include <gtest/gtest.h>

#define GLOBAL_TEST(name)     CTEST_DATA(name) {   };

class generic_test : public ::testing::Test {
protected:
    SimplifiedFuzzyStringMatching matcher;
    std::multimap<double, std::string> result;
    std::unordered_map<std::string, double> values;
};

class fsm : public generic_test {
protected:
    void SetUp() override {
        matcher.put("hello");
        matcher.put("bello");
        matcher.put("bel");
        matcher.put("hell");
        matcher.put("call");
        matcher.put("fall");
        matcher.put("tall");
        matcher.put("all");
        matcher.put("elementary");
        matcher.fuzzyMatch(0.0, 100, "fall", result);
        for (const auto& cp : result) {
            values.emplace(cp.second, cp.first);
        }
    }
};

class fsm_exact_match : public generic_test {
protected:
    void SetUp() override {
        matcher.put("hello");
        matcher.put("bello");
        matcher.put("bel");
        matcher.put("hell");
        matcher.put("call");
        matcher.put("fall");
        matcher.put("tall");
        matcher.put("all");
        matcher.put("elementary");
        matcher.fuzzyMatch(1.0, 1, "fall", result);
        for (const auto& cp : result) {
            values.emplace(cp.second, cp.first);
        }
    }
};


TEST_F(fsm_exact_match, only_one_match) {
    EXPECT_EQ(values.size(), 1);
}

TEST_F(fsm_exact_match, has_fall) {
    EXPECT_TRUE(values.contains("fall"));
}


TEST_F(fsm_exact_match, has_fall_top) {
    EXPECT_DOUBLE_EQ(values.find("fall")->second, 1.0);
}

TEST_F(fsm, data_not_matched) {
    EXPECT_TRUE(!(values.contains("elementary")));
}

TEST_F(fsm, match_order) {
    EXPECT_TRUE((values.at("fall")) > (values.at("all")));
    EXPECT_TRUE((values.at("all")) > (values.at("tall")));
    EXPECT_TRUE((values.at("all")) > (values.at("call")));
    EXPECT_DOUBLE_EQ((values.at("call")), (values.at("tall")));
    EXPECT_TRUE((values.at("call")) > (values.at("hell")));
    EXPECT_TRUE((values.at("hell")) > (values.at("hello")));
    EXPECT_DOUBLE_EQ((values.at("hello")), (values.at("bello")));
}