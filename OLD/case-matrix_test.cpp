#include "zmatrix.hpp"
#include "case-matrix.hpp"

#include <gtest/gtest.h>

TEST(caseMatrixTest, ZeroedCaseMatrixConstructor) {
    caseMatrix c = caseMatrix(0, ""); // This should probably fail, but it doesn't
    EXPECT_EQ(c.id, 0);
    EXPECT_EQ(c.c.zSum, 0);
    EXPECT_EQ(c.cT.zSum, 0);
}
