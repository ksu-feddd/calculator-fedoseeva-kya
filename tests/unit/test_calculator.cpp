#include <gtest/gtest.h>
#include "../../src/calculator.h"

TEST(CalculatorTest, AdditionInt) {
    EXPECT_EQ(add_int(2U, 3U), 5U);
    EXPECT_EQ(add_int(0U, 0U), 0U);
    EXPECT_EQ(add_int(10U, 20U), 30U);
}

TEST(CalculatorTest, AdditionFloat) {
    EXPECT_DOUBLE_EQ(add_float(2.5, 3.7), 6.2);
    EXPECT_DOUBLE_EQ(add_float(0.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(add_float(1.1, 2.2), 3.3);
}
