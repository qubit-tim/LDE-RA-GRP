#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

patternMatrix::patternMatrix(int pNum, std::string matrix) {
    caseMatch = -1;
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
    // We need to do a complete check to verify that the pattern matrix is a 2/3 swap of the other
    /* Ok, this check is a bit more complicated than I thought
    I think you need to go through each:
        std::vector<std::vector<int>> zRowCounts;
        std::vector<std::vector<int>> zColCounts;
    and make sure that each have a direct 1:1 match with the other pattern matrix
    */
   // This is a quick check to see if there's a potential match. 
   //  if (swap23 != other.p) return false;
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

// I think this is just the same check as the is23Swap function just transposed
bool patternMatrix::is23SwapT(patternMatrix other) {
    // We need to do a complete check to verify that the pattern matrix is a 2/3 swap of the other
    /* Ok, this check is a bit more complicated than I thought
    I think you need to go through each:
        std::vector<std::vector<int>> zRowCounts;
        std::vector<std::vector<int>> zColCounts;
    and make sure that each have a direct 1:1 match with the other pattern matrix
    */
   // This is a quick check to see if there's a potential match. 
   // if (swap23T != other.p) return false;
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

/* All old stuff that needs to be rewritten
int patternMatrix::matchCases(std::vector<caseMatrix> cases) {
    for (caseMatrix cm : cases) {
        bool caseMatch = true;
        bool caseTransposeMatch = true;
        for (int i = 0; i < 7; i++) {
            // Check to see if the case still matches
            if (caseMatch && cm.rowSumCounts[i] != this->rowSumCounts[i] || cm.colSumCounts[i] != this->colSumCounts[i]) {
                //std::cout << "case " << cm.caseNumber << "doesn't match" << std::endl;
                caseMatch = false;
                }
            // Check to see if the transpose of the case still matches
            if (caseTransposeMatch && cm.rowSumCounts[i] != this->colSumCounts[i] || cm.colSumCounts[i] != this->rowSumCounts[i]) {
                //std::cout << "transposed case " << cm.caseNumber << "doesn't match" << std::endl;
                caseTransposeMatch = false;
            }
            if (!caseMatch && !caseTransposeMatch) break; // No need to continue if we already know it's not a match
        }
        if(caseMatch) {
            std::cout << "Match for Pattern " << this->patternNumber << " and Case " << cm.caseNumber << std::endl;
            printMatchComparison(cm);
            // This would rearrange the matrix so it would match the case matrix then print for verification
            //std::cout << "Check the rearranged match for " << cm.caseNumber << std::endl;
            //rearrangeMatrix(cm);
            //printMatchComparison(cm);
            if (this->caseMatchNum != -1) {
                std::cerr << "Pattern " << patternNumber << " matches more than 1 case: " << this->caseMatchNum << " and " << cm.caseNumber << std::endl;
                throw std::runtime_error("Pattern matches more than 1 case: Exiting");
            }
            this->caseMatchNum = cm.caseNumber;
        
        }
        else if (caseTransposeMatch) {
            std::cout << "Match for Pattern " << this->patternNumber << " and the transpose of Case " << cm.caseNumber << std::endl;
            printMatchComparison(cm);
            // This would rearrange the matrix so it would match the case matrix then print for verification
            //std::cout << "Check the rearranged match for " << cm.caseNumber << std::endl;
            //rearrangeMatrix(cm);
            //printMatchComparison(cm);
            if (this->caseMatchNum != -1) std::cerr << "Pattern " << patternNumber << " matches more than 1 case!" << std::endl;
            this->caseMatchNum = cm.caseNumber;
        }
    }
    if(this->caseMatchNum == -1) std::cout << "Pattern " << patternNumber << " has no matches." << std::endl;
    return this->caseMatchNum;
}

// Print out debug information for verifying the pattern matrix and metadata
void patternMatrix::debugPrint() {
    std::cout << "======= BEGIN DEBUG PRINT FOR PATTERN " << this->patternNumber << " =======" << std::endl;
    if (this->patternNumber != -1) {
        std::cout << "Pattern " << this->patternNumber << std::endl;
    }
    if (this->caseMatchNum != -1) {
        std::cout << "Matches Case Number " << this->patternNumber << std::endl;
    }
    for (int i = 0; i < 6; i++) {
        std::cout << "["; // Print opening bracket for the row
        for (int j = 0; j < 6; j++) {
            std::cout << this->matrix[i][j]; // Print element
            if (j != 5) std::cout << ", "; // Don't print a comma after the last element
        }
        std::cout << "]" << std::endl; // Print closing bracket for the row
    }
    std::cout << "Printing meta data that's used for case matching " << std::endl;
    // Print row / column counts
    std::cout << "Row Sums: [";
    for (int i = 0; i < 6; i++) {
        std::cout << this->rowSums[i];
        if (i != 5) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the rowCounts

    std::cout << "Column Sums: [";
    for (int i = 0; i < 6; i++) {
        std::cout << this->colSums[i];
        if (i != 5) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the colSums
    // Print row / column count maps
    std::cout << "Row Map Sums: [";
    for (int i = 0; i < 7; i++) {
        std::cout << this->rowSumCounts[i];
        if (i != 6) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the rowCounts

    std::cout << "Column Map Sums: [";
    for (int i = 0; i < 7; i++) {
        std::cout << this->colSumCounts[i];
        if (i != 6) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the colSums

    std::cout << "Printing meta data that's used for pattern matching " << std::endl;
    std::cout << "Matrix Sum: " << this->matrixSum << std::endl;
    std::cout << "Number Counts: [";
    for (int i = 0; i < 4; i++) {
        std::cout << this->numCounts[i];
        if (i != 3) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the numCounts
    // int rowCounts[6][4]; // Count of the number of 0s, 1s, 2s, 3s in each row
    std::cout << "Row Counts: [";
    for (int i = 0; i < 6; i++) {
        std::cout << "[";
        for (int j = 0; j < 4; j++) {
            std::cout << this->rowCounts[i][j];
            if (j != 3) std::cout << ", ";
        }
        std::cout << "]";
        if (i != 5) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the rowCounts
    // int colCounts[6][4]; // Count of the number of 0s, 1s, 2s, 3s in each column
    std::cout << "Column Counts: [";
    for (int i = 0; i < 6; i++) {
        std::cout << "[";
        for (int j = 0; j < 4; j++) {
            std::cout << this->colCounts[i][j];
            if (j != 3) std::cout << ", ";
        }
        std::cout << "]";
        if (i != 5) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the colCounts
    // int rowCountCounts[4][7];
    std::cout << "Row Count Counts: [";
    for (int i = 0; i < 4; i++) {
        std::cout << "[";
        for (int j = 0; j < 7; j++) {
            std::cout << this->rowCountCounts[i][j];
            if (j != 6) std::cout << ", ";
        }
        std::cout << "]";
        if (i != 3) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the rowCountCounts
    // int colCountCounts[4][7];
    std::cout << "Column Count Counts: [";
    for (int i = 0; i < 4; i++) {
        std::cout << "[";
        for (int j = 0; j < 7; j++) {
            std::cout << this->colCountCounts[i][j];
            if (j != 6) std::cout << ", ";
        }
        std::cout << "]";
        if (i != 3) std::cout << ", ";
    }
    std::cout << "]" << std::endl; // Print closing bracket for the colCountCounts
    std::cout << "======= END DEBUG PRINT FOR PATTERN " << this->patternNumber << " =======" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const patternMatrix &pm) {
    os << pm.patternNumber << " ";
    for (int i = 0; i < 6; i++) {
        os << "["; // Print opening bracket for the row
        for (int j = 0; j < 6; j++) {
            os << pm.matrix[i][j]; // Print element
            if (j != 5) os << ","; // Don't print a comma after the last element
        }
        os << "]"; // Print closing bracket for the row
    }
    os << std::endl;
    return os;
}


void patternMatrix::printMatchComparison(caseMatrix cm) {
    std::cout << "Pattern: " << patternNumber << "\t\t" << "Case: " << cm.caseNumber << std::endl;
    // TODO: Turn this into a string representation of the rows and full matrix
    for (int i = 0; i < 6; i++) {
        // Print the pattern matrix
        std::cout << "["; // Print opening bracket for the row
        for (int j = 0; j < 6; j++) {
            // Let's translate to 0s and 1s for easier comparison to the case matrix
            std::cout << this->matrix[i][j] / 2; // Print element
            // std::cout << this->matrix[i][j]; // Print element
            if (j != 5) std::cout << ", "; // Don't print a comma after the last element
        }
        std::cout << "]\t"; // Print closing bracket for the row
        // Print the case matrix
        std::cout << "["; // Print opening bracket for the row
        for (int j = 0; j < 6; j++) {
            std::cout << cm.matrix[i][j]; // Print element
            if (j != 5) std::cout << ", "; // Don't print a comma after the last element
        }
        std::cout << "]"; // Print closing bracket for the row
        std::cout << std::endl; // Move to the next line after each row
    }
}

// This doesn't quite work yet.  
void patternMatrix::rearrangeMatrix(caseMatrix cm) {
    int temp;
    // Attempt to arrange the pattern matrix to match the case matrix
    // Set the first row to match the case matrix
    if (cm.rowSums[0] != this->rowSums[0]) {
        // Find a row that contains the correct number of counts
        for (int i = 1; i < 6; i++) {
            if (cm.rowSums[0] == this->rowSums[i]) {
                // Swap the rows
                for (int j = 0; j < 6; j++) {
                    temp = this->matrix[0][j];
                    this->matrix[0][j] = this->matrix[i][j];
                    this->matrix[i][j] = temp;
                }
                // Swap the row counts
                temp = this->rowSums[0];
                this->rowSums[0] = this->rowSums[i];
                this->rowSums[i] = temp;
                break;
            }
        }
    }
    // Now let's set the columns of the first row to match the case matrix
    for (int j = 0; j < 6; j++) {
        // If the column values don't match or column counts don't match, then we have to swap columns
        if (cm.matrix[0][j] != this->matrix[0][j] / 2 || cm.colSums[j] != this->colSums[j]) {
            // Find a column that contains the correct number of counts and correct value
            for (int k = j+1; k < 6; k++) {
                if (cm.colSums[j] == this->colSums[k] && cm.matrix[0][j] == this->matrix[0][k] / 2) {
                    // Swap the columns
                    for (int l = 0; l < 6; l++) {
                        temp = this->matrix[l][j];
                        this->matrix[l][j] = this->matrix[l][k];
                        this->matrix[l][k] = temp;
                    }
                    // Swap the column counts
                    temp = this->colSums[j];
                    this->colSums[j] = this->colSums[k];
                    this->colSums[k] = temp;
                    break;
                }
            }
        }
    }
    // Now let's rearrage the rows to match the case matrix
    for (int i = 1; i < 6; i++) {
        // Test to see if the row is an exact match
        bool exactMatch = true;
        for (int j = 0; j < 6; j++) {
            if (cm.matrix[i][j] != this->matrix[i][j] / 2) {
                exactMatch = false;
                //Attempt to swap columns
                for (int k = j+1; k < 6; k++) {
                    // skip columns that have the wrong count
                    if (cm.colSums[j] != this->colSums[k]) {
                        std::cout << "Column counts don't match" << std::endl;
                        continue;
                    }
                    if (cm.matrix[i][j] == this->matrix[i][k] / 2) {
                        // Verify the prior rows have the same values between columns j and k
                        bool priorRowsMatch = true;
                        for (int l = 0; l < i; l++) {
                            if (this->matrix[l][j] / 2 != this->matrix[l][k] / 2) {
                                priorRowsMatch = false;
                                break;
                            }
                        }
                        if(priorRowsMatch) {
                            // Swap the columns
                            for (int l = 0; l < 6; l++) {
                                temp = this->matrix[l][j];
                                this->matrix[l][j] = this->matrix[l][k];
                                this->matrix[l][k] = temp;
                            }
                            // Swap the column counts
                            temp = this->colSums[j];
                            this->colSums[j] = this->colSums[k];
                            this->colSums[k] = temp;
                            exactMatch = true;
                            break;
                        }
                    }
                }
                // Not a match so go to the next row
                if (!exactMatch) break;
            }
        }
        // If the row isn't an exact match, then we have to swap to one that is
        if (!exactMatch) {
            // Find a row that contains the correct number of counts
            for (int j = i+1; j < 6; j++) {
                // skip rows that have the wrong count
                if (cm.rowSums[i] != this->rowSums[j]) {
                    continue;
                }
                // Test to see if the row is an exact match
                exactMatch = true;
                for (int k = 0; k < 6; k++) {
                    if (cm.matrix[i][k] != this->matrix[j][k] / 2) {
                        exactMatch = false;
                        break;
                    }
                }
                // Not a match so go to the next row
                if (!exactMatch) {
                    continue;
                }
                // We have a match so swap the rows
                for (int k = 0; k < 6; k++) {
                    temp = this->matrix[i][k];
                    this->matrix[i][k] = this->matrix[j][k];
                    this->matrix[j][k] = temp;
                }
                // Swap the row counts
                temp = this->rowSums[i];
                this->rowSums[i] = this->rowSums[j];
                this->rowSums[j] = temp;
                break;
            }
            // Should probably return an error here if we don't find a match
            if (!exactMatch) {
                std::cout << "No match for row " << i << std::endl;
            }
        }
    }
}
*/