#ifndef POSSIBLE_PATTERN_MATRIX_HPP
#define POSSIBLE_PATTERN_MATRIX_HPP

#include "zmatrix.hpp"

class possiblePatternMatrix {
    public:
        possiblePatternMatrix();
        possiblePatternMatrix(int idNum, std::string matrix, bool newEncoding);
        
        int id;
        zmatrix pp; // This is the possible pattern matrix
        zmatrix ppNewEncoding; // This is the possible pattern matrix but using the new encoding which swaps 1 and 2
        zmatrix ppT;  // This is the transposed possible pattern matrix
        std::string originalMatrix; // This is the original matrix string
        std::vector<std::string> origins; // This is the list of patterns and gates that this possible pattern was derived from
        std::unordered_map<int, bool> leadsToPatternIDs;
        std::ostream* debugOutput;  // WIP...does this work???
        std::vector<std::vector<int>> rowPairCounts;  // This is the row pair counts for the pattern
        std::vector<std::vector<int>> colPairCounts;  // This is the col pair counts for the pattern
        // The following totals are counts of the pair counts
        //  i=0 -> number of rows/cols with no pairs, i=2 -> number of rows/cols with 2 pairs, i=4 -> number of rows/cols with 4 pairs, i=6 -> number of rows/cols with 6 pairs
        std::vector<int> rowPairCountsTotals;  // This contains the totals of the row pair counts for the pattern 
        std::vector<int> colPairCountsTotals;  // This contains the totals of the col pair counts for the pattern

        // << operator flags
        bool printID = false;
        bool printOrigins = false;
        bool printLeadsToPatternIDs = false;
        bool multilineOutput = false;
        bool printOldEncoding = false;

        void loadFromString(std::string m);
        void updatePairCounts();

        std::vector<std::vector<std::vector<int>>> getPossibleValues();  // This is used to copy the possible values to a pattern matrix

        bool isDuplicate(possiblePatternMatrix other);
        bool isTranspose(possiblePatternMatrix other);

        void printPairCounts(std::ostream& os);
        void printRowPairCounts(std::ostream& os);
        void printColPairCounts(std::ostream& os);
        void printCounts(std::ostream& os);
        void printRowCounts(std::ostream& os);
        void printColCounts(std::ostream& os);
        void printCountRows(std::ostream& os);
        void printCountCols(std::ostream& os);
        void printDebug(std::ostream& os);

        friend std::ostream& operator<<(std::ostream& os,const possiblePatternMatrix &);
    private:
        void init();
        int rows = 6;
        int cols = 6;
        int maxValue = 6;
};

#endif // POSSIBLE_PATTERN_MATRIX_HPP