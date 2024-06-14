#include "zmatrix.hpp"
#include "case-matrix.hpp"

#include <gtest/gtest.h>

std::string TOO_FEW_COLUMNS = "[1,1,0,0][1,1,0,0][0,0,0,0][0,0,0,0][0,0,0,0][0,0,0,0]";
std::string TOO_MANY_COLUMNS = "[1,1,0,0,0,0,0][1,1,0,0,0,0,0][0,0,0,0,0,0,0][0,0,0,0,0,0,0][0,0,0,0,0,0,0][0,0,0,0,0,0,0]";
std::string TOO_FEW_ROWS = "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";
std::string TOO_MANY_ROWS = "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";

std::string CASE1 = "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";
std::string CASE2 = "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";

TEST(caseMatrixTest, ZeroedCaseMatrixConstructor) {
    EXPECT_THROW(caseMatrix(0, ""), std::runtime_error);
    EXPECT_THROW(caseMatrix(0, TOO_FEW_COLUMNS), std::runtime_error);
    EXPECT_THROW(caseMatrix(0, TOO_MANY_COLUMNS), std::runtime_error);
    EXPECT_THROW(caseMatrix(0, TOO_FEW_ROWS), std::runtime_error);
    EXPECT_THROW(caseMatrix(0, TOO_MANY_ROWS), std::runtime_error);
    // Test with a valid and symmetric case
    caseMatrix c1 = caseMatrix(1, CASE1);
    EXPECT_EQ(c1.id, 1);
    EXPECT_TRUE(c1.c == c1.cT);
    EXPECT_TRUE(c1.c.strictMatch(c1.cT));
    // Test with a valid and non-symmetric case
    caseMatrix c2 = caseMatrix(2, CASE2);
    EXPECT_EQ(c2.id, 2);
    EXPECT_FALSE(c2.c == c2.cT);
    EXPECT_FALSE(c2.c.strictMatch(c2.cT));
    // Make sure c1 and c2 are not equal
    EXPECT_FALSE(c1.c == c2.c);
    EXPECT_FALSE(c1.cT == c2.cT);
}
