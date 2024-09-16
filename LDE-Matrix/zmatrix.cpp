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
    // Reset the value pair counts
    rowValuePairCountsTotals.clear();
    rowValuePairCountsTotals.resize(rows+1);
    for (int i = 0; i < rowValuePairCountsTotals.size(); i++) {
        rowValuePairCountsTotals[i].resize(maxValue+1);
    }
    colValuePairCountsTotals.clear();
    colValuePairCountsTotals.resize(cols+1);
    for (int i = 0; i < colValuePairCountsTotals.size(); i++) {
        colValuePairCountsTotals[i].resize(maxValue+1);
    }
    rowValuePairCounts.clear();
    rowValuePairCounts.resize(rows);
    colValuePairCounts.clear();
    colValuePairCounts.resize(cols);
    for (int i = 0; i < rows; i++) {
        rowValuePairCounts[i].resize(rows);
        for (int j = 0; j < rows; j++) {
            rowValuePairCounts[i][j].resize(maxValue+1);
        }
    }
    for (int i = 0; i < cols; i++) {
        colValuePairCounts[i].resize(cols);
        for (int j = 0; j < cols; j++) {
            colValuePairCounts[i][j].resize(maxValue+1);
        }
    }
    
    // Now to update the pair counts
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            // Initialize the pair counts to 0
            rowPairCounts[i][j] = 0;
            colPairCounts[i][j] = 0;
            for (int k = 0; k < maxValue+1; k++) {
                    rowValuePairCounts[i][j][k] = 0;
                    colValuePairCounts[i][j][k] = 0;
            }
            // If i == j, then we are comparing the same row or column so we can skip this and preset values
            if (i == j) {
                rowPairCounts[i][j] = 6;
                colPairCounts[i][j] = 6;
                for (int k = 0; k < maxValue+1; k++) {
                    rowValuePairCounts[i][j][k] = 6;
                    colValuePairCounts[i][j][k] = 6;
                }
                continue;
            }
            // Now do the pair counts
            for (int k = 0; k < cols; k++) {
                if (z[i][k] == z[j][k]) {
                    rowPairCounts[i][j]++;
                    rowValuePairCounts[i][j][z[i][k]]++;
                }
                if (z[k][i] == z[k][j]) {
                    colPairCounts[i][j]++;
                    colValuePairCounts[i][j][z[k][i]]++;
                }
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            rowPairCountsTotals[rowPairCounts[i][j]]++;
            for (int k = 0; k < maxValue+1; k++) {
                rowValuePairCountsTotals[rowValuePairCounts[i][j][k]][k]++;
            }
        }
    }
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < cols; j++) {
            colPairCountsTotals[colPairCounts[i][j]]++;
            for (int k = 0; k < maxValue+1; k++) {
                colValuePairCountsTotals[colValuePairCounts[i][j][k]][k]++;
            }
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
    printPairCounts(os);
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
    os << "Row Pair Count Totals:" << std::endl;
    for (int i = 0; i < rowPairCountsTotals.size(); i++) {
        os << i << "=" << rowPairCountsTotals[i] << std::endl;
    }
    os << "Row Value Pair Counts: " << std::endl;
    for (int i = 0; i < rowValuePairCounts.size(); i++) {
        os << "Row " << i+1 << " = [";
        for (int j = 0; j < rowValuePairCounts[i].size(); j++) {
            os << "Value " << j << " = [";
            for (int k = 0; k < rowValuePairCounts[i][j].size(); k++) {
                os << k << "=" << rowValuePairCounts[i][j][k];
                if (k != rowValuePairCounts[i][j].size()-1) os << ", ";
            }
            os << "]";
            if (j != rowValuePairCounts[i].size()-1) os << ", ";
        }
        os << "]" << std::endl;
    }
    os << "Row Value Pair Count Totals:" << std::endl;
    for (int i = 0; i < rowValuePairCountsTotals.size(); i++) {
        os << "Value " << i << " = [";
        for (int j = 0; j < rowValuePairCountsTotals[i].size(); j++) {
            os << j << "=" << rowValuePairCountsTotals[i][j];
            if (j != rowValuePairCountsTotals[i].size()-1) os << ", ";
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
    os << "Column Pair Count Totals:" << std::endl;
    for (int i = 0; i < colPairCountsTotals.size(); i++) {
        os << i << "=" << colPairCountsTotals[i] << std::endl;
    }
    os << "Column Value Pair Counts: " << std::endl;
    for (int i = 0; i < colValuePairCounts.size(); i++) {
        os << "Column " << i+1 << " = [";
        for (int j = 0; j < colValuePairCounts[i].size(); j++) {
            os << "Value " << j << " = [";
            for (int k = 0; k < colValuePairCounts[i][j].size(); k++) {
                os << k << "=" << colValuePairCounts[i][j][k];
                if (k != colValuePairCounts[i][j].size()-1) os << ", ";
            }
            os << "]";
            if (j != colValuePairCounts[i].size()-1) os << ", ";
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
        if (rowPairCountsTotals[i] != other.rowPairCountsTotals[i]) {
            //std::cout << "Row Pair Counts Totals Mismatch: " <<  "Pair Count(" << i << "): " << " self(" << rowPairCountsTotals[i] << ") != other(" << other.rowPairCountsTotals[i] << ")" << std::endl;
            return false;
        }
    }
    for (int i = 0; i < colPairCountsTotals.size(); i++) {
        if (colPairCountsTotals[i] != other.colPairCountsTotals[i]) {
            //std::cout << "Col Pair Counts Totals Mismatch: " << "Pair Count(" << i << ")" << " self(" << colPairCountsTotals[i] << ") != other(" << other.colPairCountsTotals[i] << ")" << std::endl;
            return false;
        }
    }
    for (int i = 0; i < zCountRows.size(); i++) {
        if (zCountRows[i] != other.zCountRows[i]) {
            //std::cout << "Row Count Mismatch: " << i << std::endl;
            return false;
        }
    }
    for (int i = 0; i < zCountCols.size(); i++) {
        if (zCountCols[i] != other.zCountCols[i]) {
            //std::cout << "Col Count Mismatch: " << i << std::endl;
            return false;
        }
    }
    // maxValue == 1 means that this a case pattern and not an actual pattern so we do not need to check for a rearrange match
    // This should probably be a flag or something else
    // TODO - Refactor this to make the maxValue == 1 => Case Matrix more explicit
    if (maxValue == 1) return true;
    return rearrangeMatch(other);
}

bool zmatrix::rearrangeMatch(const zmatrix &other) const {
    // attempt a strict match first before attempting to rearrange to match
    if (strictMatch(other)) return true;
    zmatrix newZ(rows, cols, maxValue);
    // if the row counts and column counts match, then we can map the rows and columns
    // i is the row index of z, j is the row index of other
    // These map other to z
    std::vector<std::map<int, int>> rowMaps;
    std::vector<std::map<int, int>> colMaps;
    
    for (int i = 0; i < rows; i++) {
        std::vector<std::map<int, int>> nextRowMaps;
        for (int j = 0; j < other.rows; j++) {
            if (zRowCounts[i] == other.zRowCounts[j]) {
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
    /*
    // print the rowMaps
    for (int i = 0; i < rowMaps.size(); i++) {
        std::cout << "Row Map " << i << ":";
        for (auto const& [key, val] : rowMaps[i]) {
            std::cout << val << "->" << key << " ";
        }
        std::cout << std::endl;    
    }
    
    // print the colMaps
    for (int i = 0; i < colMaps.size(); i++) {
        std::cout << "Col Map " << i << ": ";
        for (auto const& [key, val] : colMaps[i]) {
            std::cout << val << "->" << key << " ";
        }
        std::cout << std::endl;
    }
    */
    for (int i = 0; i < rowMaps.size(); i++) {
        for (int j = 0; j < colMaps.size(); j++) {
            // rowMaps[i] is the map # and rowMaps[i][0] is the map from other[0] -> z[N]
            if (z[rowMaps[i][0]][colMaps[j][0]] != other.z[0][0]) continue;
            for (int k = 0; k < rows; k++) {
                for (int l = 0; l < cols; l++) {
                    newZ.z[rowMaps[i][k]][colMaps[j][l]] = other.z[k][l];
                    // we can move to the next possibility if we find a mismatch between newZ and current z
                    // but we can optimize this later
                }
            }
            //std::cout << "New Z: " << newZ << std::endl;
            // Need to check with each row and column mapping
            if (strictMatch(newZ)) return true;
        }
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