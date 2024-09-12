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
    /*
    // bool isDuplicate(patternMatrix, int &, bool);
    patternMatrix pm759 = patternMatrix(759);
    pd.isDuplicate(pm759, duplicateID, false);
    EXPECT_EQ(duplicateID, 759);
    // 352000584 [1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0] -> P 759
    patternMatrix pm = patternMatrix(1, "[1,1,3,3,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,3,3,3,0,0][3,3,3,3,0,0]", true);
    pd.isDuplicate(pm, duplicateID, false);
    EXPECT_EQ(duplicateID, 759);

    // 352000645 [1,1,3,3,0,0][1,3,1,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,1,1,3,0,0][3,3,3,3,0,0] -> P ???
    patternMatrix p832 = patternMatrix(832);
    patternMatrix p867 = patternMatrix(867);
    EXPECT_EQ(p832.isDuplicate(p867), false) << "Expected 832 and 867 to not be duplicates";

    patternMatrix pm1 = patternMatrix(1, "[2,2,3,3,0,0][2,2,3,3,0,0][1,0,0,1,1,1][1,0,0,1,1,1][2,3,2,3,0,0][2,3,2,3,0,0]", false);
    pd.isDuplicate(pm1, duplicateID, false);
    EXPECT_EQ(duplicateID, 831);

    patternMatrix p816 = patternMatrix(816);
    p816.multilineOutput = true;
    patternMatrix p831 = patternMatrix(831);
    p831.multilineOutput = true;
    pd.isDuplicate(p831, duplicateID, false);
    EXPECT_EQ(duplicateID, 831);

    patternMatrix pm802 = patternMatrix(802);
    pd.isDuplicate(pm802, duplicateID, false);
    EXPECT_EQ(duplicateID, 802);
    */
    patternMatrix pm2 = patternMatrix(352000134, "[1,1,1,1,0,0][1,3,1,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][3,1,3,1,0,0][3,3,3,3,0,0]", true);
    EXPECT_TRUE(pd.isDuplicate(pm2, duplicateID, false));
    std::cout << "Duplicate ID: " << duplicateID << std::endl;
    //pm2.printDebug(std::cout);

    caseMatrix c3 = caseMatrix(3, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    //c3.printDebug();
    /*
    std::cout << "----------" << std::endl;
    std::cout << "p816:" << std::endl;
    p816.printPairCounts(std::cout);
    p816.printCounts(std::cout);
    std::cout << "----------" << std::endl;
    std::cout << "p831:" << std::endl;
    //p831.printPairCounts(std::cout);
    //p831.printCounts(std::cout);
    std::cout << "----------" << std::endl;
    
    // check 884 / 885 and 926 / 927
    patternMatrix p884 = patternMatrix(884);
    p884.multilineOutput = true;
    patternMatrix p885 = patternMatrix(885);
    p885.multilineOutput = true;
    std::cout << "----------" << std::endl;
    std::cout << "p884:" << std::endl;
    p884.printPairCounts(std::cout);
    p884.printCounts(std::cout);
    std::cout << "----------" << std::endl;
    std::cout << "p885:" << std::endl;
    p885.printPairCounts(std::cout);
    p885.printCounts(std::cout);
    std::cout << "----------" << std::endl;
    */
}

