#include "zmatrix.hpp"

#include <gtest/gtest.h>

int ROWS = 6;
int COLS = 6;
int CASE_MAX_VALUE = 1;
int PATTERN_MAX_VALUE = 3;

std::vector<std::vector<int>> CASE1 = {
    {1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

std::vector<std::vector<int>> CASE1_REARRANGED = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

std::vector<std::vector<int>> CASE7 = {
    {1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 1, 1}
};

std::vector<std::vector<int>> PATTERN_A = {
    {2, 3, 0, 0, 0, 0},
    {3, 2, 0, 0, 0, 0},
    {0, 0, 3, 3, 0, 0},
    {0, 0, 3, 3, 0, 0},
    {0, 0, 0, 0, 2, 2},
    {0, 0, 0, 0, 2, 2}
};

std::vector<std::vector<int>> PATTERN_B = {
    {3, 2, 0, 0, 0, 0},
    {3, 2, 0, 0, 0, 0},
    {1, 1, 3, 3, 1, 1},
    {1, 1, 3, 3, 1, 1},
    {0, 0, 0, 0, 3, 2},
    {0, 0, 0, 0, 2, 3}
};

std::vector<std::vector<int>> NEGATIVE_VALUE_PATTERN = {
    {-1, -1, 0, 0, 0, 0},
    {3, 2, 0, 0, 0, 0},
    {1, 1, 3, 3, 1, 1},
    {1, 1, 3, 3, 1, 1},
    {0, 0, 0, 0, 3, 2},
    {0, 0, 0, 0, 2, 3}
};

TEST(ZMatrixTest, DefaultZMatrixConstructor) {
    zmatrix z = zmatrix();
    EXPECT_EQ(z.zSum, 0);
}

TEST(ZMatrixTest, ZMatrixConstructor) {
    zmatrix zCase = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    EXPECT_EQ(zCase.z.size(), ROWS);
    EXPECT_EQ(zCase.z[0].size(), COLS);
    EXPECT_EQ(zCase.zSum, 0);
    EXPECT_EQ(zCase.zNumCounts.size(), CASE_MAX_VALUE+1);
    EXPECT_EQ(zCase.zRowCounts.size(), ROWS);
    EXPECT_EQ(zCase.zColCounts.size(), COLS);
    EXPECT_EQ(zCase.zCountRows.size(), CASE_MAX_VALUE+1);
    EXPECT_EQ(zCase.zCountCols.size(), CASE_MAX_VALUE+1);

    zmatrix zPattern = zmatrix(ROWS, COLS, PATTERN_MAX_VALUE);
    EXPECT_EQ(zPattern.z.size(), ROWS);
    EXPECT_EQ(zPattern.z[0].size(), COLS);
    EXPECT_EQ(zPattern.zSum, 0);
    EXPECT_EQ(zPattern.zNumCounts.size(), PATTERN_MAX_VALUE+1);
    EXPECT_EQ(zPattern.zRowCounts.size(), ROWS);
    EXPECT_EQ(zPattern.zColCounts.size(), COLS);
    EXPECT_EQ(zPattern.zCountRows.size(), PATTERN_MAX_VALUE+1);
    EXPECT_EQ(zPattern.zCountCols.size(), PATTERN_MAX_VALUE+1);
}

TEST(ZMatrixTest, ZMatrixUpdateMetadata) {
    // Test case matrices
    zmatrix zCase = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    // Case 1
    zCase.z = CASE1;
    zCase.updateMetadata();
    EXPECT_EQ(zCase.zSum, 4);
    EXPECT_EQ(zCase.zNumCounts[0], 32);
    EXPECT_EQ(zCase.zNumCounts[1], 4);
    for (int i = 0; i < ROWS; i++) {
        if (i==0 || i==1) {
            EXPECT_EQ(zCase.zRowCounts[i][0], 4);
            EXPECT_EQ(zCase.zRowCounts[i][1], 2);
        } else {
            EXPECT_EQ(zCase.zRowCounts[i][0], 6);
            EXPECT_EQ(zCase.zRowCounts[i][1], 0);
        }
    }
    for (int i = 0; i < COLS; i++) {
        if (i==0 || i==1) {
            EXPECT_EQ(zCase.zColCounts[i][0], 4);
            EXPECT_EQ(zCase.zColCounts[i][1], 2);
        } else {
            EXPECT_EQ(zCase.zColCounts[i][0], 6);
            EXPECT_EQ(zCase.zColCounts[i][1], 0);
        }
    }
    // i is the value we are counting, j count of the value, zCountRows[i][j] = count of rows with j count of i
    for (int i = 0; i < zCase.zCountRows.size(); i++) {
        for (int j = 0; j < zCase.zCountRows[i].size(); j++) {
            if (i == 0 && j ==4) {
                EXPECT_EQ(zCase.zCountRows[i][j], 2);
            } else if (i == 0 && j ==6) {
                EXPECT_EQ(zCase.zCountRows[i][j], 4);
            } else if (i == 1 && j ==0) {
                EXPECT_EQ(zCase.zCountRows[i][j], 4);
            } else if (i == 1 && j ==2) {
                EXPECT_EQ(zCase.zCountRows[i][j], 2);
            } else {
                EXPECT_EQ(zCase.zCountRows[i][j], 0);
            }
        }
    }
    // i is the value we are counting, j count of the value, zCountCols[i][j] = count of cols with j count of i
    for (int i = 0; i < zCase.zCountCols.size(); i++) {
        for (int j = 0; j < zCase.zCountCols[i].size(); j++) {
            if (i == 0 && j ==4) {
                EXPECT_EQ(zCase.zCountCols[i][j], 2);
            } else if (i == 0 && j ==6) {
                EXPECT_EQ(zCase.zCountCols[i][j], 4);
            } else if (i == 1 && j ==0) {
                EXPECT_EQ(zCase.zCountCols[i][j], 4);
            } else if (i == 1 && j ==2) {
                EXPECT_EQ(zCase.zCountCols[i][j], 2);
            } else {
                EXPECT_EQ(zCase.zCountCols[i][j], 0);
            }
        }
    }
    // Case 7
    zCase.z = CASE7;
    zCase.updateMetadata();
    EXPECT_EQ(zCase.zSum, 12);
    EXPECT_EQ(zCase.zNumCounts[0], 24);
    EXPECT_EQ(zCase.zNumCounts[1], 12);
    for (int i = 0; i < ROWS; i++) {
            EXPECT_EQ(zCase.zRowCounts[i][0], 4);
            EXPECT_EQ(zCase.zRowCounts[i][1], 2);
    }
    for (int i = 0; i < COLS; i++) {
            EXPECT_EQ(zCase.zColCounts[i][0], 4);
            EXPECT_EQ(zCase.zColCounts[i][1], 2);
    }
    // i is the value we are counting, j count of the value, zCountRows[i][j] = count of rows with j count of i
    for (int i = 0; i < zCase.zCountRows.size(); i++) {
        for (int j = 0; j < zCase.zCountRows[i].size(); j++) {
            if (i == 0 && j ==4) {
                EXPECT_EQ(zCase.zCountRows[i][j], 6);
            } else if (i == 1 && j ==2) {
                EXPECT_EQ(zCase.zCountRows[i][j], 6);
            } else {
                EXPECT_EQ(zCase.zCountRows[i][j], 0);
            }
        }
    }
    // i is the value we are counting, j count of the value, zCountCols[i][j] = count of cols with j count of i
    for (int i = 0; i < zCase.zCountCols.size(); i++) {
        for (int j = 0; j < zCase.zCountCols[i].size(); j++) {
            if (i == 0 && j ==4) {
                EXPECT_EQ(zCase.zCountCols[i][j], 6);
            } else if (i == 1 && j ==2) {
                EXPECT_EQ(zCase.zCountCols[i][j], 6);
            } else {
                EXPECT_EQ(zCase.zCountCols[i][j], 0);
            }
        }
    }

    // Test pattern matrices
    zmatrix zPattern = zmatrix(ROWS, COLS, PATTERN_MAX_VALUE);
    // Pattern A
    zPattern.z = PATTERN_A;
    zPattern.updateMetadata();
    EXPECT_EQ(zPattern.zSum, 30);
    EXPECT_EQ(zPattern.zNumCounts[0], 24);
    EXPECT_EQ(zPattern.zNumCounts[1], 0);
    EXPECT_EQ(zPattern.zNumCounts[2], 6);
    EXPECT_EQ(zPattern.zNumCounts[3], 6);
    for (int i = 0; i < ROWS; i++) {
        switch (i) {
            case 0: case 1: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 1);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 1);
                break;
            }
            case 2: case 3: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 2);
                break;
            }
            case 4: case 5: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 2);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 0);
                break;
            }
            default: {
                FAIL();  // This should never happen
            };
        }
    }
    for (int i = 0; i < COLS; i++) {
        switch (i) {
            case 0: case 1: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 4);
                EXPECT_EQ(zPattern.zColCounts[i][1], 0);
                EXPECT_EQ(zPattern.zColCounts[i][2], 1);
                EXPECT_EQ(zPattern.zColCounts[i][3], 1);
                break;
            }
            case 2: case 3: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 4);
                EXPECT_EQ(zPattern.zColCounts[i][1], 0);
                EXPECT_EQ(zPattern.zColCounts[i][2], 0);
                EXPECT_EQ(zPattern.zColCounts[i][3], 2);
                break;
            }
            case 4: case 5: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 4);
                EXPECT_EQ(zPattern.zColCounts[i][1], 0);
                EXPECT_EQ(zPattern.zColCounts[i][2], 2);
                EXPECT_EQ(zPattern.zColCounts[i][3], 0);
                break;
            }
            default: {
                FAIL();  // This should never happen
            };
        }
    }
    // i is the value we are counting, j count of the value, zCountRows[i][j] = count of rows with j count of i
    for (int i = 0; i < zPattern.zCountRows.size(); i++) {
        for (int j = 0; j < zPattern.zCountRows[i].size(); j++) {
            if (i == 0 && j == 4) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 6);
            } else if (i == 1 && j == 0) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 6);
            } else if ((i == 2 || i == 3) && j < 3) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 2);
            } else {
                EXPECT_EQ(zPattern.zCountRows[i][j], 0);
            }
        }
    }
    // i is the value we are counting, j count of the value, zCountCols[i][j] = count of cols with j count of i
    for (int i = 0; i < zPattern.zCountCols.size(); i++) {
        for (int j = 0; j < zPattern.zCountCols[i].size(); j++) {
            if (i == 0 && j == 4) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 6);
            } else if (i == 1 && j == 0) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 6);
            } else if ((i == 2 || i == 3) && j < 3) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 2);
            } else {
                EXPECT_EQ(zPattern.zCountCols[i][j], 0);
            }
        }
    }
    // Pattern B
    zPattern.z = PATTERN_B;
    zPattern.updateMetadata();
    EXPECT_EQ(zPattern.zSum, 40);
    EXPECT_EQ(zPattern.zNumCounts[0], 16);
    EXPECT_EQ(zPattern.zNumCounts[1], 8);
    EXPECT_EQ(zPattern.zNumCounts[2], 4);
    EXPECT_EQ(zPattern.zNumCounts[3], 8);
    for (int i = 0; i < ROWS; i++) {
        switch (i) {
            case 0: case 1: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 1);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 1);
                break;
            }
            case 2: case 3: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 2);
                break;
            }
            case 4: case 5: {
                EXPECT_EQ(zPattern.zRowCounts[i][0], 4);
                EXPECT_EQ(zPattern.zRowCounts[i][1], 0);
                EXPECT_EQ(zPattern.zRowCounts[i][2], 1);
                EXPECT_EQ(zPattern.zRowCounts[i][3], 1);
                break;
            }
            default: {
                FAIL() << "This should never happen";
            };
        }
    }
    for (int i = 0; i < COLS; i++) {
        switch (i) {
            case 0: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 2);
                EXPECT_EQ(zPattern.zColCounts[i][1], 2);
                EXPECT_EQ(zPattern.zColCounts[i][2], 0);
                EXPECT_EQ(zPattern.zColCounts[i][3], 2);
                break;
            }
            case 1: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 2);
                EXPECT_EQ(zPattern.zColCounts[i][1], 2);
                EXPECT_EQ(zPattern.zColCounts[i][2], 2);
                EXPECT_EQ(zPattern.zColCounts[i][3], 0);
                break;
            }
            case 2: case 3: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 4);
                EXPECT_EQ(zPattern.zColCounts[i][1], 0);
                EXPECT_EQ(zPattern.zColCounts[i][2], 0);
                EXPECT_EQ(zPattern.zColCounts[i][3], 2);
                break;
            }
            case 4: case 5: {
                EXPECT_EQ(zPattern.zColCounts[i][0], 2);
                EXPECT_EQ(zPattern.zColCounts[i][1], 2);
                EXPECT_EQ(zPattern.zColCounts[i][2], 1);
                EXPECT_EQ(zPattern.zColCounts[i][3], 1);
                break;
            }
            default: {
                FAIL() << "This should never happen";
            };
        }
    }
    // i is the value we are counting, j count of the value, zCountRows[i][j] = count of rows with j count of i
    for (int i = 0; i < zPattern.zCountRows.size(); i++) {
        for (int j = 0; j < zPattern.zCountRows[i].size(); j++) {
            if (i == 0 && j == 0) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 2);
            } else if (i == 0 && j == 4) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 4);
            } else if (i == 1 && j == 0) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 4);
            } else if (i == 1 && j == 4) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 2);
            } else if (i == 2 && j == 0) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 2);
            } else if (i == 2 && j == 1) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 4);
            } else if (i == 3 && j == 1) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 4);
            } else if (i == 3 && j == 2) {
                EXPECT_EQ(zPattern.zCountRows[i][j], 2);
            } else {
                EXPECT_EQ(zPattern.zCountRows[i][j], 0);
            }
        }
    }
    // i is the value we are counting, j count of the value, zCountCols[i][j] = count of cols with j count of i
    for (int i = 0; i < zPattern.zCountCols.size(); i++) {
        for (int j = 0; j < zPattern.zCountCols[i].size(); j++) {
            if (i == 0 && j == 2) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 4);
            } else if (i == 0 && j == 4) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 2);
            } else if (i == 1 && j == 0) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 2);
            } else if (i == 1 && j == 2) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 4);
            } else if (i == 2 && j == 0) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 3);
            } else if (i == 2 && j == 1) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 2);
            } else if (i == 2 && j == 2) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 1);
            } else if (i == 3 && j == 0) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 1);
            } else if (i == 3 && j == 1) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 2);
            } else if (i == 3 && j == 2) {
                EXPECT_EQ(zPattern.zCountCols[i][j], 3);
            } else {
                EXPECT_EQ(zPattern.zCountCols[i][j], 0);
            }
        }
    }
    // Test Max Value Exception
    zmatrix zMaxValueException = zmatrix(ROWS, COLS, PATTERN_MAX_VALUE-1);
    // Load a pattern with a value equal to the max value
    zMaxValueException.z = PATTERN_A;
    EXPECT_THROW(zMaxValueException.updateMetadata(),std::runtime_error);
    // Test Negative Value Exception
    zmatrix zNegativeValueException = zmatrix(ROWS, COLS, PATTERN_MAX_VALUE);
    zNegativeValueException.z = NEGATIVE_VALUE_PATTERN;
    EXPECT_THROW(zNegativeValueException.updateMetadata(),std::runtime_error);
}

TEST(ZMatrixTest, ZMatrixEqualityOperator) {
    zmatrix z1 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    zmatrix z2 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    z1.z = CASE1;
    z2.z = CASE1;
    z1.updateMetadata();
    z2.updateMetadata();
    EXPECT_TRUE(z1 == z2);
    z2.z = CASE1_REARRANGED;
    z2.updateMetadata();
    EXPECT_TRUE(z1 == z2);
    z2.z = CASE7;
    z2.updateMetadata();
    EXPECT_FALSE(z1 == z2);
    // Test for different size matrices and different max values
    zmatrix t1 = zmatrix(2, 2, 2);
    t1.z = {{1, 1}, {1, 1}};
    zmatrix t2 = zmatrix(3, 2, 2);
    t2.z = {{1, 1}, {1, 1}, {1, 1}};
    zmatrix t3 = zmatrix(2, 3, 2);
    t3.z = {{1, 1, 1}, {1, 1, 1}};
    zmatrix t4 = zmatrix(2, 2, 3);
    t3.z = {{1, 1}, {1, 1}};
    EXPECT_FALSE(t1 == t2); // Row counts don't match
    EXPECT_FALSE(t1 == t3); // Columns counts don't match
    EXPECT_FALSE(t1 == t4); // Max Values don't match
    EXPECT_FALSE(t2 == t3); // Row and Column counts don't match
    EXPECT_FALSE(t2 == t4); // Row counts and Max Values don't match
    EXPECT_FALSE(t3 == t4); // Column counts and Max Values don't match
}

TEST(ZMatrixTest, ZMatrixStrictMatch) {
    zmatrix z1 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    zmatrix z2 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    z1.z = CASE1;
    z2.z = CASE1;
    z1.updateMetadata();
    z2.updateMetadata();
    EXPECT_TRUE(z1.strictMatch(z2));
    z2.z = CASE1_REARRANGED;
    z2.updateMetadata();
    EXPECT_FALSE(z1.strictMatch(z2));
    z2.z = CASE7;
    z2.updateMetadata();
    EXPECT_FALSE(z1.strictMatch(z2));
}

TEST(ZMatrixTest, ZMatrixNotEqualOperator) {
    zmatrix z1 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    zmatrix z2 = zmatrix(ROWS, COLS, CASE_MAX_VALUE);
    z1.z = CASE1;
    z2.z = CASE1;
    z1.updateMetadata();
    z2.updateMetadata();
    EXPECT_FALSE(z1 != z2);
    z2.z = CASE1_REARRANGED;
    z2.updateMetadata();
    EXPECT_FALSE(z1 != z2);
    z2.z = CASE7;
    z2.updateMetadata();
    EXPECT_TRUE(z1 != z2);
}

/*

        void printDebug();
        friend std::ostream& operator<<(std::ostream&,const zmatrix &);
*/