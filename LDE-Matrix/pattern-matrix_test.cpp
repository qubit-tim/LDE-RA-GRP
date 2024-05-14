#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

#include <gtest/gtest.h>

std::string TOO_FEW_ROWS = "[0 0,0 0,0 0,0 0,0 0,0 0]";
std::string TOO_FEW_COLUMNS = "[0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0]";
std::string VALID_BINARY_PATTERN = "[0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 1,0 1,0 1,0 1,0 1,0 1][1 0,1 0,1 0,1 0,1 0,1 0][1 1,1 1,1 1,1 1,1 1,1 1]";
std::string VALID_NUMERICAL_PATTERN = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,1,1,1,1][2,2,2,2,2,2][3,3,3,3,3,3]";
std::string ALL_ZEROS_PATTERN = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";

std::map <int, std::vector<std::string>> CASE_TO_VALID_PATTERN_MAP = {
    {
        0,
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1]",
        }
    },
    {
        1, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0]",
        }
    },
    {
        2, // "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,0,0][3,2,0,0,0,0][2,2,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,3,0,0,3][0,0,0,0,0,0][0,0,2,0,0,2][0,0,0,0,0,0][0,0,3,0,0,2][0,0,2,0,0,3]",
            // transpose cases
            "[2,2,2,2,0,0][2,2,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0][3,0,2,0,3,2][0,0,0,0,0,0]",
        }
    },
    {
        3, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        4,  // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        5, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        6, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        7, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        8, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
        }
    },
};


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
    patternMatrix pm = patternMatrix();
    EXPECT_THROW(pm.loadFromString(""),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(TOO_FEW_ROWS),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(TOO_FEW_COLUMNS),std::runtime_error);

    // TODO - add more tests to validate the pattern matrix is populated correctly for these cases
    EXPECT_NO_THROW(pm.loadFromString(VALID_BINARY_PATTERN));
    EXPECT_NO_THROW(pm.loadFromString(VALID_NUMERICAL_PATTERN));
}

TEST(PatternMatrixTest,PatternMatrixStringConstructor) {
    patternMatrix pm = patternMatrix(1, ALL_ZEROS_PATTERN);
    zmatrix pmz = zmatrix(6,6,3);
    pmz.updateMetadata();
    zmatrix cz = zmatrix(6,6,1);
    cz.updateMetadata();
    EXPECT_EQ(pm.id, 1);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 0);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, -1);
    EXPECT_EQ(pm.subCaseMatch, -1);
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_TRUE(pm.p == pmz);
    EXPECT_TRUE(pm.pT == pmz);
    EXPECT_TRUE(pm.swap23 == pmz);
    EXPECT_TRUE(pm.swap23T == pmz);
    EXPECT_TRUE(pm.cV == cz);
    EXPECT_TRUE(pm.cVT == cz);
}

TEST(PatternMatrixTest,PatternMatrixIsTranspose) {
    patternMatrix pm1 = patternMatrix(1, "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[1,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,0,0,0,0,0][0,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[1,0,1,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    EXPECT_TRUE(pm1.isTranspose(pm2));
    EXPECT_FALSE(pm1.isTranspose(pm3));
    // Check for transposes that would need to be rearranged to match
    EXPECT_TRUE(pm1.isTranspose(pm4));
    EXPECT_TRUE(pm2.isTranspose(pm5));
    // pm1 and pm5 are equivalent but not transposes
    EXPECT_FALSE(pm1.isTranspose(pm5));
}

TEST(PatternMatrixTest,PatternMatrixIs23Swap) {
    patternMatrix pm1 = patternMatrix(1, "[2,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm6 = patternMatrix(1, "[3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm7 = patternMatrix(1, "[0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm8 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");

    EXPECT_FALSE(pm1.is23Swap(pm2));
    EXPECT_FALSE(pm1.is23Swap(pm3));
    EXPECT_FALSE(pm1.is23Swap(pm4));
    EXPECT_FALSE(pm1.is23Swap(pm5));
    EXPECT_TRUE(pm1.is23Swap(pm6));
    // Check for 23 swaps that would need to be rearranged to match
    EXPECT_TRUE(pm1.is23Swap(pm7));
    EXPECT_TRUE(pm1.is23Swap(pm8));
    // This is also a 23 swap that could be rearranged to match
    EXPECT_TRUE(pm1.is23Swap(pm1));
    // This time the 23 swap cannot be rearranged to match
    EXPECT_FALSE(pm2.is23Swap(pm2));
    // But does match pm3
    EXPECT_TRUE(pm2.is23Swap(pm3));
}

TEST(PatternMatrixTest,PatternMatrixIs23SwapT) {
    patternMatrix pm1 = patternMatrix(1, "[2,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[3,0,0,0,0,0][3,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[2,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm6 = patternMatrix(1, "[3,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm7 = patternMatrix(1, "[0,0,0,0,0,0][3,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm8 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,0,0,0,0][3,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    
    EXPECT_FALSE(pm1.is23SwapT(pm1));
    EXPECT_FALSE(pm1.is23SwapT(pm2));
    EXPECT_FALSE(pm1.is23SwapT(pm3));
    EXPECT_FALSE(pm1.is23SwapT(pm4));
    EXPECT_FALSE(pm1.is23SwapT(pm5));
    EXPECT_TRUE(pm1.is23SwapT(pm6));
    // Check for 23 swaps that would need to be rearranged to match
    EXPECT_TRUE(pm1.is23SwapT(pm7));
    EXPECT_TRUE(pm1.is23SwapT(pm8));
}

TEST(PatternMatrixTest,PatternMatrixLoadCases) {
    patternMatrix pm1 = patternMatrix(1, ALL_ZEROS_PATTERN);
    // These are the cases we expect to have and the test is to ensure we load all the cases we intend to
    std::vector<caseMatrix> baseCases;
    baseCases.push_back(caseMatrix(0, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(1, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(2, "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(3, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(4, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(5, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(6, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(7, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"));
    baseCases.push_back(caseMatrix(8, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"));
    for (int i = 0; i < baseCases.size(); i++) {
        EXPECT_TRUE(pm1.cases[i].id == baseCases[i].id);
        EXPECT_TRUE(pm1.cases[i].c == baseCases[i].c);
        EXPECT_TRUE(pm1.cases[i].cT == baseCases[i].cT);
    }
}

TEST(PatternMatrixTest,PatternMatrixMatchesCase) {
    for (auto const& [caseNumber, patterns] : CASE_TO_VALID_PATTERN_MAP) {
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
                EXPECT_TRUE(pm.matchesCase(caseNumber)) << "Case: " << caseNumber << " Pattern: " << pattern;
        }
    }    
}

/*
TEST(PatternMatrixTest,PatternMatrixMatchOnCases) {
    FAIL() << "Not implemented";
}



/*
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
*/