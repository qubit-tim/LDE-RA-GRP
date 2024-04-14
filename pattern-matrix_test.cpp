#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

#include <gtest/gtest.h>


TEST(PatternMatrixTest, PatternDefaultMatrixConstructor) {
    patternMatrix pm = patternMatrix();
    zmatrix pmz = zmatrix(6,6,3);
    zmatrix cz = zmatrix(6,6,1);
    EXPECT_EQ(pm.id, 0);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 0);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, -1);
    EXPECT_EQ(pm.subCaseMatch, -1);
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_EQ(pm.p, pmz);
    EXPECT_EQ(pm.pT, pmz);
    EXPECT_EQ(pm.swap23, pmz);
    EXPECT_EQ(pm.swap23T, pmz);
    EXPECT_EQ(pm.cV, cz);
    EXPECT_EQ(pm.cVT, cz);
}

TEST(PatternMatrixTest, PatternMatrixLoadFromString) {
    std::string tooFewRows = "[0 0,0 0,0 0,0 0,0 0,0 0]";
    std::string tooFewColumns = "[0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0]";
    std::string validPattern = "[0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 1,0 1,0 1,0 1,0 1,0 1][1 0,1 0,1 0,1 0,1 0,1 0][1 1,1 1,1 1,1 1,1 1,1 1]";
    std::string validNumericalPattern = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,1,1,1,1][2,2,2,2,2,2][3,3,3,3,3,3]";
    patternMatrix pm = patternMatrix();
    EXPECT_THROW(pm.loadFromString(""),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(tooFewRows),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(tooFewColumns),std::runtime_error);

    // TODO - add more tests to validate the pattern matrix is populated correctly for these cases
    EXPECT_NO_THROW(pm.loadFromString(validPattern));
    EXPECT_NO_THROW(pm.loadFromString(validNumericalPattern));
}

TEST(PatternMatrixTest,PatternMatrixStringConstructor) {
    patternMatrix pm = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    zmatrix pmz = zmatrix(6,6,3);
    zmatrix cz = zmatrix(6,6,1);
    EXPECT_EQ(pm.id, 1);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 0);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, -1);
    EXPECT_EQ(pm.subCaseMatch, -1);
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_EQ(pm.p, pmz);
    EXPECT_EQ(pm.pT, pmz);
    EXPECT_EQ(pm.swap23, pmz);
    EXPECT_EQ(pm.swap23T, pmz);
    EXPECT_EQ(pm.cV, cz);
    EXPECT_EQ(pm.cVT, cz);
}

TEST(PatternMatrixTest,PatternMatrixMatchedCase) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixMatchesCase) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixIsTranspose) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixIs23Swap) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixIs23SwapT) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixPrintDebug) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixLeftTGateMultiply) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixRightTGateMultiply) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixPrintMatchComparison) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixRearrangeMatrix) {
    FAIL() << "Not implemented";
}

TEST(PatternMatrixTest,PatternMatrixPatternElementAddition) {
    FAIL() << "Not implemented";
}
