#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>

#include "possible-pattern-matrix.hpp"
#include "zmatrix.hpp"


possiblePatternMatrix::possiblePatternMatrix() {
    init();
    updatePairCounts();
}

possiblePatternMatrix::possiblePatternMatrix(int idNum, std::string matrix, bool newEncoding) {
    init();
    id = idNum;
    bool replaceComma = true;
    originalMatrix = matrix; // This is the original matrix string
    for(int i = 0; i < matrix.size(); i++) {
        if (matrix[i] == '{') replaceComma = false;
        if (matrix[i] == '}') replaceComma = true;
        if (replaceComma && matrix[i] == ',') matrix[i] = ';';
        // Ok, going to stick with the old encoding for now to match the pattern matrix implementation
        if (newEncoding) {
            if (matrix[i] == '0') matrix[i] = '0';
            else if (matrix[i] == '1') matrix[i] = '2';
            else if (matrix[i] == '2') matrix[i] = '1';
            else if (matrix[i] == '3') matrix[i] = '3';
        }
    }
    loadFromString(matrix);
}

void possiblePatternMatrix::init() {
    debugOutput = &std::cout;
    id = 0;
    pp = zmatrix(rows, cols, maxValue);
    ppT = zmatrix(cols, rows, maxValue);
    ppNewEncoding = zmatrix(rows, cols, maxValue);
}

void possiblePatternMatrix::loadFromString(std::string m) {
    if (m.size() == 0) throw std::runtime_error("Empty pattern string");
    std::stringstream ms(m);
    std::string r;
    int row = 0;
    while(std::getline(ms, r, ']')) {
        if(row == rows) throw std::runtime_error("Too many rows in pattern string");
        std::stringstream rs(r);
        std::string v;
        int mv;
        int col = 0;
        while(std::getline(rs, v, ';')) {
            if(col == cols) throw std::runtime_error("Too many columns in pattern string");
            if(v[0] == '[') v.erase(0,1);
            
            // NOTE - I'm sticking with the OLD encoding for storage to mirror the pattern matrix
            //    The difference would be that the new encoding would swap 1 and 2
            if(v == "{0}") mv = 0;
            else if (v == "{1}") mv = 1;
            else if (v == "{2}") mv = 2;
            else if (v == "{3}") mv = 3;
            // These would be the new encoding: {0,2}, {1,3}, {0,2,1,3}
            // This is the old encoding which is what we are using for now: {0,1}, {2,3}, {0,1,2,3}
            else if (v == "{0,1}") mv = 4;
            else if (v == "{2,3}") mv = 5;
            else if (v == "{0,1,2,3}") mv = 6;
            else {
                std::ostringstream vErr;
                vErr << "Invalid value in pattern string: " << v ;
                throw std::runtime_error(vErr.str());
            }
            ppNewEncoding.z[row][col] = (mv == 1) ? 2 : (mv == 2) ? 1 : mv;
            pp.z[row][col] = mv;
            ppT.z[col][row] = mv;
            col++;
        }
        if(col != cols) {
            std::ostringstream colErr;
            colErr << "Too few columns in pattern string: " << col << " instead of " << cols;
            throw std::runtime_error(colErr.str());
        }
        row++;
    }
    if(row != rows) {
        std::ostringstream rowErr;
        rowErr << "Too few rows in pattern string: " << row << " instead of " << rows;
        throw std::runtime_error(rowErr.str());
    } 
    pp.updateMetadata();
    ppT.updateMetadata();
    ppNewEncoding.updateMetadata();
    updatePairCounts();
}

void possiblePatternMatrix::updatePairCounts() {
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
                if (pp.z[i][k] == pp.z[j][k]) rowPairCounts[i][j]++;
                if (pp.z[k][i] == pp.z[k][j]) colPairCounts[i][j]++;
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

std::vector<std::vector<std::vector<int>>> possiblePatternMatrix::getPossibleValues() {
    std::vector<std::vector<std::vector<int>>> possibleValues;
    for (int i = 0; i < rows; i++) {
        std::vector<std::vector<int>> rowValues;
        for (int j = 0; j < cols; j++) {
            std::vector<int> values;
            if (pp.z[i][j] == 0) {
                values.push_back(0);
            } else if (pp.z[i][j] == 1) {
                values.push_back(1);
            } else if (pp.z[i][j] == 2) {
                values.push_back(2);
            } else if (pp.z[i][j] == 3) {
                values.push_back(3);
            } else if (pp.z[i][j] == 4) {
                values.push_back(0);
                values.push_back(1);
            } else if (pp.z[i][j] == 5) {
                values.push_back(2);
                values.push_back(3);
            } else if (pp.z[i][j] == 6) {
                values.push_back(0);
                values.push_back(1);
                values.push_back(2);
                values.push_back(3);
            }
            rowValues.push_back(values);
        }
        possibleValues.push_back(rowValues);
    }
    return possibleValues;
}

bool possiblePatternMatrix::isDuplicate(possiblePatternMatrix other) {
    //std::cout << "Checking for duplicates between " << id << " and " << other.id << std::endl;
    if (pp == other.pp) {
        //std::cout << "Pattern " << id << " is a duplicate of " << other.id << std::endl;
        return true;
    }
    if (isTranspose(other)) {
        //std::cout << "Pattern " << id << " is a transpose of " << other.id << std::endl;
        return true;
    }
    return false;
}

bool possiblePatternMatrix::isTranspose(possiblePatternMatrix other) {
    return ppT == other.pp;
}

void possiblePatternMatrix::printPairCounts(std::ostream& os) {
    printRowPairCounts(os);
    printColPairCounts(os);
}

void possiblePatternMatrix::printRowPairCounts(std::ostream& os) {
    os << "Row Pair Counts:" << std::endl;
    for (int i = 0; i < rows; i++) {
        os << "[";
        for (int j = 0; j < rows; j++) {
            os << pp.rowPairCounts[i][j];
            if (j != rows-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
    os << "Row Pair Counts Totals:" << std::endl;
    for (int i = 0; i < pp.rowPairCountsTotals.size(); i++) {
        os << pp.rowPairCountsTotals[i];
        if (i != rows) os << ","; // Don't print a comma after the last element
    }
    if (multilineOutput) os << std::endl;
}

void possiblePatternMatrix::printColPairCounts(std::ostream& os) {
    os << "Column Pair Counts:" << std::endl;
    for (int i = 0; i < cols; i++) {
        os << "[";
        for (int j = 0; j < cols; j++) {
            os << pp.colPairCounts[i][j];
            if (j != cols-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
    os << "Column Pair Counts Totals:" << std::endl;
    for (int i = 0; i < pp.colPairCountsTotals.size(); i++) {
        os << pp.colPairCountsTotals[i];
        if (i != cols) os << ","; // Don't print a comma after the last element
    }
    if (multilineOutput) os << std::endl;
}

void possiblePatternMatrix::printCounts(std::ostream& os) {
    printRowCounts(os);
    printColCounts(os);
    printCountRows(os);
    printCountCols(os);
}

void possiblePatternMatrix::printRowCounts(std::ostream& os) {
    pp.printRowCounts(os);
}

void possiblePatternMatrix::printColCounts(std::ostream& os) {
    pp.printColCounts(os);
}

void possiblePatternMatrix::printCountRows(std::ostream& os) {
    pp.printCountRows(os);
}

void possiblePatternMatrix::printCountCols(std::ostream& os) {
    pp.printCountCols(os);
}

void possiblePatternMatrix::printDebug(std::ostream& os) {
    os << "Possible Pattern ID: " << id << std::endl;
    os << "Possible Pattern Matrix Debug:" << std::endl;
    pp.printDebug(os);
    os << "Transposed Pattern Matrix Debug:" << std::endl;
    ppT.printDebug(os);
}

std::ostream& operator<<(std::ostream &os,const possiblePatternMatrix &ppm) {
    if (ppm.printID) {
        os << ppm.id;
        if (ppm.multilineOutput) os << std::endl;
        else os << " ";
    }
    for (int i = 0; i < ppm.rows; i++) {
        os << "[";
        for (int j = 0; j < ppm.cols; j++) {
            int val = ppm.pp.z[i][j];
            if (ppm.printOldEncoding) {
                if (val == 0) os << "{0}";
                else if (val == 1) os << "{1}";
                else if (val == 2) os << "{2}";
                else if (val == 3) os << "{3}";
                else if (val == 4) os << "{0,1}";
                else if (val == 5) os << "{2,3}";
                else if (val == 6) os << "{0,1,2,3}";
            } else {
                if (val == 0) os << "{0}";
                else if (val == 1) os << "{2}";
                else if (val == 2) os << "{1}";
                else if (val == 3) os << "{3}";
                else if (val == 4) os << "{0,2}";
                else if (val == 5) os << "{1,3}";
                else if (val == 6) os << "{0,2,1,3}";
            }            
            if (j != ppm.cols-1) os << ",";
        }
        os << "]";
        if (ppm.multilineOutput) os << std::endl;
    }
    if (ppm.printOrigins && ppm.origins.size() > 0) {
        os << " Origins: ";
        for (int i = 0; i < ppm.origins.size(); i++) {
            os << ppm.origins[i];
            if (i != ppm.origins.size()-1) os << ", ";
        }
        if (ppm.multilineOutput) os << std::endl;
        else os << " ";
    }
    if (ppm.printLeadsToPatternIDs && ppm.leadsToPatternIDs.size() > 0) {
        os << " Leads to Pattern IDs: ";
        for (int i = 0; i < ppm.leadsToPatternIDs.size(); i++) {
            os << ppm.leadsToPatternIDs[i];
            if (i != ppm.leadsToPatternIDs.size()-1) os << ", ";
        }
        if (ppm.multilineOutput) os << std::endl;
        else os << " ";
    }
    return os;
}
