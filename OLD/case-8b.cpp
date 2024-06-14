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
    if (!spaced) stream << "[";  // if not spaced, then we need to print the opening bracket
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
    if (!spaced) stream << "]" << std::endl;  // if not spaced, then we need to print the closing bracket
}

rearrangedPattern setupRows(std::vector<std::vector<int>> pmRows, int curRow) {
    // At this point, the first row should be set and ALL of the columns set, so all we need to do is move the rows around
    std::vector<int> vTemp;
    for (int i = curRow; i < pmRows.size(); i++) {
        switch (curRow) {
            // Not implemented and needs to be handled outside of this function
            case 0:
                break;
            //  Row 1 must have the first 4 elements as 3,2,3,2
            case 1:
                if (pmRows[i][0] != 3 || pmRows[i][1] != 2 || pmRows[i][2] != 3 || pmRows[i][3] != 2) break;
                vTemp = pmRows[curRow];
                pmRows[curRow] = pmRows[i];
                pmRows[i] = vTemp;
                return setupRows(pmRows, curRow + 1);
                break;
            //  Row 2 must have the first 2 elements as 2,3, the middle 2 as 0's or 1s, and the last 2 as 2s or 3s
            case 2: 
                if (pmRows[i][0] != 2 || pmRows[i][1] != 3 || pmRows[i][4] < 2 || pmRows[i][5] < 2) break;
                vTemp = pmRows[curRow];
                pmRows[curRow] = pmRows[i];
                pmRows[i] = vTemp;
                return setupRows(pmRows, curRow + 1);
                break;
            case 3: 
                if (pmRows[i][0] != 3 || pmRows[i][1] != 2 || pmRows[i][4] < 2 || pmRows[i][5] < 2) break;
                vTemp = pmRows[curRow];
                pmRows[curRow] = pmRows[i];
                pmRows[i] = vTemp;
                return setupRows(pmRows, curRow + 1);
                break;
            //  Row 4/5 must have the the last 4 elements as 2s or 3s and the first 2 as 0s or 1s
            //  At this point though, those should be the only row types left so let's handle the 2 combinations of them
            case 4:
                setupRows(pmRows, curRow + 1);
                vTemp = pmRows[curRow];
                pmRows[curRow] = pmRows[i];
                pmRows[i] = vTemp;
                return setupRows(pmRows, curRow + 1);
                break;
            case 5:
                // now we have a fuly formed matrix that matches the case 8 b layout, so we can print it out and claim victory
                return rearrangedPattern{pmRows, true};
                break;
        }
    }
    return rearrangedPattern{pmRows, false};
}

rearrangedPattern setupColumns(std::vector<std::vector<int>> pmCols, int curColumn) {
    std::vector<int> vTemp;
    bool columnsSet = false;
    for (int i = curColumn; i < pmCols.size(); i++) {
        switch (curColumn) {
            // These columns must be 2s to match the first row of 8b
            case 0: case 2: 
                if (pmCols[i][0] != 2) break;
                vTemp = pmCols[curColumn];
                pmCols[curColumn] = pmCols[i];
                pmCols[i] = vTemp;
                return setupColumns(pmCols, curColumn + 1);
                break;
            // These columns must be 3s to match the first row of 8b
            case 1: case 3:
                if (pmCols[i][0] != 3) break;
                vTemp = pmCols[curColumn];
                pmCols[curColumn] = pmCols[i];
                pmCols[i] = vTemp;
                return setupColumns(pmCols, curColumn + 1);
                break;
            // This column must be 0s or 1s, which, by default, it should be
            case 4:
                // Return the first combo
                return setupColumns(pmCols, curColumn + 1);
                // Setup and return the final combo
                vTemp = pmCols[curColumn];
                pmCols[curColumn] = pmCols[i];
                pmCols[i] = vTemp;
                return setupColumns(pmCols, curColumn + 1);
                break;
            // This column must be 0s or 1s, which, by default, it should be
            case 5:
                columnsSet = true;
                break;
        }
    }
    // If the columns weren't set, then we don't need to move onto the row swaps
    if (!columnsSet) return rearrangedPattern{pmCols, false};

    std::vector<std::vector<int>> pmRows = pmCols;  // set to columns to get the correct size
    for (int i = 0; i < pmCols.size(); i++) {
        for (int j = 0; j < pmCols[i].size(); j++) {
            pmRows[j][i] = pmCols[i][j];
        }
        
    }
    return setupRows(pmRows, 1);
}

rearrangedPattern case8brearange(patternMatrix pm) {
    rearrangedPattern rp;
    rp.done = false;
    for (int i = 0; i < pm.p.z.size(); i++) {
        // Select a row that has at least 1x 2 and 1x 3
        //  From the paper, I think we can actually require there to be 2x 2s and 2x 3s here but lets see what happens
        if (pm.p.zRowCounts[i][2] != 2 || pm.p.zRowCounts[i][3] != 2) continue;
        // Once we find a row that could work, it's time to rearrange the matrix to see if we can find a full matrix that works
        // First, swap the row to the top but, since we are working on columns first, we need to do that in the column view
        std::vector<std::vector<int>> pmCols = pm.pT.z;
        // Update the column view for the row swap
        for (int j = 0; j < pmCols.size(); j++) {
            int temp = pmCols[j][0];
            pmCols[j][0] = pmCols[j][i];
            pmCols[j][i] = temp;
        }
        // This will be a recursive function that will try all possible combinations of column swaps and, once done, move to row swaps
        rp = setupColumns(pmCols, 0);
    }
    if (rp.done) return rp;
    for (int i = 0; i < pm.pT.z.size(); i++) {
        // Select a row that has at least 1x 2 and 1x 3
        //  From the paper, I think we can actually require there to be 2x 2s and 2x 3s here but lets see what happens
        if (pm.pT.zRowCounts[i][2] == 0 || pm.pT.zRowCounts[i][3] == 0) continue;
        // Once we find a row that could work, it's time to rearrange the matrix to see if we can find a full matrix that works
        // First, swap the row to the top but, since we are working on columns first, we need to do that in the column view
        std::vector<std::vector<int>> pmCols = pm.p.z;
        // Update the column view for the row swap
        for (int j = 0; j < pmCols.size(); j++) {
            int temp = pmCols[j][0];
            pmCols[j][0] = pmCols[j][i];
            pmCols[j][i] = temp;
        }
        // This will be a recursive function that will try all possible combinations of column swaps and, once done, move to row swaps
        rp = setupColumns(pmCols, 0);
    }
    return rp;
}

int case8bSubCase(std::vector<std::vector<int>> z) {
    int sq1sum = z[2][2] + z[2][3] + z[3][2] + z[3][3];
    int sq2sum = z[4][0] + z[4][1] + z[5][0] + z[5][1];
    if (sq1sum == 0 && sq2sum == 0) return 1;
    else if (sq1sum == 4 && sq2sum == 0) return 2;
    else if (sq1sum == 0 && sq2sum == 4) return 3;
    else if (sq1sum == 4 && sq2sum == 4) return 4;
    return -1; // this should never happen and should probably throw an exception
}

int main(int argc, char **argv) {
    // std::string patternFile = "case8-test-match-pattern.txt";
    // std::string patternFile = "case8-matches-patterns785.txt";
    // std::string case8file = "case8b/case8-785-rearranged.txt";
    std::string patternFile = "case8-matches-patterns928.txt";
    std::string case8bFile = "case8b/case8b-928-rearranged.txt";
    std::string case8bFileDetails = "case8b/case8b-928-rearranged-details.txt";
    std::vector<patternMatrix> patterns = loadPatterns("matched-cases/" + patternFile);
    std::ofstream case8bOut(case8bFile);
    std::ofstream case8bOutDets(case8bFileDetails);
    if (!case8bOut.is_open()) {
        std::cerr << "Error opening case 8b output file!" << std::endl;
        return {}; // Indicate error
    }
    if (!case8bOutDets.is_open()) {
        std::cerr << "Error opening case 8b details output file!" << std::endl;
        return {}; // Indicate error
    }
    rearrangedPattern rp;
    for (patternMatrix pm : patterns) {
        rp = case8brearange(pm);
        if (rp.done) {
            int subCase = case8bSubCase(rp.rearrangedMatrix);
            case8bOutDets << "Pattern " << pm.id << ":" << pm << " is case 8b" << subCase << std::endl;
            case8bOutDets << "Rearranged pattern to match case 8b layout: " << std::endl;
            printMatrix(case8bOutDets, rp.rearrangedMatrix, true);
            case8bOut << pm.id << " " << subCase << " ";
            printMatrix(case8bOut, rp.rearrangedMatrix, false);
        }
    }
    case8bOut.close();
    case8bOutDets.close();
    return 0;
}
