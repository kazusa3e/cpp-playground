#include <gtest/gtest.h>

#include <kuri/core.hpp>

TEST(CoreTest, AddFunction) {
    EXPECT_EQ(kuri::add(2, 3), 5);
    EXPECT_EQ(kuri::add(-1, 1), 0);
    EXPECT_EQ(kuri::add(0, 0), 0);
}
