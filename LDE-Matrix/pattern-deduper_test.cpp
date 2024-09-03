#include "pattern-matrix.hpp"
#include "pattern-deduper.hpp"
#include "zmatrix.hpp"
#include "test-utils.hpp"

#include <gtest/gtest.h>

TEST(PatternDeduper, patternDeduperConstructor) {
    // verify loading is correct
    GTEST_SKIP() << "Not implemented yet";
}


TEST(PatternDeduper, patternDeduperIsDuplicate) {
    int duplicateID = 0;
    patternDeduper pd = patternDeduper();
    // bool isDuplicate(patternMatrix, int &, bool);
    patternMatrix pm759 = patternMatrix(759);
    pd.isDuplicate(pm759, duplicateID, false);
    EXPECT_EQ(duplicateID, 759);
    // 352000584 [1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0] -> P 759
    patternMatrix pm = patternMatrix(1, "[1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0]", true);
    pd.isDuplicate(pm, duplicateID, false);
    EXPECT_EQ(duplicateID, 759);

    // 352000645 [1,1,3,3,0,0][1,3,1,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,1,1,3,0,0][3,3,3,3,0,0] -> P 64
    patternMatrix p832 = patternMatrix(832);
    patternMatrix p867 = patternMatrix(867);
    EXPECT_EQ(p832.isDuplicate(p867), false) << "Expected 832 and 867 to not be duplicates";
}

