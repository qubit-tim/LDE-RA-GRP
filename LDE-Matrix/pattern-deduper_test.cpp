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
    patternDeduper pd = patternDeduper();

    // bool isDuplicate(patternMatrix, int &, bool);
    // 352000584 [1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0] -> P 759
    patternMatrix pm = patternMatrix(1, "[1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0]", true);
    int duplicateID = 0;
    pd.isDuplicate(pm, duplicateID, false);
    EXPECT_EQ(duplicateID, 759);

    // 352000645 [1,1,3,3,0,0][1,3,1,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,1,1,3,0,0][3,3,3,3,0,0] -> P 64
    pm = 
}

