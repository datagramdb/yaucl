//
// Created by giacomo on 24/02/2022.
//

#define CTEST_COLOR_OK
#define CTEST_SEGFAULT

#include <vector>
#include <yaucl/hashing/pair_hash.h>
#include <yaucl/hashing/uset_hash.h>
#include <yaucl/structures/set_operations.h>
#include <gtest/gtest.h>

class groupby : public ::testing::Test {
protected:
    void SetUp() override {
        W = {{1,2}, {3,0.5}, {3, 0.7}, {2, 0.5}, {1, 7}};
        std::sort(W.begin(), W.end());
    }

    std::vector <std::pair<size_t, double>> W;
};

TEST_F(groupby, groupby) {
    auto S = GroupByKeyExtractorIgnoreKey<std::vector<std::pair<size_t, double>>::iterator, size_t, std::pair<size_t, double>>(W.begin(), W.end(), [](const std::pair<size_t, double>& x) {return x.first; });
    EXPECT_EQ(S, (std::vector<std::vector<std::pair<size_t, double>>>{{{1,2},{1,7}}, {{2,0.5}}, {{3,0.5}, {3,0.7}}}));
}

TEST_F(groupby, cartesian_product) {
    auto S = GroupByKeyExtractorIgnoreKey<std::vector<std::pair<size_t, double>>::iterator, size_t, std::pair<size_t, double>>(W.begin(), W.end(), [](const std::pair<size_t, double>& x) {return x.first; });
    auto M = cartesian_product(S);
    EXPECT_EQ(M,
           (std::unordered_set<std::unordered_set<std::pair<size_t, double>>>{{{1,7},{2,0.5},{3,0.7}},
                                                                              {{1,7},{2,0.5},{3,0.5}},
                                                                              {{1,2},{2,0.5}, {3,0.7}},
                                                                              {{1,2}, {2,0.5}, {3,0.5}}}));
}