#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"

std::string TFC_OUT_DIR = "tfc-output";

std::map<int, std::vector<char>> caseSubcases = {
    {1, {'-'}},
    {2, {'-'}},
    {3, {'a', 'b', 'c'}},
    {4, {'a', 'b'}},
    {5, {'a', 'b'}},
    {6, {'a', 'b', 'c'}},
    {7, {'-'}},
    {8, {'a', 'b'}},
};

std::string matchedCasesDirectory = "tfc-output";
std::string matchedSubcasesDirectory = "tfc-output";
bool useNewEncoding = false;

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int lineNumber = 0;
    while (std::getline(file, line)) {
        // ignore comments
        if (line[0] == '#') {
            std::cout << "Ignoring comment: " << line << std::endl;
            continue;
        }
        patterns.push_back(patternMatrix(++lineNumber, line, useNewEncoding));
    }
    file.close();
    return patterns;
}

void case352AllPossible() {
    std::filesystem::create_directory(TFC_OUT_DIR);
    std::ofstream tfcout = std::ofstream(TFC_OUT_DIR + "/tfc-output.txt");
    if (!tfcout.is_open()) {
        std::cerr << "Error opening file:" << TFC_OUT_DIR + "/tfc-output.txt" << std::endl;
        return;
    }
    
    patternMatrix test = patternMatrix(352);
    test.multilineOutput = true;
    //test.printDebugInfo = true;

    std::cout << "Before T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    test.rightTGateMultiply(1,4);
    test.rightTGateMultiply(2,3);

    std::cout << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // If you want to reduce and see the LDEs then see possible values, uncomment this block
    test.ldeReductionOnPattern(1);
    std::cout << "LDEs:" << std::endl;
    test.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    test.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
    
    std::cout << "Starting to generate all possible patterns" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    test.generateAllPossibleValuePatterns();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    std::cout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    std::cout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    tfcout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    tfcout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    tfcout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    tfcout << "All possible value patterns:" << std::endl;
    for (auto pm : test.allPossibleValuePatterns) {
        tfcout << pm.first << std::endl;

    }
    tfcout.close();
    std::cout << "Done" << std::endl;
}

void case352SubCaseMatching() {
    std::filesystem::create_directory(matchedSubcasesDirectory);
    std::map<int, std::string> patternFiles = {
        // {1, matchedCasesDirectory + "/case1-matches-patterns928.txt"},  // No subcases
        // {2, matchedCasesDirectory + "/case2-matches-patterns928.txt"},  // No subcases
        //{3, matchedCasesDirectory + "/case3-matches-patterns928.txt"},
        {3, matchedCasesDirectory + "/case3-patterns-tfc.txt"},
        //{4, matchedCasesDirectory + "/case4-matches-patterns928.txt"},
        //{5, matchedCasesDirectory + "/case5-matches-patterns928.txt"},
        //{6, matchedCasesDirectory + "/case6-matches-patterns928.txt"},
        // {7, matchedCasesDirectory + "/case7-matches-patterns928.txt"},  // No subcases
        //{8, matchedCasesDirectory + "/case8-matches-patterns928.txt"},
        };
    for (auto const& pair : patternFiles) {
        int caseNumber = pair.first;
        std::string patternFile = pair.second;
        std::vector<patternMatrix> patterns = loadPatterns(patternFile);
        std::vector<std::ofstream> matchedCasesFiles;
        std::vector<std::ofstream> matchedCasesFilesHumanReadable;
        matchedCasesFiles.push_back(std::ofstream(matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + "-no-subcase-match.txt"));
        matchedCasesFilesHumanReadable.push_back(std::ofstream(matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + "-no-subcase-matches-human-readable.txt"));
        for (char subcase : caseSubcases[caseNumber]) {
            std::string filename = matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + subcase + "-matches.txt";
            matchedCasesFiles.push_back(std::ofstream(filename));
            if (!matchedCasesFiles[matchedCasesFiles.size()-1].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
            matchedCasesFiles[matchedCasesFiles.size()-1] << "# Using the new encoding: 2y + x" << std::endl;

            std::string filenameHumanReadable = matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + subcase + "-matches-human-readable.txt";
            matchedCasesFilesHumanReadable.push_back(std::ofstream(filenameHumanReadable));
            if (!matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1].is_open()) {
                std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
            }
            matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1] << "# Using the new encoding: 2y + x" << std::endl;
        }

        for (patternMatrix pm : patterns) {
            std::cout << "Pattern " << pm.id << std::endl;
            if(!pm.matchesCase(caseNumber)) {
                std::cout << "Pattern " << pm.id << " does not match case " << caseNumber << " and is in the wrong file" << std::endl;
                std::cout << pm << std::endl;
                continue;
            }
            pm.caseMatch = caseNumber;
            pm.printID = true;
            
            if(pm.determineSubCase()) {
                int index = pm.subCaseMatch - 'a' + 1;
                matchedCasesFiles[index] << pm << std::endl;
                pm.multilineOutput = true;
                matchedCasesFilesHumanReadable[index] << pm << std::endl;
            } else {
                std::cout << "Pattern " << pm.id << " has no subcase matches." << std::endl;
                matchedCasesFiles[0] << pm << std::endl;
                pm.multilineOutput = true;
                matchedCasesFilesHumanReadable[0] << pm << std::endl;
            
            }
        }
        for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
        for (std::ofstream& matchedCasesFile : matchedCasesFilesHumanReadable) matchedCasesFile.close();
    }
}

void test352Possible() {
    patternMatrix test = patternMatrix(67,"[1,1,1,1,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][1,3,1,3,0,0][1,3,3,1,0,0]", true);
    test.multilineOutput = true;

    test.rearrangeMatrix();
    patternMatrix test2 = patternMatrix(67, test.getFirstCaseRearrangement());
    test2.multilineOutput = true;

    test2.printPairCounts(std::cout);
    
    
    std::cout << "Before T-Gate multiplication " << test2.printTGateOperations() << ":" << std::endl;
    std::cout << test2 << std::endl;

    // Here, you can mulitply the pattern with T-Gates.  Feel free to update as desired.
    //  You can use the following functions to multiply the pattern with T-Gates:
    //    test.rightTGateMultiply(column1, column2) - Multiply the pattern with T-Gate on the right
    //    test.leftTGateMultiply(row1, row2) - Multiply the pattern with T-Gate on the left
    test2.rightTGateMultiply(1,4);
    test2.rightTGateMultiply(2,3);

    std::cout << "After T-Gate multiplication " << test2.printTGateOperations() << ":" << std::endl;
    std::cout << test2 << std::endl;

    // If you want to reduce and see the LDEs then see possible values, uncomment this block
    test2.ldeReductionOnPattern(1);
    std::cout << "LDEs:" << std::endl;
    test2.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    test2.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
}

int main(int argc, char **argv) {
    test352Possible();
    return 0;
}