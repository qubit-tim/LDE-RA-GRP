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

// This should really be encompassed into unit tests
void zmatrix::printDebug() {
    
    // Print the matrix
    std::cout << "Matrix: " << std::endl;
    for (int i = 0; i < z.size(); i++) {
        std::cout << "["; // Print opening bracket for the row
        for (int j = 0; j < z[i].size(); j++) {
            std::cout << z[i][j]; // Print element
            if (j != z[i].size()-1) std::cout << ", "; // Don't print a comma after the last element
        }
        std::cout << "]" << std::endl; // Print closing bracket for the row
    }
    // Print the sum
    std::cout << "Sum: " << zSum << std::endl;
    // Print the number counts
    std::cout << "Number Counts: [";
    for (int i = 0; i < zNumCounts.size(); i++) {
        std::cout << i << "=" << zNumCounts[i]; 
        if(i != zNumCounts.size()-1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    // Print the row counts
    std::cout << "Row Counts: " << std::endl;
    for (int i = 0; i < zRowCounts.size(); i++) {
        std::cout << "Row " << i+1 << " = [";
        for (int j = 0; j < zRowCounts[i].size(); j++) {
            std::cout << j << "=" << zRowCounts[i][j];
            if (j != zRowCounts[i].size()-1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    // Print the column counts
    std::cout << "Column Counts: " << std::endl;
    for (int i = 0; i < zColCounts.size(); i++) {
        std::cout << "Column " << i+1 << " = [";
        for (int j = 0; j < zColCounts[i].size(); j++) {
            std::cout << j << "=" << zColCounts[i][j];
            if (j != zColCounts[i].size()-1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    // Print the count of rows
    std::cout << "Count of Rows with: " << std::endl;
    for (int i = 0; i < zCountRows.size(); i++) {
        std::cout << "Value " << i << " = [";
        for (int j = 0; j < zCountRows[i].size(); j++) {
            std::cout << j << "=" << zCountRows[i][j];
            if (j != zCountRows[i].size()-1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    // Print the count of columns
    std::cout << "Count of Columns with: " << std::endl;
    for (int i = 0; i < zCountCols.size(); i++) {
        std::cout << "Value " << i << " = [";
        for (int j = 0; j < zCountCols[i].size(); j++) {
            std::cout << j << "=" << zCountCols[i][j];
            if (j != zCountCols[i].size()-1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
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
    if (strictMatch || other.strictMatch) {
        // We only need to check the rows here as the vector type will handle the individual values
        for (int i = 0; i < z.size(); i++) {
            if (z[i] != other.z[i]) return false;
        }
    }
    return true;
}

bool zmatrix::operator!=(const zmatrix &other) const {
    return !(*this == other);
}

zmatrix zmatrix::operator*(const zmatrix &rhs) const {
    // Check if the dimensions match for matrix multiplication
    if (z.size() != rhs.z[0].size() || z[0].size() != rhs.z.size()) {
        throw std::invalid_argument("Matrix dimensions do not match");
    }
    zmatrix result;
    for (int i = 0; i < rhs.z.size(); i++) {
        std::vector<int> row;
        for (int j = 0; j < rhs.z[i].size(); j++) {
            // multiply the elements of the row of the first matrix by the elements of the column of the second matrix
            row.push_back(z[i][j] * rhs.z[j][i]);
        }
        result.z.push_back(row);
    }
    return result;
}