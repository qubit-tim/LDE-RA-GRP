#include <vector>
#include <tuple>
#include <iostream>
#include <map>
#include <string>
#include <regex>

#include "pattern-matrix.hpp"
#include "zmatrix.hpp"
#include "test-utils.hpp"

#include <gtest/gtest.h>

/*  Here, we can take case 2, apply T gates, generate the possible value patterns, and see where they go.
They should all into the following groups and will be useful for testing case transitions and LDE reductions
group 1: (lead to entire case 2) 3, 4, 5, 6
group 2: (lead to entire case 5a+b) 7,8,9,10,11,12
group 3: (lead to entire case 4) 19,20,24,115,116,117
group 4: (lead to entire case 3) 93,94,188,189
group 5: (lead to entire case 6) 95,96,98,365,366,368
group 6: (lead to entire case 6) 48,49,50,55,72,73
group 7: (lead to entire case 6) 190,191,192,194,197,198
group 8: (lead to entire case 8) 403, 404,405,700,701,702
*/


// case # -> group name -> list of patterns
/*
std::map <int, std::map <int, std::vector<int>>> PATTERN_GROUPS_BY_CASE = {
  {
    2,  // Case 2 Groups
    {
        1, {3, 4, 5, 6}, // group 1
        2, {7, 8, 9, 10, 11, 12}, // group 2
        3, {19, 20, 24, 115, 116, 117}, // group 3
        5, {93, 94, 188, 189}, // group 5
        6, {95, 96, 98, 365, 366, 368}, // group 6
        7, {48, 49, 50, 55, 72, 73}, // group 7
        8, {190, 191, 192, 194, 197, 198} // group 8
    }
  },
};
*/

/*
*/

struct PatternTGateTestData {             
  int pID;
  std::vector<std::string> tGateOps;
  std::string postTGateMatrix;
  std::string postTGateLDEs;
  std::string postLDEReductionLDEs;
  std::string postLDEReductionPossibleValues;
};

std::ostream& operator<<(std::ostream& os, const PatternTGateTestData& s) {
  os << "{Pattern ID: " << s.pID;
  os << ", T-Gate Operations: ";
  for (const auto& op : s.tGateOps) {
    os << op << " ";
  }
  os << ", Post T-Gate Matrix: " << s.postTGateMatrix;
  os << ", Post T-Gate LDEs: " << s.postTGateLDEs;
  os << ", Post LDE Reduction LDEs: " << s.postLDEReductionLDEs;
  os << ", Post LDE Reduction Possible Values: " << s.postLDEReductionPossibleValues;
  os << "}";
  return os;
}

std::vector<PatternTGateTestData> patternTGateTestCases = {
  {//case 2 - random check
    2, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 1 (lead to entire case 2) 3, 4, 5, 6
    3, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 2 (lead to entire case 5a+b) 7,8,9,10,11,12
    7, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 3 (lead to entire case 4) 19,20,24,115,116,117
    19, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 4 (lead to entire case 3) 93,94,188,189
    93, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 5 (lead to entire case 6) 95,96,98,365,366,368
    95, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 6 (lead to entire case 6) 48,49,50,55,72,73
    48, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 7 (lead to entire case 8) 190,191,192,194,197,198
    190, 
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
  {//case 2 - group 8 (lead to entire case 8) 403,404,405,700,701,702
    403,
    {"xT12"}, 
    "[0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
    "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]",
    "[-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]",
    "[{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]",
  },
};

/*
TEST(PM_T_GATES_TEST, PatternTGateTests) {
  for (const auto& testCase : PatternTGateTestCases) {
    patternMatrix pm = patternMatrix(testCase.pID);
    
    //std::cout << "Before T-Gate multiplication:" << std::endl;
    //std::cout << pm << std::endl;
    
    for (auto tGateOp : testCase.tGateOps) {
      if (std::regex_match(tGateOp, R_T_GATE_REGEX)) {
          std::string c1(1, tGateOp[2]);
          std::string c2(1, tGateOp[3]);
          pm.rightTGateMultiply(std::stoi(c1), std::stoi(c2));
      }
      if (std::regex_match(tGateOp, L_T_GATE_REGEX)) {
          std::string r1(1, tGateOp[1]);
          std::string r2(1, tGateOp[2]);
          pm.leftTGateMultiply(std::stoi(r1), std::stoi(r2));
      }
  }
    
    //std::cout << "After T-Gate multiplication " << pm.printTGateOperations() << ":" << std::endl;
    //std::cout << pm << std::endl;

    // Check the post-T-Gate matrix
    EXPECT_EQ(pm.toString(), testCase.postTGateMatrix);

    //std::cout << "LDEs Before Reduction(s):" << std::endl;
    //pm.printLDEs(std::cout);
    
    // Check the LDEs before reduction
    std::ostringstream postTGateLDEsString;
    pm.printLDEs(postTGateLDEsString);
    EXPECT_EQ(postTGateLDEsString.str(), testCase.postTGateLDEs);

    pm.doLDEReduction();
    
    //std::cout << "LDEs After Reduction(s):" << std::endl;
    //pm.printLDEs(std::cout);
    
    // Check the LDEs after reduction
    std::ostringstream postReducationLDEsString;
    pm.printLDEs(postReducationLDEsString);
    EXPECT_EQ(postReducationLDEsString.str(), testCase.postLDEReductionLDEs);

    //std::cout << "Possible values:" << std::endl;
    //pm.printPossibleValues(std::cout);
    
    // Check the possible values
    std::ostringstream possibleValueString;
    pm.printPossibleValues(possibleValueString);
    EXPECT_EQ(possibleValueString.str(), testCase.postLDEReductionPossibleValues);
  }
}

TEST(PM_T_GATES_TEST, Pattern2) {
  patternMatrix pm = patternMatrix(2);
  std::cout << "Before T-Gate multiplication:" << std::endl;
  std::cout << pm << std::endl;
  pm.rightTGateMultiply(1,2);
  // Expect the matrix to be:
  //  [0,0,2,2,0,2][0,0,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]
  std::cout << "After T-Gate multiplication " << pm.printTGateOperations() << ":" << std::endl;
  std::cout << pm << std::endl;
  std::cout << "LDEs Before Reduction(s):" << std::endl;
  pm.printLDEs(std::cout);
  std::cout << std::endl;
  // Expect the LDEs to be:
  //   [1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0]
  pm.doLDEReduction();
  std::cout << "LDEs After Reduction(s):" << std::endl;
  pm.printLDEs(std::cout);
  std::cout << std::endl;
  // Expect the LDEs to be:
  //    [-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1][-1,-1,-1,-1,-1,-1]
  std::cout << "Possible values:" << std::endl;
  pm.printPossibleValues(std::cout);
  // Expect Possible Values to be:
  //   [{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{1,3},{1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2},{0,2},{0,2},{0,2}]
}
*/

class MyParameterizedTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {
};

TEST_P(MyParameterizedTest, SumTest) {
  auto params = GetParam();
  int a = std::get<0>(params);
  int b = std::get<1>(params);
  int expectedSum = std::get<2>(params);

  ASSERT_EQ(a + b, expectedSum);
}

INSTANTIATE_TEST_SUITE_P(
    MyTestCases,
    MyParameterizedTest,
    ::testing::Values(
        std::make_tuple(1, 2, 3),
        std::make_tuple(4, 5, 9),
        std::make_tuple(-1, 1, 0)
    )
);

class ParameterizedTGateTests : public ::testing::TestWithParam<PatternTGateTestData> {};

TEST_P(ParameterizedTGateTests, PatternTGateTests) {
  PatternTGateTestData testCase = GetParam();
  patternMatrix pm = patternMatrix(testCase.pID);
  
  //std::cout << "Before T-Gate multiplication:" << std::endl;
  //std::cout << pm << std::endl;
  
  for (auto tGateOp : testCase.tGateOps) {
    if (std::regex_match(tGateOp, R_T_GATE_REGEX)) {
        std::string c1(1, tGateOp[2]);
        std::string c2(1, tGateOp[3]);
        pm.rightTGateMultiply(std::stoi(c1), std::stoi(c2));
    }
    if (std::regex_match(tGateOp, L_T_GATE_REGEX)) {
        std::string r1(1, tGateOp[1]);
        std::string r2(1, tGateOp[2]);
        pm.leftTGateMultiply(std::stoi(r1), std::stoi(r2));
    }
  }
  
  //std::cout << "After T-Gate multiplication " << pm.printTGateOperations() << ":" << std::endl;
  //std::cout << pm << std::endl;

  // Check the post-T-Gate matrix
  EXPECT_EQ(pm.toString(), testCase.postTGateMatrix);

  //std::cout << "LDEs Before Reduction(s):" << std::endl;
  //pm.printLDEs(std::cout);
  
  // Check the LDEs before reduction
  std::ostringstream postTGateLDEsString;
  pm.printLDEs(postTGateLDEsString);
  EXPECT_EQ(postTGateLDEsString.str(), testCase.postTGateLDEs);

  pm.doLDEReduction();
  
  //std::cout << "LDEs After Reduction(s):" << std::endl;
  //pm.printLDEs(std::cout);
  
  // Check the LDEs after reduction
  std::ostringstream postReducationLDEsString;
  pm.printLDEs(postReducationLDEsString);
  EXPECT_EQ(postReducationLDEsString.str(), testCase.postLDEReductionLDEs);

  //std::cout << "Possible values:" << std::endl;
  //pm.printPossibleValues(std::cout);
  
  // Check the possible values
  std::ostringstream possibleValueString;
  pm.printPossibleValues(possibleValueString);
  EXPECT_EQ(possibleValueString.str(), testCase.postLDEReductionPossibleValues);
};

INSTANTIATE_TEST_SUITE_P(
  TGateTestCases,
  ParameterizedTGateTests,
  ::testing::ValuesIn(patternTGateTestCases)
);
