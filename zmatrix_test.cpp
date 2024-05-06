#include "zmatrix.hpp"

#include <gtest/gtest.h>

TEST(ZMatrixTest, DefaultZMatrixConstructor) {
    zmatrix z = zmatrix();
    EXPECT_EQ(z.zSum, 0);
}