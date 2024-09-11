#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "zmatrix.hpp"

zmatrix::zmatrix() {
    zSum = 0;
}

zmatrix::zmatrix(int rows, int cols, int maxValue) {
    this->rows = rows;
    this->cols = cols;
    this->maxValue = maxValue;
    z.resize(rows);
    for (int i = 0; i < rows; i++) {
        z[i].resize(cols);
    }
    zSum = 0;
    zNumCounts.resize(maxValue+1);
    zRowCounts.resize(rows);
    for (int i = 0; i < rows; i++) {
        zRowCounts[i].resize(maxValue+1);
    }
    zColCounts.resize(cols);
    for (int i = 0; i < cols; i++) {
        zColCounts[i].resize(maxValue+1);
    }
    zCountRows.resize(maxValue+1);
    zCountCols.resize(maxValue+1);
    for (int i = 0; i < maxValue+1; i++) {
        zCountRows[i].resize(rows+1);
        zCountCols[i].resize(cols+1);
    }
}

// TODO - Add row / column sum sorting -> highest to the top left
// Update the metadata for the matrix
// This includes the sum, number counts, row counts, column counts, and counts of counts
void zmatrix::updateMetadata() {
    // Reset all the counts to 0
    zSum = 0;
    // TODO - Make new vectors here to speed this up
    for (int i = 0; i < zNumCounts.size(); i++) {
        zNumCounts[i] = 0;
    }
    for (int i = 0; i < zRowCounts.size(); i++) {
        for (int j = 0; j < zRowCounts[i].size(); j++) {
            zRowCounts[i][j] = 0;
        }
    }
    for (int i = 0; i < zColCounts.size(); i++) {
        for (int j = 0; j < zColCounts[i].size(); j++) {
            zColCounts[i][j] = 0;
        }
    }
    for (int i = 0; i < zCountRows.size(); i++) {
        for (int j = 0; j < zCountRows[i].size(); j++) {
            zCountRows[i][j] = 0;
        }
    }
    for (int i = 0; i < zCountCols.size(); i++) {
        for (int j = 0; j < zCountCols[i].size(); j++) {
            zCountCols[i][j] = 0;
        }
    }
    // Update the counts
    for (int i = 0; i < z.size(); i++) {
        for (int j = 0; j < z[i].size(); j++) {
            if (z[i][j] < 0 || z[i][j] > maxValue) {
                std::string val_error = std::format("Value out of range: z[{}][{}] = {} > {}", i, j, z[i][j], maxValue);
                throw std::runtime_error(val_error);
            }
            zSum += z[i][j];
            zNumCounts[z[i][j]]++;
            zRowCounts[i][z[i][j]]++;
            zColCounts[j][z[i][j]]++;
        }
    }
    for (int i = 0; i < zRowCounts.size(); i++) {
        for (int j = 0; j < zCountRows.size(); j++) {
            zCountRows[j][zRowCounts[i][j]]++;
        }
    }
    for (int i = 0; i < zColCounts.size(); i++) {
        for (int j = 0; j < zCountCols.size(); j++) {
            zCountCols[j][zColCounts[i][j]]++;
        }
    }
    updatePairCounts();
}

void zmatrix::updatePairCounts(){
    rowPairCountsTotals.clear();
    rowPairCountsTotals.resize(rows+1);
    colPairCountsTotals.clear();
    colPairCountsTotals.resize(cols+1);
    // Reset the pair counts
    rowPairCounts.clear();
    rowPairCounts.resize(rows);
    for (int i = 0; i < rows; i++) {
        rowPairCounts[i].resize(rows);
    }
    colPairCounts.clear();
    colPairCounts.resize(cols);
    for (int i = 0; i < cols; i++) {
        colPairCounts[i].resize(cols);
    }
    // Now to update the pair counts
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            rowPairCounts[i][j] = 0;
            colPairCounts[i][j] = 0;
            if (i == j) {
                rowPairCounts[i][j] = 6;
                colPairCounts[i][j] = 6;
                continue;
            }
            for (int k = 0; k < cols; k++) {
                if (z[i][k] == z[j][k]) rowPairCounts[i][j]++;
                if (z[k][i] == z[k][j]) colPairCounts[i][j]++;
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            rowPairCountsTotals[rowPairCounts[i][j]]++;
            colPairCountsTotals[colPairCounts[i][j]]++;
        }
    }
}

void zmatrix::swapRows(int i, int j) {
    std::vector<int> temp;
    temp = z[i];
    z[i] = z[j];
    z[j] = temp;
    // TODO - This is not the most efficient way to update the metadata, so it should be improved
    updateMetadata();    
}

void zmatrix::swapColumns(int i, int j) {
    for (int k = 0; k < cols; k++) {
        int temp = z[k][i];
        z[k][i] = z[k][j];
        z[k][j] = temp;
    }
    // TODO - This is not the most efficient way to update the metadata, so it should be improved
    updateMetadata();
}

// Print a debug of the matrix
void zmatrix::printDebug(std::ostream& os) {
    
    // Print the matrix
    os << "Matrix: " << std::endl;
    for (int i = 0; i < z.size(); i++) {
        os << "["; // Print opening bracket for the row
        for (int j = 0; j < z[i].size(); j++) {
            os << z[i][j]; // Print element
            if (j != z[i].size()-1) os << ", "; // Don't print a comma after the last element
        }
        os << "]" << std::endl; // Print closing bracket for the row
    }
    os << "Summary: " << std::endl;
    os << "Rows: " << rows << std::endl;
    os << "Cols: " << cols << std::endl;
    os << "Max Value: " << maxValue << std::endl;
    // Print the sum
    os << "Sum: " << zSum << std::endl;
    // Print the number counts
    os << "Number Counts: [";
    for (int i = 0; i < zNumCounts.size(); i++) {
        os << i << "=" << zNumCounts[i]; 
        if(i != zNumCounts.size()-1) os << ", ";
    }
    os << "]" << std::endl;
    // Print the row counts
    os << "Row Counts: " << std::endl;
    for (int i = 0; i < zRowCounts.size(); i++) {
        os << "Row " << i+1 << " = [";
        for (int j = 0; j < zRowCounts[i].size(); j++) {
            os << j << "=" << zRowCounts[i][j];
            if (j != zRowCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
    // Print the column counts
    os << "Column Counts: " << std::endl;
    for (int i = 0; i < zColCounts.size(); i++) {
        os << "Column " << i+1 << " = [";
        for (int j = 0; j < zColCounts[i].size(); j++) {
            os << j << "=" << zColCounts[i][j];
            if (j != zColCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
    // Print the count of rows
    os << "Count of Rows with: " << std::endl;
    for (int i = 0; i < zCountRows.size(); i++) {
        os << "Value " << i << " = [";
        for (int j = 0; j < zCountRows[i].size(); j++) {
            os << j << "=" << zCountRows[i][j];
            if (j != zCountRows[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
    // Print the count of columns
    os << "Count of Columns with: " << std::endl;
    for (int i = 0; i < zCountCols.size(); i++) {
        os << "Value " << i << " = [";
        for (int j = 0; j < zCountCols[i].size(); j++) {
            os << j << "=" << zCountCols[i][j];
            if (j != zCountCols[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printPairCounts(std::ostream& os) {
    printRowPairCounts(os);
    printColPairCounts(os);
}

void zmatrix::printRowPairCounts(std::ostream& os) {
    os << "Row Pair Counts: " << std::endl;
    for (int i = 0; i < rowPairCounts.size(); i++) {
        os << "Row " << i+1 << " = [";
        for (int j = 0; j < rowPairCounts[i].size(); j++) {
            os << rowPairCounts[i][j];
            if (j != rowPairCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printColPairCounts(std::ostream& os) {
    os << "Column Pair Counts: " << std::endl;
    for (int i = 0; i < colPairCounts.size(); i++) {
        os << "Column " << i+1 << " = [";
        for (int j = 0; j < colPairCounts[i].size(); j++) {
            os << colPairCounts[i][j];
            if (j != colPairCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printCounts(std::ostream& os) {
    printRowCounts(os);
    printColCounts(os);
    printCountRows(os);
    printCountCols(os);
}

void zmatrix::printRowCounts(std::ostream& os) {
    os << "Row Counts: " << std::endl;
    for (int i = 0; i < zRowCounts.size(); i++) {
        os << "Row " << i+1 << " = [";
        for (int j = 0; j < zRowCounts[i].size(); j++) {
            os << j << "=" << zRowCounts[i][j];
            if (j != zRowCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printColCounts(std::ostream& os) {
    os << "Column Counts: " << std::endl;
    for (int i = 0; i < zColCounts.size(); i++) {
        os << "Column " << i+1 << " = [";
        for (int j = 0; j < zColCounts[i].size(); j++) {
            os << j << "=" << zColCounts[i][j];
            if (j != zColCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printCountRows(std::ostream& os) {
    os << "Count of Rows with: " << std::endl;
    for (int i = 0; i < zCountRows.size(); i++) {
        os << "Value " << i << " = [";
        for (int j = 0; j < zCountRows[i].size(); j++) {
            os << j << "=" << zCountRows[i][j];
            if (j != zCountRows[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

void zmatrix::printCountCols(std::ostream& os) {
    os << "Count of Columns with: " << std::endl;
    for (int i = 0; i < zCountCols.size(); i++) {
        os << "Value " << i << " = [";
        for (int j = 0; j < zCountCols[i].size(); j++) {
            os << j << "=" << zCountCols[i][j];
            if (j != zCountCols[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const zmatrix &zm) {
    // Print the matrix
    for (int i = 0; i < zm.z.size(); i++) {
        os << "["; // Print opening bracket for the row
        for (int j = 0; j < zm.z[i].size(); j++) {
            os << zm.z[i][j]; // Print element
            if (j != zm.z[i].size()-1) os << ","; // Don't print a comma after the last element
        }
        os << "]"; // Print closing bracket for the row
        if(zm.multilineOutput) os << std::endl; // Print a newline if we're doing multiline output
    }
    return os;
}
// Check if two matrices are considered equal
//  This means that the matrices can be rearranged to match but don't have to be in the same order
//  This does not mean that the matrices are exactly the same
//  Also, this does not check for transposition equality
bool zmatrix::operator==(const zmatrix &other) const {
    if (rows != other.rows) return false;
    if (cols != other.cols) return false;
    if (maxValue != other.maxValue) return false;
    if (zSum != other.zSum) return false;
    if (zNumCounts != other.zNumCounts) return false;
    if (zRowCounts.size() != other.zRowCounts.size()) return false;
    if (zColCounts.size() != other.zColCounts.size()) return false;
    if (zCountRows.size() != other.zCountRows.size()) return false;
    if (zCountCols.size() != other.zCountCols.size()) return false;
    // We do not check for 1:1 matches in zRowCounts and zColCounts here
    // because the rows and columns can be rearranged and still be considered the same matrix
    // but the row pair counts and column pair counts must match
    for (int i = 0; i < rowPairCountsTotals.size(); i++) {
        if (rowPairCountsTotals[i] != other.rowPairCountsTotals[i]) return false;
    }
    for (int i = 0; i < colPairCountsTotals.size(); i++) {
        if (colPairCountsTotals[i] != other.colPairCountsTotals[i]) return false;
    }
    for (int i = 0; i < zCountRows.size(); i++) {
        if (zCountRows[i] != other.zCountRows[i]) return false;
    }
    for (int i = 0; i < zCountCols.size(); i++) {
        if (zCountCols[i] != other.zCountCols[i]) return false;
    }
    if (maxValue == 1) return true;
    return rearrangeMatch(other);
}

bool zmatrix::rearrangeMatch(const zmatrix &other) const {
    // attempt a strict match first before attempting to rearrange to match
    if (strictMatch(other)) return true;
    // Ok, this might be easier than expected
    //  Since we already know about pair counts and row / column counts, we can use those to construct
    //  possible arrangements of the matrix to see if they match.  We might be able to do this in a multithreaded way
    //  to speed up the process.  
    //    To do this, we need to find the row / column / pair counts that match for each matrix and then map them to possible locations
    //    So, for example, z[0] could map to either other.z[3] / other.z[4], 
    //     so the map would be 0 -> 3, 4 (z -> other.z) 
    //     or we could flip it to 3 -> 0, 4-> 0 (other.z -> z) which might make rearranging easier as we can just iterate
    //     the rows and columns, put values in place, and see if we have a match.  This should limit the number of possibilities.
    //     I think we can go through the row / columns and quickly check values, so do r0 / c0 and check new[0][0] == other.z[0][0]
    //     then do c1 and check new[0][1] == other.z[0][1] and so on.  If we get a mismatch, we can break out and try the next possibility.
    //     This probably will be recursive.
    // 
    // Here we need to make sure all of the rows and columns match regardless of how they are arranged
    // Here, the specific row and column value counts must match along with the specific pair counts
    //  If those match, then we need to verify that entry by entry they match regardless of how they are rearranged
    //for (int i = 0; i < z.size(); i++) {
        // ideas -> find which rows that row i can match with then repeat for    
    //}
    // rowMap: zRow -> otherRow
    // colMap: zCol -> otherCol
    //std::map<int, std::vector<int>> rowMap;
    //std::map<int, std::vector<int>> colMap;
    zmatrix newZ(rows, cols, maxValue);
    // if the row counts and column counts match, then we can map the rows and columns
    // i is the row index of z, j is the row index of other
    std::vector<std::map<int, int>> rowMaps;
    std::vector<std::map<int, int>> colMaps;
    // *** NEW NOTES ***
    //  Need to compare the rearranged other to this zmatrix
    //  So make a map of other -> z
    for (int i = 0; i < rows; i++) {
        std::vector<std::map<int, int>> nextRowMaps;
        for (int j = 0; j < other.rows; j++) {
            for (int k = 0; k < zRowCounts[i].size(); k++) {
                //std::cout << "zRowCounts[" << i << "][" << k << "] = " << zRowCounts[i][k] << " other.zRowCounts[" << j << "][" << k << "] = " << other.zRowCounts[j][k] << std::endl;
            }
            if (zRowCounts[i] == other.zRowCounts[j]) {
                //std::cout << "Row " << i << " matches with other row " << j << std::endl;
                if (i==0) {
                    std::map<int, int> newMap;
                    newMap[j] = i;
                    nextRowMaps.push_back(newMap);
                } else {
                    for (int k = 0; k < rowMaps.size(); k++) {
                        std::map<int, int> newMap = rowMaps[k];
                        newMap[j] = i;
                        // The prevents duplicates for row mappings
                        if (newMap.size() == i+1) {
                            nextRowMaps.push_back(newMap);
                        }
                    }
                }
            }
        }
        rowMaps = nextRowMaps;
    }
    // i is the column index of z, j is the column index of other
    for (int i = 0; i < cols; i++) {
        std::vector<std::map<int, int>> nextColMaps;
        for (int j = 0; j < other.cols; j++) {
            if (zColCounts[i] == other.zColCounts[j]) {
                std::cout << "Column " << i << " matches with other column " << j << std::endl;
                if (i==0) {
                    std::map<int, int> newMap;
                    newMap[j] = i;
                    nextColMaps.push_back(newMap);
                    continue;
                }
                for (int k = 0; k < colMaps.size(); k++) {
                    std::map<int, int> newMap = colMaps[k];
                    newMap[j] = i;
                    // The prevents duplicates for row mappings
                    if (newMap.size() == i+1) nextColMaps.push_back(newMap);
                }
            }
        }
        colMaps = nextColMaps;
    }
    // print the rowMaps
    for (int i = 0; i < rowMaps.size(); i++) {
        std::cout << "Row Map " << i << std::endl;
        for (auto const& [key, val] : rowMaps[i]) {
            std::cout << key << " -> " << val << std::endl;
        }
    }
    // print the colMaps
    for (int i = 0; i < colMaps.size(); i++) {
        std::cout << "Col Map " << i << std::endl;
        for (auto const& [key, val] : colMaps[i]) {
            std::cout << key << " -> " << val << std::endl;
        }
    }
    for (int i = 0; i < rowMaps.size(); i++) {
        for (int j = 0; j < colMaps.size(); j++) {
            if (z[rowMaps[i][0]][colMaps[j][0]] != other.z[0][0]) continue;
            for (int k = 0; k < rows; k++) {
                //std::cout << "Row " << k << " maps to " << rowMaps[i][k] << std::endl;
                for (int l = 0; l < cols; l++) {
                    newZ.z[rowMaps[i][k]][colMaps[j][l]] = other.z[k][l];
                    // we can move to the next possibility if we find a mismatch between newZ and current z
                }
            }
        }
        std::cout << "New Z: " << newZ << std::endl;
        if (strictMatch(newZ)) return true;
    }
    return false;
}

bool zmatrix::strictMatch(const zmatrix &other) const {
    // Check if the matrices have the same values
    for (int i = 0; i < z.size(); i++) {
        if (z[i] != other.z[i]) {
            return false;
        }
    }
    return true;
}

bool zmatrix::operator!=(const zmatrix &other) const {
    return !(*this == other);
}