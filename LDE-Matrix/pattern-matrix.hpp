#ifndef PATTERN_MATRIX_HPP
#define PATTERN_MATRIX_HPP

#include <map>
#include <string>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "zmatrix.hpp"

class patternMatrix {
    public:
        patternMatrix();
        patternMatrix(int pattern928Number);  // This will load a 928 pattern by number
        patternMatrix(int patternNumber, std::string matrix);
        patternMatrix(int pNum, std::string matrix, bool newEncoding);
        int id;  // Primary identifier for the pattern
        // These identifiers start at 1 and increase with 0 meaning it's not in the file
        int id2704 = 0;  // Identifier for the 2704 pattern
        int id928 = 0;   // Identifier for the 928 pattern
        int id785 = 0;   // Identifier for the 785 pattern
        // Cases that the pattern could match
        std::vector<caseMatrix> cases;
        int caseMatch;
        char subCaseMatch;
        bool rearrangedToMatchCase = false;
        // These are the flags for the pattern matrix
        bool printDebugInfo = false;  // WIP: This is for printing debug information
        bool singleCaseRearrangement = false;  // This is stop the case rearrangement code after a single solution is found
        // << operator flags
        bool printID = false;
        bool printCaseMatch = false;
        bool printAllIDs = false;
        bool multilineOutput = false;
        bool printOldEncoding = false;
        // TODO refactor to split new vs old encoding
        zmatrix pNewEncoding;  // This is the pattern matrix with the new encoding - 2y + x
        // These are the matrices that are used for comparison
        zmatrix p; // This is the pattern matrix
        zmatrix pT;  // This is the transposed pattern matrix
        zmatrix swap23;  // This is the pattern matrix with 2s swapped for 3s and 3s swapped for 2s
        zmatrix swap23T;  // This is the transposed pattern matrix with 2s swapped for 3s and 3s swapped for 2s
        zmatrix cV;  // This is the pattern matrix changed to match the case style, 0s for 0,1 and 1s for 2,3
        zmatrix cVT;  // This is the transposed pattern matrix changed to match the case style, 0s for 0,1 and 1s for 2,3
        zmatrix pGroupings;  // This is the pattern matrix with the groupings applied
        std::vector<std::vector<std::vector<int>>> possibleValues;  // After LDE reduction, these are the possible values for the pattern
        // TODO: Add a A, B set of matrices for the pattern where: A+Bsqrt(2) = pattern
        //   and use these for normality and orthogonality checking
        //   Essentially, this is keeping the original form of the patterns when they are in binary form 'A B' of (0 0, 0 1, 1 0, 1 1)
        // TODO: Use this to check all the rules that Ming shared such as normality, orthogonality, 3 pair counts, 2/1 pair counts, etc.
        //    See line 160 in the Latex document
        std::string originalMatrix; // This is the original matrix string
        std::map<std::string, bool> caseRearrangements; // This is a map of all the possible case rearrangements
        std::map<std::string, bool> allPossibleValuePatterns; // This is a map of all the possible case rearrangements
        std::vector<std::vector<int>> rowPairCounts;  // This is the row pair counts for the pattern
        std::vector<std::vector<int>> colPairCounts;  // This is the col pair counts for the pattern
        // The following totals are counts of the pair counts
        //  i=0 -> number of rows/cols with no pairs, i=2 -> number of rows/cols with 2 pairs, i=4 -> number of rows/cols with 4 pairs, i=6 -> number of rows/cols with 6 pairs
        std::vector<int> rowPairCountsTotals;  // This contains the totals of the row pair counts for the pattern 
        std::vector<int> colPairCountsTotals;  // This contains the totals of the col pair counts for the pattern
        // LDE Tracking
        int LDE = 0;  // This is the LDE of the pattern
        // This tracks an entry by entry LDE change based on T-Gate operations and factorization
        std::vector<std::vector<int>> entryLDEs;
        // This will track the T-Gate operations applied to the pattern
        std::vector<std::string> tGateOperations;
        // This will hold the sets of T-Gate operations depending on the case and whether it's optimal or all
        //  The string will be formatted "Tpqx" for Rows or "xTpq" for columns where p (int) and q (int) are the rows or columns to multiply
        //  This will follow the format of the T-Gate operations and what is listed in the papers
        std::vector<std::vector<std::string>> tGateOperationSets;

        void loadFromString(std::string m);
        void updatePairCounts();
        // Case Matching Functions
        void matchOnCases();
        bool matchesCase(int caseIndex);
        bool determineSubCase();
        bool case1SubCaseMatch();
        bool case2SubCaseMatch();
        bool case3SubCaseMatch();
        bool case3aSubCaseCheck();
        bool case3bSubCaseCheck();
        bool case3cSubCaseCheck();
        bool case4SubCaseMatch();
        bool case5SubCaseMatch();
        bool case6SubCaseMatch();
        bool case7SubCaseMatch();
        bool case8SubCaseMatch();
        std::string getFirstCaseRearrangement();

        // Duplicate Pattern Checks
        bool isDuplicate(patternMatrix other);
        bool isTranspose(patternMatrix other);
        bool is23Swap(patternMatrix other);
        bool is23SwapT(patternMatrix other);
        // Extra Printing Functions
        void printDebug(std::ostream& os);
        void printLDEs(std::ostream& os);
        void printPossibleValues(std::ostream& os);
        void printPairCounts(std::ostream& os);
        void printRowPairCounts(std::ostream& os);
        void printColPairCounts(std::ostream& os);
        void printCounts(std::ostream& os);
        void printRowCounts(std::ostream& os);
        void printColCounts(std::ostream& os);
        void printCountRows(std::ostream& os);
        void printCountCols(std::ostream& os);
        // Get a list of T-Gate operations applied
        std::string printTGateOperations();
        // Get the possible values
        std::string getMaxOfPossibleValues();
        void generateAllPossibleValuePatterns();
        void recursiveAllPossibleValueSet(int position, zmatrix z);
        // T-Gate Multiplication Functions
        void leftTGateMultiply(int p, int q);
        void rightTGateMultiply(int p, int q);
        int patternElementAddition(int a, int b);
        bool findOptimalTGateOperations();
        bool optimalTGatesCase1();
        bool optimalTGatesCase2();
        bool optimalTGatesCase3();
        bool optimalTGatesCase4();
        bool optimalTGatesCase5();
        bool optimalTGatesCase6();
        bool optimalTGatesCase7();
        bool optimalTGatesCase8();
        bool findAllTGateOptions();
        bool allTGatesCase1();
        bool allTGatesCase2();
        bool allTGatesCase3();
        bool allTGatesCase4();
        bool allTGatesCase5();
        bool allTGatesCase6();
        bool allTGatesCase7();
        bool allTGatesCase8();
        // LDE Reduction Functions
        void ldeReductionOnEntry(int row, int col, int ldeReduction);
        void ldeReductionOnPattern(int ldeValue);
        // These could be private but are public for testing
        bool rearrangeMatrix();
        void rearrangeColumns(zmatrix patternVersion, zmatrix caseVersion, int currentColumn);
        void rearrangeRows(zmatrix patternVersion, zmatrix caseVersion, int currentRow);
        std::string toString();
        // TODO: Add a csv output for the pattern matrix
        bool isNormalized();
        bool isOrthogonal();

        // Friends
        friend std::ostream& operator<<(std::ostream&,const patternMatrix &);
    
    private:
        void init();
        void loadCases();
        // These need to be refactored to only use 1 value as the matrix will always be a square matrix
        int rows = 6;
        int cols = 6;
        int maxValue = 3;
};

#endif // PATTERN_MATRIX_HPP
