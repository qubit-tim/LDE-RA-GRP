#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "pattern-matrix.hpp"
#include "case-matrix.hpp"

struct rearrangedPattern
{
   std::vector<std::vector<int>> rearrangedMatrix;
   bool done;
};

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening pattern file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int patternNumber = 1;
    while (std::getline(file, line)) {
        int id = std::stoi(line.substr(0, line.find(" ")));
        patterns.push_back(patternMatrix(id, line.substr(line.find(" ")+1, line.size()-1)));
    }
    file.close();
    return patterns;
}

void printMatrix(std::ostream& stream, std::vector<std::vector<int>> m, bool spaced = false) {
    for (int i = 0; i < m.size(); i++) {
        stream << "["; // Print opening bracket for the row
        for (int j = 0; j < m[i].size(); j++) {
            stream << m[i][j];
            if (spaced) stream << " ";
            // Don't print a comma after the last element
            if (j != m[i].size()-1) {
                stream << ","; 
                if (spaced) stream << " ";
            }
        }
        stream << "]"; // Print closing bracket for the row
        if (spaced) stream << std::endl;
    }
    if (!spaced) stream << std::endl;  // if not spaced, then we need to print an extra newline
}

rearrangedPattern setupColumns(std::vector<std::vector<int>> pmCols, int curColumn) {
    std::vector<int> vTemp;
    for (int i = curColumn; i < pmCols.size(); i++) {
        switch (curColumn) {
            case 0: case 1: case 2: case 3: {
                int count = 0;
                for (int j = 0; j < pmCols[i].size(); j++) {
                    if (pmCols[i][j] == 2 || pmCols[i][j] == 3) count++;
                }
                if (count != 4) break;
                vTemp = pmCols[curColumn];
                pmCols[curColumn] = pmCols[i];
                pmCols[i] = vTemp;
                return setupColumns(pmCols, curColumn + 1);
                break;
            }
            // This column must be 0s or 1s, which, by default, it should be
            case 4:
                // Return the first combo
                return setupColumns(pmCols, curColumn + 1);
                break;
            // This column must be 0s or 1s, which, by default, it should be
            case 5:
                // Put this back into row view
                std::vector<std::vector<int>> pmRows = pmCols;  // set to columns to get the correct size
                for (int i = 0; i < pmCols.size(); i++) {
                    for (int j = 0; j < pmCols[i].size(); j++) {
                        pmRows[j][i] = pmCols[i][j];
                    }
                }
                return rearrangedPattern{pmRows, true};
                break;
        }
    }
    // Put this back into row view
    std::vector<std::vector<int>> pmRows = pmCols;  // set to columns to get the correct size
    for (int i = 0; i < pmCols.size(); i++) {
        for (int j = 0; j < pmCols[i].size(); j++) {
            pmRows[j][i] = pmCols[i][j];
        }
    }
    return rearrangedPattern{pmRows, false};
}

rearrangedPattern setupRows(std::vector<std::vector<int>> pmRows, int curRow) {
    // At this point, the first row should be set and ALL of the columns set, so all we need to do is move the rows around
    std::vector<int> vTemp;
    bool rowsSet = false;
    for (int i = curRow; i < pmRows.size(); i++) {
        switch (curRow) {
            // Row 0-3 must have the 4 elements as 2s or 3s
            case 0: case 1: case 2: case 3: {
                int count = 0;
                for (int j = 0; j < pmRows[i].size(); j++) {
                    if (pmRows[i][j] == 2 || pmRows[i][j] == 3) count++;
                }
                if (count != 4) break;
                vTemp = pmRows[curRow];
                pmRows[curRow] = pmRows[i];
                pmRows[i] = vTemp;
                return setupRows(pmRows, curRow + 1);
                break;
            }
            case 4: {
                return setupRows(pmRows, curRow + 1);
                break;
            }
            case 5: {
                rowsSet = true;
                break;
            }
        }
    }
    if (!rowsSet) return rearrangedPattern{pmRows, false};
    // Setup the columns view before trying to set the columns
    std::vector<std::vector<int>> pmCols = pmRows;  // set to columns to get the correct size
    for (int i = 0; i < pmRows.size(); i++) {
        for (int j = 0; j < pmRows[i].size(); j++) {
            pmCols[j][i] = pmRows[i][j];
        }
    }
    return setupColumns(pmCols, 0);
}

// This, along with all of the other case rearrange functions, should be moved into the pattern matrix class
//  and then have them directly modify the pattern matrix along with appropriate metadata
rearrangedPattern case3rearange(patternMatrix pm) {
    rearrangedPattern rp;
    rp.done = false;
    for (int i = 0; i < pm.p.z.size(); i++) {
        std::vector<std::vector<int>> pmRows = pm.p.z;
        rp = setupRows(pmRows, 0);
    }
    if (rp.done) return rp;
    for (int i = 0; i < pm.pT.z.size(); i++) {
        // But since we are doing the transpose, we need to use the original pattern matrix to start
        std::vector<std::vector<int>> pmRows = pm.pT.z;
        rp = setupRows(pmRows, 0);
    }
    return rp;
}

int case3SubCase(std::vector<std::vector<int>> z) {
    int rightSum = z[0][4] + z[0][5] + z[1][4] + z[1][5] + z[2][4] + z[2][5] + z[3][4] + z[3][5];
    int sqSum = z[4][4] + z[4][5] + z[5][4] + z[5][5];
    int bottomSum = z[4][0] + z[4][1] + z[4][2] + z[4][3] + z[5][0] + z[5][1] + z[5][2] + z[5][3];
    if (sqSum == 4 && (rightSum == 0 || bottomSum == 0)) return 1;
    else if (sqSum == 0 && (rightSum == 8 || bottomSum == 8)) return 2;
    return -1; 
}

int main(int argc, char **argv) {
    // Taking case 3 back to the origin
    // Rearrange case 3 files and look for the 2 patterns on top left of page 12
    //  Then do the multiplication to see where we land

    std::string patternFile = "case3-matches-patterns928.txt";
    std::string case3File = "case3/case3-928-rearranged.txt";
    std::string case3FileDetails = "case3/case3-928-rearranged-details.txt";
    std::vector<patternMatrix> patterns = loadPatterns("matched-cases/" + patternFile);
    std::ofstream case3Out(case3File);
    std::ofstream case3OutDets(case3FileDetails);
    if (!case3Out.is_open()) {
        std::cerr << "Error opening case 3 output file!" << std::endl;
        return {}; // Indicate error
    }
    if (!case3OutDets.is_open()) {
        std::cerr << "Error opening case 3 details output file!" << std::endl;
        return {}; // Indicate error
    }
    rearrangedPattern rp;
    for (patternMatrix pm : patterns) {
        rp = case3rearange(pm);
        if (rp.done) {
            int subCase = case3SubCase(rp.rearrangedMatrix);
            if (subCase == -1) {
                case3OutDets << "Pattern " << pm.id << ":" << pm << " is case 3 but not a subcase we are looking for " << subCase << std::endl;
                // Don't print the matrix for this one
                continue;
            }
            case3OutDets << "Pattern " << pm.id << ":" << pm << " is case 3-" << subCase << std::endl;
            case3OutDets << "Rearranged pattern to match case 3 layout: " << std::endl;
            printMatrix(case3OutDets, rp.rearrangedMatrix, true);
            // We are only concerned with the first 4 rows of case 3
            for (int i = 0; i < 4; i++) {
                for (int j = i+1; j < 4; j++) {
                    if (rp.rearrangedMatrix[i] == rp.rearrangedMatrix[j]) {
                        case3OutDets << "Rows " << i+1 << " and " << j+1 << " are duplicates." << std::endl;
                    }
                }
            }
            std::vector<std::vector<int>> pmCols = rp.rearrangedMatrix;  // set to columns to get the correct size
            for (int i = 0; i < rp.rearrangedMatrix.size(); i++) {
                for (int j = 0; j < rp.rearrangedMatrix[i].size(); j++) {
                    pmCols[j][i] = rp.rearrangedMatrix[i][j];
                }
            }
            // We are only concerned with the first 4 columns of case 3
            for (int i = 0; i < 4; i++) {
                for (int j = i+1; j < 4; j++) {
                    if (pmCols[i] == pmCols[j]) {
                        case3OutDets << "Columns " << i+1 << " and " << j+1 << " are duplicates." << std::endl;
                    }
                }
            }
            case3Out << pm.id << " ";
            printMatrix(case3Out, rp.rearrangedMatrix, false);
        }
    }
    case3Out.close();
    case3OutDets.close();
    // This is a hack until the rearranging is moved into the pattern matrix class
    std::string multFile = "case3/case3-928-multiplication.txt";
    std::ofstream multOut(multFile);
    if (!multOut.is_open()) {
        std::cerr << "Error opening multiplication output file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> ptns = loadPatterns("case3/case3-928-rearranged.txt");
    // The logic for this should also be moved into the pattern matrix class
    for (patternMatrix pm : ptns) {
        // From looking at the case 3 rearranged details file, we can do column multiplication to remove the 2s and 3s
        //  and then see what we get
        pm.rightTGateMultiply(0,2);
        pm.rightTGateMultiply(1,3);
        multOut << "Pattern " << pm.id << ":" << pm << " - post multiplication: " << std::endl;
        printMatrix(multOut, pm.p.z, true);
    }
    multOut.close();
    return 0;
}
