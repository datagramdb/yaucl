#include <gtest/gtest.h>

class SquareTest :  public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(SquareTest /*test suite name*/, PosZeroNeg /*test name*/) {
//    EXPECT_EQ (9.0, (3.0*2.0)); // fail, test continues
    EXPECT_EQ (0.0, (0.0));     // success
//    ASSERT_EQ (9, (3)*(-3.0));  // fail, test interrupts
//    ASSERT_EQ (-9, (-3)*(-3.0));// not executed due to the previous assert

    EXPECT_TRUE(true);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}