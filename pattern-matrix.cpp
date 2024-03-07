#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

patternMatrix::patternMatrix(int pNum, std::string matrix) {
    caseMatch = -1;
    subCaseMatch = -1;
    id = pNum;
    p = zmatrix(rows, cols, 3);
    pT = zmatrix(cols, rows, 3);
    swap23 = zmatrix(rows, cols, 3);
    swap23T = zmatrix(cols, rows, 3);
    cV = zmatrix(rows, cols, 1);
    cV.strictMatch = false;  // case matrices do not need to match element to element since they can be rearranged to match
    cVT = zmatrix(cols, rows, 1);
    cVT.strictMatch = false;  // case matrices do not need to match element to element since they can be rearranged to match
    loadFromString(matrix);
}

void patternMatrix::loadFromString(std::string m) {
    std::stringstream ms(m);
    std::string r;
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

            p.z[row][col] = mv;
            pT.z[col][row] = mv;
            swap23.z[row][col] = (mv == 2) ? 3 : (mv == 3) ? 2 : mv;
            swap23T.z[col][row] = (mv == 2) ? 3 : (mv == 3) ? 2 : mv;
            cV.z[row][col] = mv / 2;
            cVT.z[col][row] = mv / 2;
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
}

int patternMatrix::matchedCase(std::vector<caseMatrix> cases) {
    caseMatch = -1;
    for (caseMatrix cm : cases) {
        if (matchesCase(cm)) {
            if (caseMatch != -1) {
                std::ostringstream caseErr;
                std::cerr << "Pattern " << id << " matches more than 1 case: " << caseMatch << " and " << cm.id << std::endl;
                throw std::runtime_error("Pattern matches more than 1 case: Exiting");
            }
            caseMatch = cm.id;
        }
    }
    return caseMatch;
}

bool patternMatrix::matchesCase(caseMatrix cm) {
    if (cV == cm.c) return true; // pattern matrix case style matches the case matrix
    if (cVT == cm.c) return true; // transposed pattern matrix case style matches the case matrix
    return false;
}

bool patternMatrix::isTranspose(patternMatrix other) {
    // We need to do a complete check to verify that the pattern matrix is a transpose of the other
    bool isTranspose = (pT == other.p);
    return isTranspose;
}

bool patternMatrix::is23Swap(patternMatrix other) {
   // Now to check details starting with rows.
   std::vector<std::vector<int>> zRowCounts = swap23.zRowCounts;
    for (int i = 0; i < other.p.zRowCounts.size(); i++) {
        for (int j = 0; j < zRowCounts.size(); j++) {
            if (zRowCounts[j] == other.p.zRowCounts[i]) {
                zRowCounts.erase(zRowCounts.begin() + j);
                break;
            }
        }
    }
    if (zRowCounts.size() != 0) return false;
    // Now to check details of columns.
    std::vector<std::vector<int>> zColCounts = swap23.zColCounts;
    for (int i = 0; i < other.p.zColCounts.size(); i++) {
        for (int j = 0; j < zColCounts.size(); j++) {
            if ( zColCounts[j] == other.p.zColCounts[i]) {
                zColCounts.erase(zColCounts.begin() + j);
                break;
            }
        }
    }
    if (zColCounts.size() != 0) return false;
    // If we made it this far, then we have a match (I think)
    return true;
}


// This is essentially the same as is23Swap, but we are checking the transposed swap23 matrix
// TODO - Refactor this to use a common function with is23Swap
bool patternMatrix::is23SwapT(patternMatrix other) {
   // Now to check details starting with rows.
   std::vector<std::vector<int>> zRowCounts = swap23T.zRowCounts;
    for (int i = 0; i < other.p.zRowCounts.size(); i++) {
        for (int j = 0; j < zRowCounts.size(); j++) {
            if ( zRowCounts[j] == other.p.zRowCounts[i]) {
                zRowCounts.erase(zRowCounts.begin() + j);
                break;
            }
        }
    }
    if (zRowCounts.size() != 0) return false;
    // Now to check details of columns.
    std::vector<std::vector<int>> zColCounts = swap23T.zColCounts;
    for (int i = 0; i < other.p.zColCounts.size(); i++) {
        for (int j = 0; j < zColCounts.size(); j++) {
            if ( zColCounts[j] == other.p.zColCounts[i]) {
                zColCounts.erase(zColCounts.begin() + j);
                break;
            }
        }
    }
    if (zColCounts.size() != 0) return false;
    // If we made it this far, then we have a match (I think)
    return true;
}

void patternMatrix::printDebug() {
    std::cout << "Pattern Number: " << id << std::endl;
    std::cout << "Matched Case Number: " << caseMatch << std::endl;
    std::cout << "Pattern Matrix Debug:" << std::endl;
    p.printDebug();
    std::cout << "Transposed Pattern Matrix Debug:" << std::endl;
    pT.printDebug();
    std::cout << "Swapped 2s and 3s Pattern Matrix Debug:" << std::endl;
    swap23.printDebug();
    std::cout << "Case Style Pattern Matrix Debug:" << std::endl;
    cV.printDebug();
    std::cout << "Transposed Case Style Pattern Matrix Debug:" << std::endl;
    cVT.printDebug();
}

std::ostream& operator<<(std::ostream& os,const patternMatrix &pm) {
    // Could do this differently by using flags to set the output format
    if (pm.printID) {
        os << pm.id << " ";
    }
    if (pm.printCaseMatch) {
        os << pm.caseMatch << " ";
    }
    if (pm.printAllIDs) {
        os << pm.id2704 << " " << pm.id928 << " " << pm.id785 << " ";
    }
    os << pm.p;
    return os;
}

void patternMatrix::printMatchComparison(caseMatrix cm) {
    std::cout << "NOT IMPLEMENTED YET" << std::endl;
}

void patternMatrix::rearrangeMatrix(caseMatrix cm) {
    std::cout << "NOT IMPLEMENTED YET" << std::endl;
}

void patternMatrix::leftTGateMultiply(int pRow, int qRow) {
    //  Handle left T-gate multiplication
    //  This means adding rows p, q and replacing both with the result
    //  This might be different when p > q
    // TODO: We might need to track LDEs here
    int result;
    for (int i = 0; i < p.z[pRow].size(); i++) {
        result = patternElementAddition(p.z[pRow][i], p.z[qRow][i]);
        p.z[pRow][i] = result;
        p.z[qRow][i] = result;
    }
}

void patternMatrix::rightTGateMultiply(int pCol, int qCol) {
    //  Handle right T-gate multiplication
    //  This means adding column p, q and replacing both with the result
    //  This might be different when p > q
    // TODO: We might need to track LDEs here
    int result;
    for (int i = 0; i < p.z.size(); i++) {
        result = patternElementAddition(p.z[i][pCol], p.z[i][qCol]);
        p.z[i][pCol] = result;
        p.z[i][qCol] = result;
    }
}

// TODO: We might need to return any LDE increases here
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
    if (a == b) return 0;
    // This handles all the remaining cases where a or b are 0
    else if (a == 0 || b == 0) return a + b;
    // This handles all the remaining cases where a or b are 1.  All that's left though is 1+2=3 or 1+3=2
    else if (a == 1 || b == 1) return (a + b == 4) ? 2 : 3;
    // Now, all that's left is 2+3=1
    else if (a == 2 || b == 2) return 1;
    return -1;  // This should never happen
}
