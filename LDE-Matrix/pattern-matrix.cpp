#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>

#include "case-matrix.hpp"
#include "pattern-matrix.hpp"
#include "zmatrix.hpp"
#include "data/patterns928.hpp"


patternMatrix::patternMatrix() {
    init();
    updatePairCounts();
}

void patternMatrix::init() {
    debugOutput = &std::cout;
    id = 0;
    caseMatch = -1;
    subCaseMatch = '-';
    pNewEncoding = zmatrix(rows, cols, maxValue);
    p = zmatrix(rows, cols, maxValue);
    pT = zmatrix(cols, rows, maxValue);
    swap23 = zmatrix(rows, cols, maxValue);
    swap23T = zmatrix(cols, rows, maxValue);
    cV = zmatrix(rows, cols, 1);
    cVT = zmatrix(cols, rows, 1);
    pGroupings = zmatrix(rows, cols, 36);
    entryLDEs.resize(rows);
    for (int i = 0; i < rows; i++) {
        entryLDEs[i].resize(cols);
    }
    originalMatrix = "[0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0]";
    possibleValues.resize(rows);
    for (int i = 0; i < rows; i++) {
        possibleValues[i].resize(cols);
    }
    rowToRowSet.resize(rows);
    loadCases();
}

// TODO - Refactor so a rearrangement is not needed which means updating the 928 data
patternMatrix::patternMatrix(int pattern928Number) {
    init();
    id = pattern928Number;
    id928 = pattern928Number;
    singleCaseRearrangement = true;
    loadFromString(PATTERNS_928[pattern928Number]);
    rearrangeMatrix();
    loadFromString(getFirstCaseRearrangement());
    rearrangedToMatchCase = true;
    // This could be removed if the p928 patters are updated to match the case style and labelled by case / subcase
    determineSubCase();
}


// This constructor uses the old encoding and should probably be updated or something
//  as the new vs old encoding is a bit confusing
patternMatrix::patternMatrix(int pNum, std::string matrix) {
    init();
    id = pNum;
    loadFromString(matrix);
}

patternMatrix::patternMatrix(int pNum, std::string matrix, bool newEncoding) {
    init();
    id = pNum;
    if (newEncoding) {
        for(int i = 0; i < matrix.size(); i++) {
            if (matrix[i] == '0') matrix[i] = '0';
            else if (matrix[i] == '1') matrix[i] = '2';
            else if (matrix[i] == '2') matrix[i] = '1';
            else if (matrix[i] == '3') matrix[i] = '3';
        }
    }
    loadFromString(matrix);
}

void patternMatrix::loadFromString(std::string m) {
    if (m.size() == 0) throw std::runtime_error("Empty pattern string");
    std::stringstream ms(m);
    std::string r;
    int group = 1;  // Used to track groupings; initially, all entries are in their own group
    int row = 0;
    while(std::getline(ms, r, ']')) {
        if(row == rows) throw std::runtime_error("Too many rows in pattern string");
        std::stringstream rs(r);
        std::string v;
        int mv;
        int col = 0;
        while(std::getline(rs, v, ',')) {
            if(col == cols) throw std::runtime_error("Too many columns in pattern string");
            if(v[0] == '[') v.erase(0,1);
            
            // This set represents the original patterns we got from the creators
            if(v == "0 0") mv = 0;
            else if (v == "0 1") mv = 1;
            else if (v == "1 0") mv = 2;
            else if (v == "1 1") mv = 3;
            // This set represents our representation of the patterns
            else if(v == "0") mv = 0;
            else if (v == "1") mv = 1;
            else if (v == "2") mv = 2;
            else if (v == "3") mv = 3;
            else {
                std::ostringstream vErr;
                vErr << "Invalid value in pattern string: " << v ;
                throw std::runtime_error(vErr.str());
            }
            pNewEncoding.z[row][col] = (mv == 1) ? 2 : (mv == 2) ? 1 : mv;
            p.z[row][col] = mv;
            pT.z[col][row] = mv;
            // Since we know the pattern, the possible values are just the pattern values
            //  This will change after LDE reduction
            possibleValues[row][col].clear();
            possibleValues[row][col].push_back(mv);
            swap23.z[row][col] = (mv == 2) ? 3 : (mv == 3) ? 2 : mv;
            swap23T.z[col][row] = (mv == 2) ? 3 : (mv == 3) ? 2 : mv;
            cV.z[row][col] = mv / 2;
            cVT.z[col][row] = mv / 2;
            pGroupings.z[row][col] = group;
            group++;
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
    p.updateMetadata();
    pT.updateMetadata();
    cV.updateMetadata();
    cVT.updateMetadata();
    swap23.updateMetadata();
    swap23T.updateMetadata();
    pGroupings.updateMetadata();  // This probably isn't necessary but it's good to be consistent
    // Now we know we have a valid matrix string so let's save it
    originalMatrix = toString();
    updatePairCounts();
}

// I need to refactor this to, instead, use the zmatrix pair values
void patternMatrix::updatePairCounts(){
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
                if (p.z[i][k] == p.z[j][k]) rowPairCounts[i][j]++;
                if (p.z[k][i] == p.z[k][j]) colPairCounts[i][j]++;
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

void patternMatrix::matchOnCases() {
    caseMatch = -1;
    for (int i = 0; i < cases.size(); i++) {
        //std::cout << "Matching on case " << cases[i].id << std::endl;
        if (matchesCase(i)) {
            if (caseMatch != -1) {
                std::ostringstream caseErr;
                caseErr << "Pattern " << id << " matches more than 1 case: " << caseMatch << " and " << cases[i].id << std::endl;
                throw std::runtime_error(caseErr.str());
            }
            switch (cases[i].id)
            {
                case 2: {
                    // Case 2 requires that the rows / columns with the 4 case entries are fully paired
                    int rc1 = -1;
                    int rc2 = -1;
                    for (int j = 0; j < rows; j++) {
                        if (p.zRowCounts[j][2] + p.zRowCounts[j][3] == 4) {
                            if (rc1 == -1) rc1 = j;
                            else rc2 = j;
                        }
                    }
                    for (int j = 0; j < cols; j++) {
                        if (p.zColCounts[j][2] + p.zColCounts[j][3] == 4) {
                            if (rc1 == -1) rc1 = j;
                            else rc2 = j;
                        }
                    }
                    // make sure rc1 and rc2 are set
                    if (rowPairCounts[rc1][rc2] == 6) {
                        //std::cout << "RowPairCounts: " << rc1 << " " << rc2 << " " << rowPairCounts[rc1][rc2] << std::endl;
                        caseMatch = cases[i].id;
                    } else if (colPairCounts[rc1][rc2] == 6) {
                        //std::cout << "ColPairCounts: " << rc1 << " " << rc2 << " " << colPairCounts[rc1][rc2] << std::endl;
                        caseMatch = cases[i].id;
                    }
                    break;
                }
                default:
                    caseMatch = cases[i].id;
                    break;
            }
        }
    }
}

bool patternMatrix::matchesCase(int caseIndex) {
    if (cV == cases[caseIndex].c) return true; // pattern matrix case style matches the case matrix
    if (cVT == cases[caseIndex].c) return true; // transposed pattern matrix case style matches the case matrix
    return false;
}

// This might need a refactor into the normal matchOnCases 
//  however it does require the pattern to be rearranged to match the case (I think)
bool patternMatrix::determineSubCase(){
    subCaseMatch = '-';
    updatePairCounts();
    switch (caseMatch)
    {
    case 1:
        return case1SubCaseMatch();
    case 2:
        return case2SubCaseMatch();
    case 3:
        return case3SubCaseMatch();
    case 4:
        return case4SubCaseMatch();
    case 5:
        return case5SubCaseMatch();
    case 6:
        return case6SubCaseMatch();
    case 7:
        return case7SubCaseMatch();
    case 8:
        return case8SubCaseMatch();
    default:
        break;
    }
    return false;
}

bool patternMatrix::case1SubCaseMatch() {
    // Case 1 doesn't have any subcases
    return true;
}

bool patternMatrix::case2SubCaseMatch() {
    // Case 2 doesn't have any subcases
    return true;
}

bool patternMatrix::case3SubCaseMatch() {
    return case3aSubCaseCheck() || case3bSubCaseCheck() || case3cSubCaseCheck();
    /*
    TODO - Either add this back or remove the the comment block
    Keeping this, for now, as the case 3b check might be useful in the future but, for now, it's not.  
    if (case3aSubCaseCheck()) {
        return true;
    }
    if (case3bSubCaseCheck()) {
        
        std::cout << "Pattern " << id << " - Case 3b" << std::endl;
        // Do additional check to make sure that the upper right [4x2] and lower left [2x4] are the same value
        int col42sum = p.z[0][4] + p.z[0][5] + p.z[1][4] + p.z[1][5] + p.z[2][4] + p.z[2][5] + p.z[3][4] + p.z[3][5];
        int row24sum = p.z[4][0] + p.z[5][0] + p.z[4][1] + p.z[5][1] + p.z[4][2] + p.z[5][2] + p.z[4][3] + p.z[5][3];
        if (col42sum == 0 || col42sum == 8 || row24sum == 0 || row24sum == 8) {
            return true;
        }
        std::cout << "Pattern " << id << " - Case 3b isn't paired correctly on the right 2 columns and bottom 2 rows" << std::endl;
        // Reset the case match as this is an invalid state
        caseMatch = -1;
        subCaseMatch = '-';
        return false;
        
    }
    if (case3cSubCaseCheck()) {
        return true;
    }
    // If we don't match a subcase, then we have an invalid case 3 pattern
    caseMatch = -1;
    return false;
    */
}

bool patternMatrix::case3aSubCaseCheck() {
    bool isCase3a = false;
    // 3a: The four columns/rows with odd entries are fully paired
    if ((rowPairCounts[0][1] == 6 && rowPairCounts[2][3] == 6) || 
        (rowPairCounts[0][2] == 6 && rowPairCounts[1][3] == 6) ||
        (rowPairCounts[0][3] == 6 && rowPairCounts[1][2] == 6)) {
        subCaseMatch = 'a';
        isCase3a = true;
    }
    if ((colPairCounts[0][1] == 6 && colPairCounts[2][3] == 6) || 
        (colPairCounts[0][2] == 6 && colPairCounts[1][3] == 6) ||
        (colPairCounts[0][3] == 6 && colPairCounts[1][2] == 6)
        ) {
        subCaseMatch = 'a';
        isCase3a = true;
    }
    return isCase3a;
}

bool patternMatrix::case3bSubCaseCheck() {
    bool isCase3b = false;
    // 3b: In the first four rows/columns, four entries of every row/column are paired.
    if ((rowPairCounts[0][1] == 4 && rowPairCounts[2][3] == 4) ||
        (rowPairCounts[0][2] == 4 && rowPairCounts[1][3] == 4) ||
        (rowPairCounts[0][3] == 4 && rowPairCounts[1][2] == 4)) {
        subCaseMatch = 'b';
        isCase3b = true;
    }
    if ((colPairCounts[0][1] == 4 && colPairCounts[2][3] == 4) || 
        (colPairCounts[0][2] == 4 && colPairCounts[1][3] == 4) ||
        (colPairCounts[0][3] == 4 && colPairCounts[1][2] == 4)) {
        subCaseMatch = 'b';
        isCase3b = true;
    }
    return isCase3b;
}

bool patternMatrix::case3cSubCaseCheck() {
    bool isCase3c = false;
    // 3c: there are only two paired numbers per row/column.
    // 3c: there are only two paired numbers per row/column.
    if ((rowPairCounts[0][1] == 2 && rowPairCounts[2][3] == 2) ||
        (rowPairCounts[0][2] == 2 && rowPairCounts[1][3] == 2) ||
        (rowPairCounts[0][3] == 2 && rowPairCounts[1][2] == 2)) {
        subCaseMatch = 'c';
        isCase3c = true;
    }
    if ((colPairCounts[0][1] == 2 && colPairCounts[2][3] == 2) ||
        (colPairCounts[0][2] == 2 && colPairCounts[1][3] == 2) ||
        (colPairCounts[0][3] == 2 && colPairCounts[1][2] == 2)) {
        subCaseMatch = 'c';
        isCase3c = true;
    }
    return isCase3c;
}

bool patternMatrix::case4SubCaseMatch() {
    // 4a: has two fully paired columns if m11 = m12.
    if ((colPairCounts[0][1] == 6 && colPairCounts[2][3] == 6) || (colPairCounts[0][2] == 6 && colPairCounts[1][3] == 6)) {
        subCaseMatch = 'a';
        return true;
    }
    // 4b: where m11 ̸ = m12
    subCaseMatch = 'b';
    return true;
}

bool patternMatrix::case5SubCaseMatch() {
    // 5a: Either columns (1,2) and (3,4) are fully paired if m11 = m12 and 
    //     m13 = m14, where xT12xT34 is correct T sequence, or rows (1,2) and (3,4) are
    //     fully paired if m11 = m21 and m31 = m41, where T12xT34x reduce k to k-1.
    if ((rowPairCounts[0][1] == 6 && rowPairCounts[2][3] == 6) || (rowPairCounts[0][2] == 6 && rowPairCounts[1][3] == 6)) {
        subCaseMatch = 'a';
        return true;
    }
    if ((colPairCounts[0][1] == 6 && colPairCounts[2][3] == 6) || (colPairCounts[0][2] == 6 && colPairCounts[1][3] == 6)) {
        subCaseMatch = 'a';
        return true;
    }
    // 5b: m11 = m12 and m13 ̸ = m14
    //     I'm adding the case where m33 = m34 and m31 != m32 as this block can be moved into place
    //       This addition might not be necessary
    if((p.z[0][0] == p.z[0][1] && p.z[0][2] != p.z[0][3]) || (p.z[2][2] == p.z[2][3] && p.z[2][0] != p.z[2][1])) {
        subCaseMatch = 'b';
        return true;
    }
    // I don't think this should happen
    return false;
}

bool patternMatrix::case6SubCaseMatch() {
    // 6a: Rows (1,2) and (3,4) are paired
    if (rowPairCounts[0][1] == 6 && rowPairCounts[2][3] == 6) {
        subCaseMatch = 'a';
        return true;
    }
    // 6b: columns (1,2), (3,4), and (5,6) are fully paired
    if (colPairCounts[0][1] == 6 && colPairCounts[2][3] == 6 && colPairCounts[4][5] == 6) {
        subCaseMatch = 'b';
        return true;
    }
    // 6c: In the first four rows, only two paired entries between sets of 2 rows ([1,2 and 3,4] or [1,3 and 2,4] or [1,4 and 2,3])
    if ((rowPairCounts[0][1] == 2 && rowPairCounts[2][3] == 2) || 
        (rowPairCounts[0][2] == 2 && rowPairCounts[1][3] == 2) ||
        (rowPairCounts[0][3] == 2 && rowPairCounts[1][2] == 2)) {
        subCaseMatch = 'c';
        return true;
    }
    // I don't think this should happen
    return false;
}

bool patternMatrix::case7SubCaseMatch() {
    // Maybe N/A??  The single pattern corresponds to I ⊗ H?
    return true;
}

bool patternMatrix::case8SubCaseMatch() {
    // 8a: V11, V12 have the same parity or V11, V21 have the same parity
    // 8b: V11, V12 have different parity and V11, V21 have different parity
    // Assume subcase 'a' and change to 'b' if the parity is different
    subCaseMatch = 'a';
    if (p.z[0][0] % 2 != p.z[0][1] % 2 && p.z[0][0] % 2 != p.z[1][0] % 2) {
        subCaseMatch = 'b';
    }
    return true;
}

std::string patternMatrix::getFirstCaseRearrangement() {
    if (caseRearrangements.size() == 0) return toString();
    return caseRearrangements.begin()->first;
}

bool patternMatrix::isDuplicate(patternMatrix other) {
    //std::cout << "Checking for duplicates between " << id << " and " << other.id << std::endl;
    if (p == other.p) {
        //std::cout << "Pattern " << id << " is a duplicate of " << other.id << std::endl;
        return true;
    }
    if (isTranspose(other)) {
        //std::cout << "Pattern " << id << " is a transpose of " << other.id << std::endl;
        return true;
    }
    if (is23Swap(other)) {
        //std::cout << "Pattern " << id << " is a 2-3 swap of " << other.id << std::endl;
        return true;
    }
    if (is23SwapT(other)) {
        //std::cout << "Pattern " << id << " is a transpose of a 2-3 swap of " << other.id << std::endl;
        return true;
    }
    return false;
}

bool patternMatrix::isTranspose(patternMatrix other) {
    return pT == other.p;
}

bool patternMatrix::is23Swap(patternMatrix other) {
    return swap23 == other.p;
}

bool patternMatrix::is23SwapT(patternMatrix other) {
    return swap23T == other.p;
}

void patternMatrix::printDebug(std::ostream& os) {
    os << "Pattern Number: " << id << std::endl;
    os << "Matched Case Number: " << caseMatch << std::endl;
    os << "Pattern Matrix Debug:" << std::endl;
    p.printDebug(os);
    os << "Transposed Pattern Matrix Debug:" << std::endl;
    pT.printDebug(os);
    os << "Swapped 2s and 3s Pattern Matrix Debug:" << std::endl;
    swap23.printDebug(os);
    os << "Case Style Pattern Matrix Debug:" << std::endl;
    cV.printDebug(os);
    os << "Transposed Case Style Pattern Matrix Debug:" << std::endl;
    cVT.printDebug(os);
}

void patternMatrix::printLDEs(std::ostream& os){
    for (int i = 0; i < rows; i++) {
        os << "[";
        for (int j = 0; j < cols; j++) {
            os << entryLDEs[i][j];
            if (j != cols-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
}

std::string patternMatrix::rowPossibleValueToString(int row) {
    std::ostringstream os;
    os << "[";
    for (int i = 0; i < cols; i++) {
        os << "{";
        for (int j = 0; j < possibleValues[row][i].size(); j++) {
            os << possibleValues[row][i][j];
            if (j != possibleValues[row][i].size()-1) os << ",";
        }
        os << "}";
        if (i != cols-1) os << ",";
    }
    os << "]";
    return os.str();
}

void patternMatrix::printPossibleValues(std::ostream& os) {
    for (int i = 0; i < rows; i++) {
        os << "[";
        for (int j = 0; j < cols; j++) {
            os << "{";
            for (int k = 0; k < possibleValues[i][j].size(); k++) {
                int val = possibleValues[i][j][k];
                if(!printOldEncoding) {
                    val = (val == 1) ? 2 : (val == 2) ? 1 : val;
                }
                os << val;
                if (k != possibleValues[i][j].size()-1) os << ","; // Don't print a comma after the last element
            }
            os << "}";
            if (j != cols-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
}

void patternMatrix::printPairCounts(std::ostream& os) {
    printRowPairCounts(os);
    printColPairCounts(os);
}

void patternMatrix::printRowPairCounts(std::ostream& os) {
    os << "Row Pair Counts:" << std::endl;
    for (int i = 0; i < rows; i++) {
        os << "[";
        for (int j = 0; j < rows; j++) {
            os << p.rowPairCounts[i][j];
            if (j != rows-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
    os << "Row Pair Counts Totals:" << std::endl;
    for (int i = 0; i < p.rowPairCountsTotals.size(); i++) {
        os << p.rowPairCountsTotals[i];
        if (i != rows) os << ","; // Don't print a comma after the last element
    }
    if (multilineOutput) os << std::endl;
}

void patternMatrix::printColPairCounts(std::ostream& os) {
    os << "Column Pair Counts:" << std::endl;
    for (int i = 0; i < cols; i++) {
        os << "[";
        for (int j = 0; j < cols; j++) {
            os << p.colPairCounts[i][j];
            if (j != cols-1) os << ","; // Don't print a comma after the last element
        }
        os << "]";
        if (multilineOutput) os << std::endl;
    }
    os << "Column Pair Counts Totals:" << std::endl;
    for (int i = 0; i < p.colPairCountsTotals.size(); i++) {
        os << p.colPairCountsTotals[i];
        if (i != cols) os << ","; // Don't print a comma after the last element
    }
    if (multilineOutput) os << std::endl;
}

void patternMatrix::printCounts(std::ostream& os) {
    printRowCounts(os);
    printColCounts(os);
    printCountRows(os);
    printCountCols(os);
}

void patternMatrix::printRowCounts(std::ostream& os) {
    p.printRowCounts(os);
}

void patternMatrix::printColCounts(std::ostream& os) {
    p.printColCounts(os);
}

void patternMatrix::printCountRows(std::ostream& os) {
    p.printCountRows(os);
}

void patternMatrix::printCountCols(std::ostream& os) {
    p.printCountCols(os);
}

std::string patternMatrix::printTGateOperations() {
    std::ostringstream os;
    for (int i = 0; i < tGateOperations.size(); i++) {
        os << tGateOperations[i];
        if(i != tGateOperations.size()-1) os << " ";
    }
    return os.str();
}

std::string patternMatrix::printTGateOperationSets() {
    std::ostringstream os;
    for (int i = 0; i < tGateOperationSets.size(); i++) {
        for (int j = 0; j < tGateOperationSets[i].size(); j++) {
            os << tGateOperationSets[i][j];
            if(j != tGateOperationSets[i].size()-1) os << ",";
        }
        if(i != tGateOperationSets.size()-1) os << " ";
    }
    return os.str();
}

std::string patternMatrix::getMaxOfPossibleValues() {
    std::string maxValues = "";
    for (int i = 0; i < rows; i++) {
        maxValues += "[";
        for (int j = 0; j < cols; j++) {
            int max = 0;
            for (int k = 0; k < possibleValues[i][j].size(); k++) {
                if (possibleValues[i][j][k] > max) max = possibleValues[i][j][k];
            }
            if(!printOldEncoding) {
                    max = (max == 1) ? 2 : (max == 2) ? 1 : max;
            }
            maxValues += std::to_string(max);
            if (j != cols-1) maxValues += ","; // Don't print a comma after the last element
        }
        maxValues += "]";
    }
    return maxValues;
}

bool patternMatrix::possibleValuesLeadToAllPatterns(){
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (possibleValues[i][j].size() != 4) return false;
        }
    }
    return true;
}

void patternMatrix::generateAllPossibleValuePatterns() {
    if (possibleValuesLeadToAllPatterns()) {
        *debugOutput << "All possible values lead to all patterns" << std::endl;
        for (int i = 1; i <= 928; i++) {
            // Load all of the patterns into the allPossibleValuePatterns map
            allPossibleValuePatterns[patternMatrix(i).toString()] = true;
        }
        return;
    }
    // Need to iterate through all possible values and generate a unique pattern for each combination
    //  This pattern will be stored in allPossibleValuePatterns
    // This will be very similar to generating all possible patterns, so we can use that as a base
    allPossibleValuePatterns.clear();
    zmatrix z = zmatrix(rows, cols, maxValue);
    recursiveAllPossibleValueSet(0, z);
}

void patternMatrix::recursiveAllPossibleValueSet(int position, zmatrix z) {
    if (singleValidPattern && allPossibleValuePatterns.size() > 0) {
        // If we have a single valid pattern, then we can skip this
        return;
    }
    if (position == rows*cols) return;
    for (int i = 0; i < possibleValues[position / cols][position % cols].size(); i++) {
        z.z[position / cols][position % cols] = possibleValues[position / cols][position % cols][i];
        if (position == (rows * cols) - 1) {
            std::ostringstream os;
            os << z;
            patternMatrix pm = patternMatrix(1, os.str());
            pm.matchOnCases();
            if (pm.caseMatch > 0 && pm.isOrthogonal() && pm.isNormalized()) {
                if (printDebugInfo) {
                    *debugOutput << "Standard Version - Case: " << pm.caseMatch << " Valid Pattern:" << pm << " Count: " << allPossibleValuePatterns.size()+1 << std::endl;
                }
                allPossibleValuePatterns[pm.toString()] = true;
            }
        }
        recursiveAllPossibleValueSet(position + 1, z);
    }
}

void patternMatrix::optimizedGenerateAllPossibleValuePatterns() {
    if (possibleValuesLeadToAllPatterns()) {
        *debugOutput << "All possible values lead to all patterns" << std::endl;
        for (int i = 1; i <= 928; i++) {
            // Load all of the patterns into the allPossibleValuePatterns map
            allPossibleValuePatterns[patternMatrix(i).toString()] = true;
        }
        return;
    }
    // Need to iterate through all possible values and generate a unique pattern for each combination
    //  This pattern will be stored in allPossibleValuePatterns
    // This will be very similar to generating all possible patterns, so we can use that as a base
    allPossibleValuePatterns.clear();
    zmatrix z = zmatrix(rows, cols, maxValue);
    optimizedAllPossibleValuePatterns(0, z);
}

void patternMatrix::optimizedAllPossibleValuePatterns(int position, zmatrix z) {
    // If we only want to find a single valid pattern and exit, this handles that
    if (singleValidPattern && allPossibleValuePatterns.size() > 0) {
        // If we have a single valid pattern, then we can skip this
        return;
    }
    // pick rows / columns with fewest possible values (2 preferred)
    // check normality on a per row / row or column / column
    //check if row is normalized, if not, it's not a valid set
    //do this for each row as you go, so for the recursive set, check ever 6 items
    if (position == rows*cols) return;
    int curRow = position / cols;
    int curColumn = position % cols;
    for (int i = 0; i < possibleValues[curRow][curColumn].size(); i++) {    
        z.z[curRow][curColumn] = possibleValues[curRow][curColumn][i];
        if (position != 0 && curColumn == 0) {
            // here, we are at position 6,12,18,24,30,36 which means that we are at the start of a row
            //  we need to check the normality of the prior row
            //  if the prior row is not normalized, then we can skip this set
            if(!isRowNormalized(curRow - 1, z)) return;
            // if we have enough rows, we can check for orthogonality between rows
            if (curRow >= 2) {
                // If curRow == 2, then rows 0, 1 have been set and we can check the orthogonality of them
                // this for loop might not be necessary if the following is true
                //  r1 orthogonal to r2 and r3 orthogonal to r2 => r1 orthogonal to r3
                //  I don't think it is and will check later
                for (int j = 0; j < curRow - 1; j++) {
                    if(!areRowsOrthogonal(curRow - 1, j, z)) return;
                }
            }
        }
        if (position == (rows * cols) - 1) {
            std::ostringstream os;
            os << z;
            patternMatrix pm = patternMatrix(1, os.str());
            pm.matchOnCases();
            if (pm.caseMatch > 0 && pm.isOrthogonal() && pm.isNormalized()) {
                if (printDebugInfo) {
                    *debugOutput << "Optimized Version - Case: " << pm.caseMatch << " Valid Pattern: " << pm << " Count: " << allPossibleValuePatterns.size()+1 << std::endl;
                }
                allPossibleValuePatterns[pm.toString()] = true;
            } else {
                if (printDebugInfo) {
                    *debugOutput << "Optimized Version - Case: " << pm.caseMatch << " Invalid Pattern: " << pm << std::endl;
                }
            }
        }
        optimizedAllPossibleValuePatterns(position + 1, z);
    }
}

void patternMatrix::generateRowSet(int pvRow, int rsPos, std::vector<int> newRow, int pos) {
    if (pos == 6) {
        // Check if the row is normalized
        if (isRowNormalized(newRow)) {
            possiblePatternRowSets[rsPos].push_back(newRow);
            if (printDebugInfo) {
                // using size minus one to get the index of the last element for the row output
                *debugOutput << "Row Set: " << rsPos << " Row: " << possiblePatternRowSets[rsPos].size() - 1 << " [";
                for (int i = 0; i < newRow.size(); i++) {
                    int v = newRow[i];
                    if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                    *debugOutput << v;
                    if (i != newRow.size()-1) *debugOutput << ",";
                }
                *debugOutput << "]" << std::endl;
            }
            
        }
        return;
    }
    for (int i = 0; i < possibleValues[pvRow][pos].size(); i++) {
        newRow[pos] = possibleValues[pvRow][pos][i];
        generateRowSet(pvRow, rsPos, newRow, pos + 1);
    }
}

// This version will create sets of rows that are normalized, check orthogonality, and then generate patterns
void patternMatrix::opt2GenerateAllPossibleValuePatterns() {
    if (possibleValuesLeadToAllPatterns()) {
        *debugOutput << "All possible values lead to all patterns" << std::endl;
        for (int i = 1; i <= 928; i++) {
            // Load all of the patterns into the allPossibleValuePatterns map
            allPossibleValuePatterns[patternMatrix(i).toString()] = true;
        }
        return;
    }
    allPossibleValuePatterns.clear();
    if (printDebugInfo) {
        *debugOutput << "Normalized Rows:" << std::endl;
    }
    // First, create the sets of normalized rows
    int setNum = 0;
    for (int i = 0; i < rows; i++) {
        std::string key = rowPossibleValueToString(i);
        if (rowSetStringToIntID.find(key) == rowSetStringToIntID.end()) {
            rowSetStringToIntID[key] = setNum;
            possiblePatternRowSets.push_back(std::vector<std::vector<int>>());
            // Now, we need to create rows and check normality
            generateRowSet(i, setNum, std::vector<int>(6), 0);
            setNum++;
        } 
        rowToRowSet[i] = rowSetStringToIntID[key];
    }
    if (printDebugInfo) {
        *debugOutput << "Total Row Sets: " << possiblePatternRowSets.size() << std::endl;
    }

    if (printDebugInfo) {
        *debugOutput << "Orthogonality of Row Sets:" << std::endl;
    }
    // To try to add some clarity, rsi = row set i, rsj = row set j, ri = row i from rsi, rj = row j from rsj
    for (int rsi = 0; rsi < possiblePatternRowSets.size(); rsi++) {
        for (int rsj = rsi; rsj < possiblePatternRowSets.size(); rsj++) {
            for (int ri = 0; ri < possiblePatternRowSets[rsi].size(); ri++) {
                for (int rj = 0; rj < possiblePatternRowSets[rsj].size(); rj++) {
                    // skip if the comparison has already been done
                    // TODO: This isn't correct as it's still doing a comparison to all rows
                    if (rsj < rsi && rj < ri) continue;
                    std::string lhs = std::to_string(rsi) + "-" + std::to_string(ri);
                    std::string rhs = std::to_string(rsj) + "-" + std::to_string(rj);
                    std::string orthoKey1 = lhs + ":" + rhs;
                    std::string orthoKey2 = rhs + ":" + lhs;
                    // This will make look ups easier in the future
                    if (lhs == rhs) {
                        rowSetOrthogonality[orthoKey1] = true;
                        continue;
                    }
                    bool isOrthogonal = areRowsOrthogonal(possiblePatternRowSets[rsi][ri], possiblePatternRowSets[rsj][rj]);
                    rowSetOrthogonality[orthoKey1] = isOrthogonal;
                    rowSetOrthogonality[orthoKey2] = isOrthogonal;
                    if (printDebugInfo) {
                        *debugOutput << "Row Set: " << rsi << " Row: " << ri << " [";
                        for (int i = 0; i < possiblePatternRowSets[rsi][ri].size(); i++) {
                            int v = possiblePatternRowSets[rsi][ri][i];
                            if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                            *debugOutput << v;
                            if (i != possiblePatternRowSets[rsi][ri].size()-1) *debugOutput << ",";
                        }
                        *debugOutput << "] is ";
                        *debugOutput << " " << (isOrthogonal ? "Orthogonal" : "Not Orthogonal") << " to ";
                        *debugOutput << "Row Set: " << rsj << " Row: " << rj << " [";
                        for (int j = 0; j < possiblePatternRowSets[rsj][rj].size(); j++) {
                            int v = possiblePatternRowSets[rsj][rj][j];
                            if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                            *debugOutput << v;
                            if (j != possiblePatternRowSets[rsj][rj].size()-1) *debugOutput << ",";
                        }
                        *debugOutput << "]" << std::endl;
                    }
                }
            }
        }
    }
    // Finally, generate the patterns by iterating through the row sets and trying different combinations
    //  This will be similar to the recursive function but will use the row sets instead of the rows
    if (printDebugInfo) {
        *debugOutput << "Generating Patterns:" << std::endl;
    }
    recursiveRowSetPatternGeneration(0, std::vector<std::string>(6));
}

void patternMatrix::recursiveRowSetPatternGeneration(int curRow, std::vector<std::string> rowSelections) {
    // If we only want to find a single valid pattern and exit, this handles that
    if (singleValidPattern && allPossibleValuePatterns.size() > 0) {
        // If we have a single valid pattern, then we can skip this
        return;
    }
    if (curRow == rows) return;
    for (int i = 0; i < possiblePatternRowSets[rowToRowSet[curRow]].size(); i++) {
        rowSelections[curRow] = std::to_string(rowToRowSet[curRow]) + "-" + std::to_string(i);
        // Check orthogonality of the current row selection
        bool validRowSelection = true;
        for (int j = 0; j < curRow; j++) {
            if (!rowSetOrthogonality[rowSelections[curRow] + ":" + rowSelections[j]]) {
                validRowSelection = false;
                break;
            }
        }
        if (!validRowSelection) continue;
        if (curRow == rows - 1) {
            // We have a valid set of rows, now we need to generate the pattern
            std::ostringstream os;
            zmatrix z = zmatrix(rows, cols, maxValue);
            for (int j = 0; j < rowSelections.size(); j++) {
                std::string rs = rowSelections[j];
                int rowSet = std::stoi(rs.substr(0, rs.find("-")));
                int rowSelection = std::stoi(rs.substr(rs.find("-")+1));
                for (int k = 0; k < possiblePatternRowSets[rowSet][rowSelection].size(); k++) {
                    z.z[j][k] = possiblePatternRowSets[rowSet][rowSelection][k];
                }
            }
            os << z;
            patternMatrix pm = patternMatrix(1, os.str());
            pm.matchOnCases();
            int cM = pm.caseMatch;
            bool isOrtho = pm.isOrthogonal();
            bool isNorm = pm.isNormalized();
            if (cM > 0 && isOrtho && isNorm) {
                if (printDebugInfo) {
                    *debugOutput << "Valid Pattern: " << pm << " Case Match: " << cM << " Count: " << allPossibleValuePatterns.size()+1 << " [Opt2]" << std::endl;
                }
                allPossibleValuePatterns[pm.toString()] = true;
            } else {
                if (printDebugInfo) {
                    *debugOutput << "Invalid Pattern: " << pm << " Case Match: " << cM;
                    *debugOutput << " Is " << (isOrtho ? "Orthogonal" : "Not Orthogonal");
                    *debugOutput << " Is " << (isNorm ? "Normalized" : "Not Normalized");
                    *debugOutput << " [Opt2]" << std::endl;
                }
            }
        }
        recursiveRowSetPatternGeneration(curRow + 1, rowSelections);
    }
}

// TODO - Add a note on which pattern encoding is being used
// TODO - Refactor this mess, especially the new vs old encoding
std::ostream& operator<<(std::ostream& os,const patternMatrix &pm) {
    // Could do this differently by using flags to set the output format
    if (pm.printID) {
        os << pm.id;
    }
    if (pm.printCaseMatch) {
        os << pm.caseMatch;
    }
    if (pm.printAllIDs) {
        os << pm.id2704 << " " << pm.id928 << " " << pm.id785;
    }
    if (pm.multilineOutput) {
        if (pm.printID || pm.printCaseMatch || pm.printAllIDs) {
            os << std::endl;
        }
        if (pm.printOldEncoding) {
            zmatrix temp = pm.p;
            temp.multilineOutput = true;
            os << temp;
        } else {
            zmatrix temp = pm.pNewEncoding;
            temp.multilineOutput = true;
            os << temp;
        }
    } else {
        if (pm.printID || pm.printCaseMatch || pm.printAllIDs) os << " ";
        if (pm.printOldEncoding) {
            os << pm.p;
        } else {
            os << pm.pNewEncoding;
        }
    }
    return os;
}

// Do something with factorization of the sqrt(2) and the modulo 2 addition
//  Need to make sure that we handle all of the different outcomes as some may produce multiple
//   See your notes for that.


// ====== MULTIPLICATION BY T-GATES ======
// TODO: THIS IS FOR BOTH LEFT AND RIGHT MULTIPLICATION
// TODO: Track LDEs during this process
// TODO: Verify that columns and rows are orthogonal
//  col(i) * col(j) -> i==j then 2^k, i!=j then 0 (covers both orthogonal and normalized)
//  row(i) * row(j) -> i==j then 2^k, i!=j then 0 (covers both orthogonal and normalized)
// TODO: Normalized columns and rows = 2^k

// Left T-gate multiplication means adding rows p, q and replacing both with the result
void patternMatrix::leftTGateMultiply(int pRow, int qRow) {
    tGateOperations.push_back("T" + std::to_string(pRow) + std::to_string(qRow) + "x");
    // Since we, humans, are 1-indexed, we need to subtract 1 from the row numbers
    --pRow;
    --qRow;
    //  Handle left T-gate multiplication
    //  This means adding rows p, q and replacing both with the result
    //  This might be different when p > q
    int result;
    for (int i = 0; i < p.z[pRow].size(); i++) {
        result = patternElementAddition(p.z[pRow][i], p.z[qRow][i]);
        p.z[pRow][i] = result;
        p.z[qRow][i] = result;
        pNewEncoding.z[pRow][i] = (result == 1) ? 2 : (result == 2) ? 1 : result;
        pNewEncoding.z[qRow][i] = (result == 1) ? 2 : (result == 2) ? 1 : result;
        // Update the LDE
        entryLDEs[pRow][i]++;
        entryLDEs[qRow][i]++;
        // Groupings - Going to use the lower group number of the two
        //  It might not matter but might also make it easier for humans to read
        if (pGroupings.z[pRow][i] > pGroupings.z[qRow][i]) {
            pGroupings.z[pRow][i] = pGroupings.z[qRow][i];
        } else {
            pGroupings.z[qRow][i] = pGroupings.z[pRow][i];
        }
    }
    
}

// Right T-gate multiplication means adding columns p, q and replacing both with the result
void patternMatrix::rightTGateMultiply(int pCol, int qCol) {
    tGateOperations.push_back("xT" + std::to_string(pCol) + std::to_string(qCol));
    // Since we, humans, are 1-indexed, we need to subtract 1 from the column numbers
    --pCol;
    --qCol;
    //  Handle right T-gate multiplication
    //  This means adding column p, q and replacing both with the result
    //  This might be different when p > q
    int result;
    for (int i = 0; i < p.z.size(); i++) {
        result = patternElementAddition(p.z[i][pCol], p.z[i][qCol]);
        p.z[i][pCol] = result;
        p.z[i][qCol] = result;
        pNewEncoding.z[i][pCol] = (result == 1) ? 2 : (result == 2) ? 1 : result;
        pNewEncoding.z[i][qCol] = (result == 1) ? 2 : (result == 2) ? 1 : result;
        // Update the LDE
        entryLDEs[i][pCol]++;
        entryLDEs[i][qCol]++;
        // Groupings - Going to use the lower group number of the two
        //  It might not matter but might also make it easier for humans to read
        if (pGroupings.z[i][pCol] <= pGroupings.z[i][qCol]) {
            pGroupings.z[i][pCol] = pGroupings.z[i][qCol];
        } else {
            pGroupings.z[i][qCol] = pGroupings.z[i][pCol];
        }
    }
}


// This will attempt to find optimal t-gate multiplication sets for the pattern matrix.
//   The pattern should be rearranged to match the case prior to this being called but, if not,
//    the code will attempt to rearrange it to match the case.
bool patternMatrix::findOptimalTGateOperations() {
    if (!rearrangedToMatchCase) {
        bool oldSCR = singleCaseRearrangement;
        singleCaseRearrangement = true;
        if(!rearrangeMatrix()) return false;  // If we can't rearrange the matrix, then we can't apply t-gates
        loadFromString(getFirstCaseRearrangement());
        rearrangedToMatchCase = true;
        singleCaseRearrangement = oldSCR;
        determineSubCase();  // This should be useful later
    }
    //std::vector<std::vector<std::string>> optimalTGateOperations
    tGateOperationSets.clear();
    // This will attempt to find optimal t-gate multiplication sets for the pattern matrix
    switch (caseMatch)
    {
    case 1:
        return optimalTGatesCase1();
        break;
    case 2:
        return optimalTGatesCase2();
        break;
    case 3:
        return optimalTGatesCase3();
        break;
    case 4:
        return optimalTGatesCase4();
        break;
    case 5:
        return optimalTGatesCase5();
        break;
    case 6:
        return optimalTGatesCase6();
        break;
    case 7:
        return optimalTGatesCase7();
        break;
    case 8:
        return optimalTGatesCase8();
        break;
    default:
        break;
    }
    return false;
}

bool patternMatrix::optimalTGatesCase1() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    bool optimalsFound = false;
    std::vector<int> caseValues = {2,3};
    // Case 1 has to go back to the origin, so one of options must have 2 case pairs 
    //  and we can skip a loop for the case pairs
    for (int pairs= 6; pairs > 0; pairs = pairs - 2) {
        if (p.getColPairValuesCount(0, 1, caseValues) == 2 && p.colPairCounts[0][1] == pairs) {
            std::vector<std::string> tGateOps;
            tGateOperationSets.push_back({"xT12"});
            optimalsFound = true;
        }
        if (p.getRowPairValuesCount(0, 1, caseValues) == 2 && p.rowPairCounts[0][1] == pairs) {
            std::vector<std::string> tGateOps;
            tGateOperationSets.push_back({"T12x"});
            optimalsFound = true;
        }
        if (optimalsFound) return true;
    }
    return false;
}

bool patternMatrix::optimalTGatesCase2() {
    /* 2: IF the first two columns must be fully paired
    if (p.colPairCounts[0][1] != 6) {
        return false;
    }
    */
    tGateOperationSets.push_back({"xT12"});
    return true;
}

bool patternMatrix::optimalTGatesCase3() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    bool optimalsFound = false;
    std::vector<int> caseValues = {2,3};
    // We need to prefer case pairings over overall pairs
    for (int cpairs = 4; cpairs > 0; cpairs = cpairs-2) {
        // The maximum overall pairings can only be +2 from the case pairings
        for (int pairs= cpairs + 2; pairs >= cpairs; pairs = pairs - 2) {
            if ((p.getColPairValuesCount(0, 1, caseValues) == cpairs || p.getColPairValuesCount(2, 3, caseValues) == cpairs) &&
                (p.colPairCounts[0][1] == pairs || p.colPairCounts[2][3] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("xT12");
                tGateOps.push_back("xT34");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if ((p.getColPairValuesCount(0, 2, caseValues) == cpairs || p.getColPairValuesCount(1, 3, caseValues) == cpairs) &&
                (p.colPairCounts[0][2] == pairs || p.colPairCounts[1][3] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("xT13");
                tGateOps.push_back("xT24");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if ((p.getColPairValuesCount(0, 3, caseValues) == cpairs || p.getColPairValuesCount(1, 2, caseValues) == cpairs) &&
                (p.colPairCounts[0][3] == pairs || p.colPairCounts[1][2] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("xT14");
                tGateOps.push_back("xT23");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            // Next check for fully paired rows
            if ((p.getRowPairValuesCount(0, 1, caseValues) == cpairs || p.getRowPairValuesCount(2, 3, caseValues) == cpairs) &&
                (p.rowPairCounts[0][1] == pairs || p.rowPairCounts[2][3] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("T12x");
                tGateOps.push_back("T34x");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if ((p.getRowPairValuesCount(0, 2, caseValues) == cpairs || p.getRowPairValuesCount(1, 3, caseValues) == cpairs) &&
                (p.rowPairCounts[0][2] == pairs || p.rowPairCounts[1][3] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("T13x");
                tGateOps.push_back("T24x");
                tGateOperationSets.push_back(tGateOps);
            }
            if ((p.getRowPairValuesCount(0, 3, caseValues) == cpairs || p.getRowPairValuesCount(1, 2, caseValues) == cpairs) &&
                (p.rowPairCounts[0][3] == pairs || p.rowPairCounts[2][3] == pairs)) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("T14x");
                tGateOps.push_back("T23x");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if (optimalsFound) return true;
        }
    }
    return false;
}

// This is similar to case 1
bool patternMatrix::optimalTGatesCase4() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    std::vector<std::string> tGateOps;
    std::vector<int> caseValues = {2,3};
    if (p.getRowPairValuesCount(0, 1, caseValues) > p.getColPairValuesCount(0, 1, caseValues)) {
        tGateOps.push_back("T12x");
    } else {
        tGateOps.push_back("xT12");
    }
    tGateOperationSets.push_back(tGateOps);
    return true;
}

// This is similar to case 1 but with another option for the middle block
bool patternMatrix::optimalTGatesCase5() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    return false; // When this is fixed, remove this line
    bool optimalsFound = false;
    std::vector<int> caseValues = {2,3};
    // There are 2 blocks of case values, one in the upper left and one in the middle
    //   We want to find the combinations with the most pairs
    //    block1: r1, r2 and/or c1, c2
    //    block2: r3, r4 and/or c3, c4
    // cpairs is either 2 or 0; I'm not sure if 0 is really possible but I'll leave it for now
    for (int cpairs = 2; cpairs >= 0; cpairs = cpairs-2) {
        // The maximum overall pairings can only be +4 from the case pairings
        for (int pairs= cpairs + 4; pairs >= cpairs; pairs = pairs - 2) {
            if (p.getColPairValuesCount(0, 1, caseValues) == cpairs && p.colPairCounts[0][1] == pairs) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("xT12");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if (p.getColPairValuesCount(2, 3, caseValues) == cpairs && p.colPairCounts[2][3] == pairs) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("xT34");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if (p.getRowPairValuesCount(0, 1, caseValues) == cpairs && p.rowPairCounts[0][1] == pairs) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("T12x");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if (p.getRowPairValuesCount(2, 3, caseValues) == cpairs && p.rowPairCounts[2][3] == pairs) {
                std::vector<std::string> tGateOps;
                tGateOps.push_back("T34x");
                tGateOperationSets.push_back(tGateOps);
                optimalsFound = true;
            }
            if (optimalsFound) return true;
        }
    }
}

bool patternMatrix::optimalTGatesCase6() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    return false; // When this is fixed, remove this line
    // UPDATE THIS
    // Either column 1,2
    //  or rows 1,2 / 3,4
    std::vector<int> caseValues = {2,3};
    // From the original paper, this is a left T(3,4)
    leftTGateMultiply(3, 4);
    return true;
}

// This is similar to case 5 with an another option for a bottom left block
bool patternMatrix::optimalTGatesCase7() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    return false; // When this is fixed, remove this line
    std::vector<int> caseValues = {2,3};
    int rc1 = 0;
    int rc2 = 1;
    if (p.colPairCounts[2][3] > p.colPairCounts[0][1] || p.rowPairCounts[2][3] > p.rowPairCounts[0][1]) {
        rc1 = 2;
        rc2 = 3;
    }
    if (p.colPairCounts[4][5] > p.colPairCounts[rc1][rc2] || p.rowPairCounts[4][5] > p.rowPairCounts[rc1][rc2]) {
        rc1 = 4;
        rc2 = 5;
    }
    if (p.colPairCounts[rc1][rc2] > p.rowPairCounts[rc1][rc2]) {
        rightTGateMultiply(rc1+1, rc2+1);
        return true;
    }
    leftTGateMultiply(rc1+1, rc2+1);
    return true;
}

bool patternMatrix::optimalTGatesCase8() {
    return false; // check for symmetric patterns, if they are, then xT12 == T12x
    return false; // When this is fixed, remove this line
    std::vector<int> caseValues = {2,3};
    // 8: V11, V12 have the same parity -> right T(1,2) T(3,4) T(5,6)
    if (p.z[0][0] % 2 == p.z[0][1] % 2) {
        rightTGateMultiply(1, 2);
        rightTGateMultiply(3, 4);
        rightTGateMultiply(5, 6);
    }
    // 8: V11, V21 have the same parity -> left T(1,2) T(3,4) T(5,6)
    if (p.z[0][0] % 2 != p.z[1][0] % 2) {
        leftTGateMultiply(1, 2);
        leftTGateMultiply(3, 4);
        leftTGateMultiply(5, 6);
    }
    // 8: V11, V12 have different parity and V11, V21 have different parity
    //   We don't have a path forward at this time
    return false;
}


bool patternMatrix::findAllTGateOptions() {
    // If we haven't rearranged the matrix to match the case, then we need to do that first
    if (!rearrangedToMatchCase) {
        bool oldSCR = singleCaseRearrangement;
        singleCaseRearrangement = true;
        if(!rearrangeMatrix()) return false;  // If we can't rearrange the matrix, then the t-gates selected won't be correct
        loadFromString(getFirstCaseRearrangement());
        rearrangedToMatchCase = true;
        singleCaseRearrangement = oldSCR;
        determineSubCase();  // This should be useful later
    }
    tGateOperationSets.clear();
    switch (caseMatch)
    {
    case 1:
        return allTGatesCase1();
        break;
    case 2:
        return allTGatesCase2();
        break;
    case 3:
        return allTGatesCase3();
        break;
    case 4:
        return allTGatesCase4();
        break;
    case 5:
        return allTGatesCase5();
        break;
    case 6:
        return allTGatesCase6();
        break;
    case 7:
        return allTGatesCase7();
        break;
    case 8:
        return allTGatesCase8();
        break;
    default:
        break;
    }
    return false;
}

bool patternMatrix::allTGatesCase1() {
    // Column operations
    tGateOperationSets.push_back({"xT12"});
    // Row operations
    if (!isSymmetric()) {
        tGateOperationSets.push_back({"T12x"});
    }
    return true;
}

bool patternMatrix::allTGatesCase2() {
    // Columns
    tGateOperationSets.push_back({"xT12"});
    return true;
}

bool patternMatrix::allTGatesCase3() {
    // Columns
    tGateOperationSets.push_back({"xT12", "xT34"});
    tGateOperationSets.push_back({"xT13", "xT24"});
    tGateOperationSets.push_back({"xT14", "xT23"});
    // Additional sets if we want to include single operations
    //tGateOperationSets.push_back({"xT12"});
    //tGateOperationSets.push_back({"xT13"});
    //tGateOperationSets.push_back({"xT14"});
    //tGateOperationSets.push_back({"xT23"});
    //tGateOperationSets.push_back({"xT24"});
    //tGateOperationSets.push_back({"xT34"});
    
    // Rows
    if (!isSymmetric()) {
        tGateOperationSets.push_back({"T12x", "T34x"});
        tGateOperationSets.push_back({"T13x", "T24x"});
        tGateOperationSets.push_back({"T14x", "T23x"});
        // Additional sets if we want to include single operations
        //tGateOperationSets.push_back({"T12x"});
        //tGateOperationSets.push_back({"T13x"});
        //tGateOperationSets.push_back({"T14x"});
        //tGateOperationSets.push_back({"T23x"});
        //tGateOperationSets.push_back({"T24x"});
        //tGateOperationSets.push_back({"T34x"});
    }
    return true;
}

bool patternMatrix::allTGatesCase4() {
    // Columns
    tGateOperationSets.push_back({"xT12"});
    // Rows
    if (!isSymmetric()) {
        tGateOperationSets.push_back({"T12x"});
    }
    return true;
}

bool patternMatrix::allTGatesCase5() {
    // Columns
    tGateOperationSets.push_back({"xT12"});
    tGateOperationSets.push_back({"xT34"});
    tGateOperationSets.push_back({"xT12", "xT34"});
    // Rows
    if (!isSymmetric()) {
        tGateOperationSets.push_back({"T12x"});
        tGateOperationSets.push_back({"T34x"});
        tGateOperationSets.push_back({"T12x", "T34x"});
    }
    return true;
}

bool patternMatrix::allTGatesCase6() {
    // Rows
    //tGateOperationSets.push_back({"T12x", "T34x"});  // In the event this will clear everything
    tGateOperationSets.push_back({"T12x"});
    tGateOperationSets.push_back({"T34x"});
    // Columns
    tGateOperationSets.push_back({"xT12"});
    return true;
}

bool patternMatrix::allTGatesCase7() {
    // Only going to include sets of 1x operation as these will lead to other cases
    // Columns
    tGateOperationSets.push_back({"xT12"});
    tGateOperationSets.push_back({"xT34"});
    tGateOperationSets.push_back({"xT56"});
    // Rows
    if (!isSymmetric()) {
        tGateOperationSets.push_back({"T12x"});
        tGateOperationSets.push_back({"T34x"});
        tGateOperationSets.push_back({"T56x"});
    }
    return true;
}

bool patternMatrix::allTGatesCase8() {
    // Using the id to determine what to do with individual case 8b's
    //   and assume that anything left over is case 8a
    switch (id)
    {
        // All of these are case 8b
        case 880: // M880 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        case 881: // M881 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        case 882: // M882 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        case 883: // M883 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        case 884: // M884 : xT13, xT24 followed by T36x, T45x.
            tGateOperationSets.push_back({"xT13", "xT24", "T36x", "T45x"});
            break;
        case 885: // M885 : xT13, xT24 followed by T36X, T45X.
            tGateOperationSets.push_back({"xT13", "xT24", "T36x", "T45x"});
            break;
        case 926: // M926 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        case 927: // M927 : xT13, xT24 followed by T35x, T46x.
            tGateOperationSets.push_back({"xT13", "xT24", "T35x", "T46x"});
            break;
        // Assuming that we only have case 8a left
        default:
            // 8: V11, V12 have the same parity -> right T(1,2) T(3,4) T(5,6)
            if (p.z[0][0] % 2 == p.z[0][1] % 2) {
                tGateOperationSets.push_back({"xT12", "xT34", "xT56"});
            }
            // 8: V11, V21 have the same parity -> left T(1,2) T(3,4) T(5,6)
            if (!isSymmetric()) {
                if (p.z[0][0] % 2 != p.z[1][0] % 2) {
                    tGateOperationSets.push_back({"T12x", "T34x", "T56x"});
                }
            }
            break;
    }
    return true;
}

// LDE Reduction
/*
Resulting value
0 -> {0,1,2,3} and k-1
1 -> {2,3} and k
2 -> {2} and k+1
3 -> {3} and k+1

// Need to update this....

*/
void patternMatrix::ldeReductionOnEntry(int row, int col,  int ldeReduction) {
    if (ldeReduction == 0) return;
    switch (p.z[row][col]) {
        case 0:
            if(ldeReduction == -1) {
                // 0 -> {0,1} and k-1
                possibleValues[row][col].clear();
                possibleValues[row][col].push_back(0);
                possibleValues[row][col].push_back(1);
                entryLDEs[row][col]--;
            } else {
                // 0 -> {0,1,2,3} and k-2
                possibleValues[row][col].clear();
                possibleValues[row][col].push_back(0);
                possibleValues[row][col].push_back(1);
                possibleValues[row][col].push_back(2);
                possibleValues[row][col].push_back(3);
                entryLDEs[row][col] -= 2;
            }
            break;
        case 1:
            // This shouldn't be hit if LDE reduction != -1 and should probably throw an error
            if (ldeReduction == -1) {
                // 1 -> {2,3} and k-1
                possibleValues[row][col].clear();
                possibleValues[row][col].push_back(2);
                possibleValues[row][col].push_back(3);
                entryLDEs[row][col]--;
            }
            break;
        case 2:
            // 2 -> {2} and k
            possibleValues[row][col].clear();
            possibleValues[row][col].push_back(2);
            break;
        case 3:
            // 3 -> {3} and k
            possibleValues[row][col].clear();
            possibleValues[row][col].push_back(3);
            break;
        default:
            break;
    }
}

// If ldeValue is negative, then we are reducing all LDEs
//  Otherwise, we are only reducing the LDEs that match ldeValue
void patternMatrix::ldeReductionOnPattern(int ldeValue) {
    // This needs to be updated to take into account getting everyone to a common LDE after reduction
    //  Example: if we can only reduce by 1 down to 0, then 0 -> {0,1} and 1 -> {2,3}
    //  Gonna need to think on this becuase it should also include a full reduction
    
    int ldeDecrease = -2;  // Assume we can reduce the LDEs by 2
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(ldeValue > -1 && entryLDEs[i][j] != ldeValue) {
                continue;
            }
            // Entry values vs possible LDE reductions
            // 0 -> {0,1} and k-1
            // 0 -> {0,1,2,3} and k-2
            // 1 -> {2,3} and k
            if (p.z[i][j] == 1) {
                ldeDecrease = -1;
            } else if (p.z[i][j] >= 2) {
                ldeDecrease = 0;
                break;
            }
        }
    }
    // This should return an error
    if (ldeDecrease == 0) return;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(ldeValue > -1 && entryLDEs[i][j] != ldeValue) {
                continue;
            }
            ldeReductionOnEntry(i, j, ldeDecrease);
        }
    }
}

// Returns true if the current LDEs are a combination of 0 and -1
//  There must be a -1 though, otherwise, it cannot be fully reduced
// This is designed to be run after an LDE reduction on 1 values
bool patternMatrix::canFullyReduceLDE() {
    //FIX ME - This needs to take into account things that might be LDE 2 -> 0 and cannot be reduced anymore
    bool foundNegativeOne = false;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if (entryLDEs[i][j] == -1) {
                foundNegativeOne = true;
            } else if (entryLDEs[i][j] != 0) {
                return false;
            }
        }
    }
    return foundNegativeOne;
}

int patternMatrix::getMaxLDEValue() {
    int maxLDE = -1;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if (entryLDEs[i][j] > maxLDE) {
                maxLDE = entryLDEs[i][j];
            }
        }
    }
    return maxLDE;
}

void patternMatrix::doLDEReduction() {
    int targetLDE = getMaxLDEValue() - 2;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int ldeDecrease = -2;  // Assume we can reduce the LDEs by 2
            // Entry values vs possible LDE reductions
            // 0 -> {0,1} and k-1
            // 0 -> {0,1,2,3} and k-2
            // 1 -> {2,3} and k
            if (p.z[i][j] == 1) {
                ldeDecrease = -1;
            } else if (p.z[i][j] >= 2) {
                ldeDecrease = 0;
            }
            if (entryLDEs[i][j] + ldeDecrease > targetLDE) {
                targetLDE = entryLDEs[i][j] + ldeDecrease;
            }
        }
    }
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int reduction = targetLDE - entryLDEs[i][j];
            ldeReductionOnEntry(i, j, reduction);
        }
    }
}

// ====== MULTIPLICATION BY T-GATES ======

// TODO: Maybe move this out of the class?  It might need to stay in the class depending on how we handle the LDEs
// TODO: We need to return any LDE increases here
//    which probably means returning a struct or modifying a,b by reference and returning the LDE increase
int patternMatrix::patternElementAddition(int a, int b) {
    // TODO: Put the logic for this someplace and reference it here
    // This is not "real" addition here.  What's really happening is that we are
    //  adding the modulo 2 of the N and M values from N + M*sqrt(2)
    //  and then taking the modulo 2 of the individual results
    //  where N is bit 1 and M is bit 0:
    //    Examples:  0 == N=0, M=0
    //               1 == N=0, M=1
    //               2 == N=1, M=0
    //               3 == N=1, M=1
    // Pattern addition properties based on above:
    //  0 + 0,1,2,3 = 0,1,2,3
    //  1 + 1,2,3 = 0,3,2
    //  2 + 2,3 = 0,1
    //  3 + 3 = 0
    // Each step below whittles down the possibilities so each successive if statement handles fewer cases
    // This handles the case where a and b are the same
    // After pulling sqrt(2) out immediately...
    //  0 + 0 = 0,0 or 1,1 (same parity)
    //  1 + 1 = 0,0 or 1,1 (same parity)
    //  2 + 2 = 0,1 or 1,0 (different parity)
    //  3 + 3 = 0,1 or 1,0 (different parity)
    //  2 + 3 = 2,3 or 3,2 (different parity)
    // These you cannot pull the sqrt(2) out immediatley and need another T
    //    Try to avoid these!!!
    //  1 + 2 = 3
    //  1 + 3 = 2
    // Note from Ming
    // (“0”+-”1”)/sqrt2 is “2” and “2” or “3” and “3” ( same parity)
    if (a == b) return 0;
    // This handles all the remaining cases where a or b are 0
    else if (a == 0 || b == 0) return a + b;
    // This handles all the remaining cases where a or b are 1.  All that's left though is 1+2=3 or 1+3=2
    else if (a == 1 || b == 1) return (a + b == 4) ? 2 : 3;
    // Now, all that's left is 2+3=1
    else if (a == 2 || b == 2) return 1;
    return -1;  // This should never happen
}

void patternMatrix::loadCases() {
    // Loading from a file is another way to do this but, with only 8 of them and only being used for the patterns, this is fine
    // Case 0 is the root case and is all 0s
    cases.push_back(caseMatrix(0, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(1, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(2, "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(3, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(4, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(5, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(6, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"));
    cases.push_back(caseMatrix(7, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"));
    cases.push_back(caseMatrix(8, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"));
}

// TODO - Refactor this to output either the new or old encoding
//  it might be betteer to have a function for toStringOldEncoding and toStringNewEncoding
std::string patternMatrix::toString() {
    // TODO - this needs to be split to old and new encoding as things use it and want the implied old encoding :(
    std::ostringstream os;
    if(toStringOldEncoding) os << p;
    else os << pNewEncoding;
    return os.str();
}

struct rearrangedPattern
{
   std::vector<std::vector<int>> rearrangedMatrix;
   bool done;
};

// TODO - This might not need a return value so remove?
bool patternMatrix::rearrangeMatrix() {
    // Clear out the previous rearraangements
    caseRearrangements.clear();

    if (caseMatch == -1) {
        // Attempt to match the pattern to a case
        matchOnCases();
        // If there's still no match, then we can't rearrange the matrix
        if (caseMatch == -1) {
            return false;
        }
    }
    // Case 0 is the base case and doesn't need to be rearranged
    // TODO - we might need to rearrange case 0 at some point but not now
    if (caseMatch == 0) {
        return false;
    }
    
    //  We also need to try both the pattern and the transposed pattern
    if (cV == cases[caseMatch].c) {
        rearrangeColumns(p, cV, 0);
    }
    if (cVT == cases[caseMatch].c) {
        rearrangeColumns(pT, cVT, 0);
    }
    return true;
}

void patternMatrix::rearrangeColumns(zmatrix patternVersion, zmatrix caseVersion, int curCol) {
    if (singleCaseRearrangement && caseRearrangements.size() > 0) return;
    for (int i = curCol; i < cols; i++) {
        // If the column is all 0s, then we don't need to do anything
        if (cases[caseMatch].c.zColCounts[curCol][1] == 0) {
            curCol++;
            continue;
        }
        // swap columns
        if (cases[caseMatch].c.zColCounts[curCol][1] == caseVersion.zColCounts[i][1]) {
            zmatrix myPattern = patternVersion;
            zmatrix myCase = caseVersion;
            if (curCol != i) {
                myPattern.swapColumns(curCol, i);
                myCase.swapColumns(curCol, i);
            }
            rearrangeColumns(myPattern, myCase, curCol+1);
        }
    }
    if (curCol == cols && caseVersion.zColCounts == cases[caseMatch].c.zColCounts) {
        rearrangeRows(patternVersion, caseVersion, 0);
    }
}

void patternMatrix::rearrangeRows(zmatrix patternVersion, zmatrix caseVersion, int curRow) {
    if (singleCaseRearrangement && caseRearrangements.size() > 0) return;
    for (int i = curRow; i < rows; i++) {
        // If the row is all 0s, then we don't need to do anything
        if (cases[caseMatch].c.zRowCounts[curRow][1] == 0) {
            curRow++;
            continue;
        }
        // swap rows
        if (cases[caseMatch].c.zRowCounts[curRow][1] == caseVersion.zRowCounts[i][1]) {
            zmatrix myPattern = patternVersion;
            zmatrix myCase = caseVersion;
            if (curRow != i) {
                myPattern.swapRows(curRow, i);
                myCase.swapRows(curRow, i);
            }
            rearrangeRows(myPattern, myCase, curRow+1);
        }
    }
    if (curRow == rows && caseVersion.strictMatch(cases[caseMatch].c)) {
        std::ostringstream os;
        os << patternVersion;
        caseRearrangements[os.str()] = true;
    }
}


/* Orthonormality checks
    // Map of N / M vs letters in the paper:
    //  a = N, b = M;
    //  x = N, y = M;
    // N and M values from N + M*sqrt(2)
    //  and then taking the modulo 2 of the individual results
    //  where N is bit 1 and M is bit 0:
    //    Examples:  0 == N=0, M=0
    //               1 == N=0, M=1
    //               2 == N=1, M=0
    //               3 == N=1, M=1
    Rules for orthnormality:
    // definition of mij which is a different encoding
    mij = 2yij + xij
    // These are normality
    i. ∑ mij = 0 (mod4)
    ii. mij = 3 has to be paired if exists
    // These are orthogonality
    iii. ri · rj = 0 (mod2) when i  ̸= j
    iv. the count of (1, 2), (1, 3), and (2, 3) pairs in  ri and  rj should be even
*/

bool patternMatrix::isSymmetric() {
    return p == pT;
}

bool patternMatrix::isNormalized() {
    bool isNormal = true;
    for (int i = 0; i < p.z.size(); i++) {
        int m4Row = 0;
        int m2Row = 0;
        int m4Col = 0;
        int m2Col = 0;
        for (int j = 0; j < p.z.size(); j++) {
            if (p.z[i][j] > 3) {
                std::ostringstream os;
                os << "Invalid value in pattern matrix: " << p.z[i][j];
                throw std::runtime_error(os.str());
            
            }
            int mij = p.z[i][j] / 2 + 2 * (p.z[i][j] % 2);  //mij = Nij + 2*Mij (row)
            int mji = p.z[j][i] / 2 + 2 * (p.z[j][i] % 2);  //mji = Nji + 2*Mji (column)
            // Rule i. ∑ mij = 0 (mod4); row check
            m4Row += mij;
            // Rule i. ∑ mji = 0 (mod4); column check
            m4Col += mji;
            // Rule ii. mij = 3 has to be paired if exists (row check)
            if (mij == 3) m2Row++;
            // Rule ii. mij = 3 has to be paired if exists (col check)
            if (mji == 3) m2Col++;
        }
        if (m4Row % 4 != 0 || m2Row % 2 != 0) {
            if (printDebugInfo) {
                *debugOutput << "Row " << i+1 << " is not normalized" << std::endl;
                *debugOutput << "  Rule i. ∑ mij = " << m4Row << "; mod4: " << m4Row % 4 << std::endl;
                *debugOutput << "  Rule ii. mij = 3 has to be paired if exists; count: " << m2Row << "; mod2: " << m2Row % 2 << std::endl;
            }
            isNormal = false;
        }
        if (m4Col % 4 != 0 || m2Col % 2 != 0) {
            if (printDebugInfo) {
                *debugOutput << "Column " << i+1 << " is not normalized" << std::endl;
                *debugOutput << "  Rule i. ∑ mji = " << m4Col << "; mod4: " << m4Col % 4 << std::endl;
                *debugOutput << "  Rule ii. mij = 3 has to be paired if exists; count: " << m2Col << "; mod2: " << m2Col % 2 << std::endl;
            }
            isNormal = false;
        }
    }
    return isNormal;
}

bool patternMatrix::isOrthogonal() {
    bool isOrthogonal = true;
    // Orthogonality Checks
    for (int i = 0; i < p.z.size(); i++) {
        for (int j = i + 1; j < p.z.size(); j++) {
            int m2RowDotProd = 0;
            int m2RowPairs = 0;
            int m2ColDotProd = 0;
            int m2ColPairs = 0;
            for (int k = 0; k < p.z.size(); k++) {
                /*
                */
                int mik = p.z[i][k] / 2 + 2 * (p.z[i][k] % 2);  //mij = Nij + 2*Mij (row A)
                int mjk = p.z[j][k] / 2 + 2 * (p.z[j][k] % 2);  //mkj = Nkj + 2*Mkj (row B)
                int mki = p.z[k][i] / 2 + 2 * (p.z[k][i] % 2);  //mji = Nji + 2*Mji (column A)
                int mkj = p.z[k][j] / 2 + 2 * (p.z[k][j] % 2);  //mjk = Njk + 2*Mjk (column B)
                // iii. ri · rj = 0 (mod2) when i  ̸= k
                // row A * row B
                m2RowDotProd += mik * mjk;
                // col A * col B
                m2ColDotProd += mki * mkj;
                // iv. the count of (1, 2), (1, 3), and (2, 3) pairs in  ri and rj should be even
                if (mik != 0 && mjk != 0 && mik != mjk) m2RowPairs++;
                if (mki != 0 && mkj != 0 && mki != mkj) m2ColPairs++;
            }
            if (m2RowDotProd % 2 != 0 || m2RowPairs % 2 != 0 ) {
                if (printDebugInfo) {
                    *debugOutput << "Rows " << i+1 << " and " << j+1 << " are not orthogonal" << std::endl;
                    *debugOutput << "  Dot Product = " << m2RowDotProd << "; mod2 = " << m2RowDotProd % 2 << std::endl;
                    *debugOutput << "  (1, 2), (1, 3), and (2, 3) pairs should be even; count = " << m2RowPairs << "; mod2 = " << m2RowPairs % 2 << std::endl;
                }
                isOrthogonal = false;
            }
            if (m2ColDotProd % 2 != 0 || m2ColPairs % 2 != 0) {
                if (printDebugInfo) {
                    *debugOutput << "Columns " << i+1 << " and " << j+1 << " are not orthogonal" << std::endl;
                    *debugOutput << "  Dot Product = " << m2ColDotProd << "; mod2 = " << m2ColDotProd % 2 << std::endl;
                    *debugOutput << "  (1, 2), (1, 3), and (2, 3) pairs should be even; count = " << m2ColPairs << "; mod2 = " << m2ColPairs % 2 << std::endl;
                }
                isOrthogonal = false;
            }
        }
    }
    return isOrthogonal;
}

bool patternMatrix::isRowNormalized(int row, zmatrix z) {
    // This should probably just call isRowNormalized(std::vector<int> row) and pass in z.z[row]
    // but this might not be used once we get the new sets optimization in place for possible values
    bool isNormal = true;
    int m4Row = 0;
    int m2Row = 0;
    for (int j = 0; j < z.z[row].size(); j++) {
        if (z.z[row][j] > 3) {
            std::ostringstream os;
            os << "Invalid value in pattern matrix: " << p.z[row][j];
            throw std::runtime_error(os.str());
        
        }
        int mij = z.z[row][j] / 2 + 2 * (z.z[row][j] % 2);  //mij = Nij + 2*Mij (row)
        // Rule i. ∑ mij = 0 (mod4); row check
        m4Row += mij;
        // Rule ii. mij = 3 has to be paired if exists (row check)
        if (mij == 3) m2Row++;
    }
    if (m4Row % 4 != 0 || m2Row % 2 != 0) {
        if (printDebugInfo) {
            *debugOutput << "Row " << row + 1 << " is not normalized: [";
            for (int j = 0; j < z.z[row].size(); j++) {
                int v = z.z[row][j];
                if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                *debugOutput << v;
                if (j < z.z[row].size() - 1) *debugOutput << ",";
            }
            *debugOutput << "]" << std::endl;
            //std::cout << "  Rule i. ∑ mij = " << m4Row << "; mod4: " << m4Row % 4 << std::endl;
            //std::cout << "  Rule ii. mij = 3 has to be paired if exists; count: " << m2Row << "; mod2: " << m2Row % 2 << std::endl;
        }
        isNormal = false;
    }
    return isNormal;
}

bool patternMatrix::isRowNormalized(std::vector<int> row) {
    bool isNormal = true;
    int m4Row = 0;
    int m2Row = 0;
    for (int j = 0; j < row.size(); j++) {
        if (row[j] > 3) {
            std::ostringstream os;
            os << "Invalid value in isRowNormalized check: " << row[j];
            throw std::runtime_error(os.str());
        
        }
        int mij = row[j] / 2 + 2 * (row[j] % 2);  //mij = Nij + 2*Mij (row)
        // Rule i. ∑ mij = 0 (mod4); row check
        m4Row += mij;
        // Rule ii. mij = 3 has to be paired if exists (row check)
        if (mij == 3) m2Row++;
    }
    if (m4Row % 4 != 0 || m2Row % 2 != 0) {
        if (printDebugInfo) {
            *debugOutput << "Row is not normalized: [";
            for (int j = 0; j < row.size(); j++) {
                int v = row[j];
                if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                *debugOutput << v;
                if (j < row.size() - 1) *debugOutput << ",";
            }
            *debugOutput << "]" << std::endl;
            //std::cout << "  Rule i. ∑ mij = " << m4Row << "; mod4: " << m4Row % 4 << std::endl;
            //std::cout << "  Rule ii. mij = 3 has to be paired if exists; count: " << m2Row << "; mod2: " << m2Row % 2 << std::endl;
        }
        isNormal = false;
    }
    return isNormal;
}

bool patternMatrix::areRowsOrthogonal(int row1, int row2, zmatrix z) {
    bool isOrthogonal = true;
    int m2RowDotProd = 0;
    int m2RowPairs = 0;
    for (int k = 0; k < z.z[row1].size(); k++) {
        /*
        */
        int mik = z.z[row1][k] / 2 + 2 * (z.z[row1][k] % 2);  //mij = Nij + 2*Mij (row A)
        int mjk = z.z[row2][k] / 2 + 2 * (z.z[row2][k] % 2);  //mkj = Nkj + 2*Mkj (row B)
        // iii. ri · rj = 0 (mod2) when i  ̸= k
        // row A * row B
        m2RowDotProd += mik * mjk;
        // iv. the count of (1, 2), (1, 3), and (2, 3) pairs in  ri and rj should be even
        if (mik != 0 && mjk != 0 && mik != mjk) m2RowPairs++;
    }
    if (m2RowDotProd % 2 != 0 || m2RowPairs % 2 != 0 ) {
        if (printDebugInfo) {
            int highRow = row1;
            if (row2 > row1) highRow = row2;
            *debugOutput << "Rows " << row1+1 << " and " << row2+1 << " are not orthogonal:  "; 
            for (int k = 0; k <= highRow; k++) {
                *debugOutput << "Row " << k+1 << ": [";
                for (int n = 0; n < z.z[k].size(); n++) {
                    int v = z.z[k][n];
                    if (!printOldEncoding) v = (v == 1) ? 2 : (v == 2) ? 1 : v;
                    *debugOutput << v;
                    //std::cout << z.z[k][n];
                    if (n < z.z[k].size() - 1) *debugOutput << ",";
                }
                *debugOutput << "]  ";
            }
            *debugOutput << std::endl;
            //std::cout << "  Dot Product = " << m2RowDotProd << "; mod2 = " << m2RowDotProd % 2 << std::endl;
            //std::cout << "  (1, 2), (1, 3), and (2, 3) pairs should be even; count = " << m2RowPairs << "; mod2 = " << m2RowPairs % 2 << std::endl;
        }
        isOrthogonal = false;
    }
    return isOrthogonal;
}

bool patternMatrix::areRowsOrthogonal(std::vector<int> row1, std::vector<int> row2) {
    bool isOrthogonal = true;
    int m2RowDotProd = 0;
    int m2RowPairs = 0;
    for (int k = 0; k < row1.size(); k++) {
        /*
        */
        int mik = row1[k] / 2 + 2 * (row1[k] % 2);  //mij = Nij + 2*Mij (row A)
        int mjk = row2[k] / 2 + 2 * (row2[k] % 2);  //mkj = Nkj + 2*Mkj (row B)
        // iii. ri · rj = 0 (mod2) when i  ̸= k
        // row A * row B
        m2RowDotProd += mik * mjk;
        // iv. the count of (1, 2), (1, 3), and (2, 3) pairs in  ri and rj should be even
        if (mik != 0 && mjk != 0 && mik != mjk) m2RowPairs++;
    }
    if (m2RowDotProd % 2 != 0 || m2RowPairs % 2 != 0 ) {
        isOrthogonal = false;
    }
    return isOrthogonal;
}