#ifndef Z_MATRIX_HPP
#define Z_MATRIX_HPP

#include <vector>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Attempting to generalize the pattern and case matrices into a single class
class zmatrix {
    public:
        zmatrix();
        zmatrix(int rows, int cols, int maxValue);

        // These are the flags for the zmatrix
        bool multilineOutput = false;

        std::vector<std::vector<int>> z;
        int zSum;
        std::vector<int> zNumCounts; // Count of the number of 0s, 1s, 2s, 3s, ..., n's in the matrix
        std::vector<std::vector<int>> zRowCounts;  // Count of the number of 0s, 1s, 2s, 3s, ..., n's in each row
        std::vector<std::vector<int>> zColCounts;  // Count of the number of 0s, 1s, 2s, 3s, ..., n's in each column
        std::vector<std::vector<int>> zCountRows;  // Number of rows with a count of  0, 1, 2, 3, 4, 5, 6, ..n (based on columns) of 0s, 1s, 2s, 3s, ..., n's
        std::vector<std::vector<int>> zCountCols;  // Number of columns with a count of 0, 1, 2, 3, 4, 5, 6, ..n (based on rows) of 0s, 1s, 2s, 3s, ..., n's
        // These are the pair counts for the matrix when comparing row[i] to row[j] and col[i] to col[j]
        std::vector<std::vector<int>> rowPairCounts;  // This is the row pair counts for the pattern
        std::vector<std::vector<int>> colPairCounts;  // This is the col pair counts for the pattern
        // The following totals are counts of the pair counts
        //  i=0 -> number of rows/cols with no pairs, i=2 -> number of rows/cols with 2 pairs, i=4 -> number of rows/cols with 4 pairs, i=6 -> number of rows/cols with 6 pairs
        std::vector<int> rowPairCountsTotals;  // This contains the totals of the row pair counts for the pattern 
        std::vector<int> colPairCountsTotals;  // This contains the totals of the col pair counts for the pattern
        // These are the pair counts for the matrix when comparing row[i] to row[j] and col[i] to col[j] but split out by value
        std::vector<std::vector<std::vector<int>>> rowValuePairCounts;  // This contains the pair counts for each value in the row vs another row
        std::vector<std::vector<std::vector<int>>> colValuePairCounts;  // This contains the pair counts for each value in the col vs another col
        // The following totals are counts of the pair value counts
        //  i=0 -> number of rows/cols with no pairs for each value, i=2 -> number of rows/cols with 2 pairs for each value, i=4 -> number of rows/cols with 4 pairs for each value, i=6 -> number of rows/cols with 6 pairs for each value
        std::vector<std::vector<int>> rowValuePairCountsTotals;  // This contains the counts for each value in the row
        std::vector<std::vector<int>> colValuePairCountsTotals;  // This contains the counts for each value in the col

        void updateMetadata();
        void updatePairCounts();
        void swapRows(int i, int j);
        void swapColumns(int i, int j);

        // These are used to find pair counts
        int getRowPairCount(int i, int j, std::map<int, bool> values); //m.count(key) == 1
        int getColPairCount(int i, int j, std::map<int, bool> values); //m.count(key) == 1

        bool operator==(const zmatrix &) const;  // Read the comments in zmatrix.cpp for more information on what equality means
        bool operator!=(const zmatrix &) const;
        bool rearrangeMatch(const zmatrix &other) const;
        bool strictMatch(const zmatrix &other) const;

        void printDebug(std::ostream&);
        void printPairCounts(std::ostream&);
        void printRowPairCounts(std::ostream&);
        void printColPairCounts(std::ostream&);
        void printCounts(std::ostream&);
        void printRowCounts(std::ostream&);
        void printColCounts(std::ostream&);
        void printCountRows(std::ostream&);
        void printCountCols(std::ostream&);
        friend std::ostream& operator<<(std::ostream&,const zmatrix &);
    
    private:
        int rows;
        int cols;
        int maxValue;
};

#endif // Z_MATRIX_HPP