#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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
    for (int i = 0; i < maxValue+1; i++) {
        zCountRows[i].resize(rows+1);
    }
    zCountCols.resize(maxValue+1);
    for (int i = 0; i < maxValue+1; i++) {
        zCountCols[i].resize(cols+1);
    }
}

// TODO - Add row / column sum sorting -> highest to the top left
// Update the metadata for the matrix
// This includes the sum, number counts, row counts, column counts, and counts of counts
void zmatrix::updateMetadata() {
    // Reset all the counts to 0
    zSum = 0;
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

std::ostream& operator<<(std::ostream& os, const zmatrix &zm) {
    // Print the matrix
    for (int i = 0; i < zm.z.size(); i++) {
        os << "["; // Print opening bracket for the row
        for (int j = 0; j < zm.z[i].size(); j++) {
            os << zm.z[i][j]; // Print element
            if (j != zm.z[i].size()-1) os << ","; // Don't print a comma after the last element
        }
        os << "]"; // Print closing bracket for the row
    }
    return os;
}

bool zmatrix::operator==(const zmatrix &other) const {
    if (zSum != other.zSum) return false;
    if (zNumCounts != other.zNumCounts) return false;
    if (zRowCounts.size() != other.zRowCounts.size()) return false;
    if (zColCounts.size() != other.zColCounts.size()) return false;
    if (zCountRows.size() != other.zCountRows.size()) return false;
    if (zCountCols.size() != other.zCountCols.size()) return false;
    // We do not check for 1:1 matches in zRowCounts and zColCounts here
    // because the rows and columns can be rearranged and still be considered the same matrix
    for (int i = 0; i < zCountRows.size(); i++) {
        if (zCountRows[i] != other.zCountRows[i]) return false;
    }
    for (int i = 0; i < zCountCols.size(); i++) {
        if (zCountCols[i] != other.zCountCols[i]) return false;
    }
    return true;
}

bool zmatrix::strictMatch(const zmatrix &other) {
    // Check if the matrices are the same size but possibly arranged differently
    if (operator!=(other)) {
        return false;
    }
    // Check if the matrices have the same values
    for (int i = 0; i < z.size(); i++) {
        if (z[i] != other.z[i]) return false;
    }
    return true;
}

bool zmatrix::operator!=(const zmatrix &other) const {
    return !(*this == other);
}