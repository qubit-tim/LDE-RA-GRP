#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <map>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/data/patterns928.hpp"

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

std::string matchedCasesDirectory = "matched-cases";
std::string matchedSubcasesDirectory = "matched-subcases";

void caseSubcaseMatch(int caseNum, bool validPatterns) {
    std::string validityString = validPatterns ? "valid-patterns" : "invalid-patterns";
    std::vector<std::ofstream> matchedCasesFiles;
    std::vector<std::ofstream> matchedCasesFilesHumanReadable;
    matchedCasesFiles.push_back(std::ofstream(matchedSubcasesDirectory+ "/" + validityString + "-case" + std::to_string(caseNum) + "-no-subcase-match.txt"));
    matchedCasesFilesHumanReadable.push_back(std::ofstream(matchedSubcasesDirectory+ "/" + validityString + "-case" + std::to_string(caseNum) + "-no-subcase-matches-human-readable.txt"));
    for (char subcase : caseSubcases[caseNum]) {
        if (subcase == '-') {
            continue;
        }
        std::string filename = matchedSubcasesDirectory+ "/" + validityString + "-case" + std::to_string(caseNum) + subcase + "-matches.txt";
        matchedCasesFiles.push_back(std::ofstream(filename));
        if (!matchedCasesFiles[matchedCasesFiles.size()-1].is_open()) {
            std::cerr << "Error opening file:" << filename << std::endl;
        }
        matchedCasesFiles[matchedCasesFiles.size()-1] << "# Using the new encoding: 2y + x" << std::endl;

        std::string filenameHumanReadable = matchedSubcasesDirectory+ "/" + validityString + "-case" + std::to_string(caseNum) + subcase + "-matches-human-readable.txt";
        matchedCasesFilesHumanReadable.push_back(std::ofstream(filenameHumanReadable));
        if (!matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1].is_open()) {
            std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
        }
        matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1] << "# Using the new encoding: 2y + x" << std::endl;
    }

    for (const auto& pair : CASE_PATTERN_MAP_PATTERNS_928[caseNum]) {
        int patternID = pair.first;
        if (validPatterns && !PATTERN_928_VALIDITY[patternID]) {
            continue;
        }
        if (!validPatterns && PATTERN_928_VALIDITY[patternID]) {
            continue;
        }
        patternMatrix pm = patternMatrix(patternID);
        pm.printOldEncoding = false;
        pm.printID = true;
        std::cout << "Pattern " << pm.id << std::endl;
        if(!pm.matchesCase(caseNum)) {
            std::cout << "Pattern " << pm.id << " does not match case " << caseNum << " and is in the part of the map" << std::endl;
            std::cout << pm << std::endl;
            continue;
        }
        //pm.caseMatch = caseNum;
        if(pm.determineSubCase()) {
            int index = pm.subCaseMatch - 'a' + 1;
            if (pm.subCaseMatch == '-') {
                index = 0;
            }
            std::cout << "Pattern " << pm.id << " matches case " << caseNum << " subcase " << pm.subCaseMatch << std::endl;
            matchedCasesFiles[index] << pm << std::endl;
            pm.multilineOutput = true;
            matchedCasesFilesHumanReadable[index] << pm << std::endl;
        } else {
            std::cout << "Pattern " << pm.id << " matches case " << caseNum << " and has no subcase matches." << std::endl;
            matchedCasesFiles[0] << pm << std::endl;
            pm.multilineOutput = true;
            matchedCasesFilesHumanReadable[0] << pm << std::endl;
        }
    }
    for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
    for (std::ofstream& matchedCasesFile : matchedCasesFilesHumanReadable) matchedCasesFile.close();
}


int main(int argc, char **argv) {
    std::filesystem::create_directory(matchedCasesDirectory);
    std::filesystem::create_directory(matchedSubcasesDirectory);
    std::cout << "Matching valid patterns" << std::endl;
    for (int i = 1; i <= 8; i++) {
        caseSubcaseMatch(i, true);
    }
    std::cout << "Matching invalid patterns" << std::endl;
    for (int i = 1; i <= 8; i++) {
        caseSubcaseMatch(i, false);
    }
    std::cout << "Done matching cases and subcases" << std::endl;
    std::cout << "All matched cases and subcases are in the directory: " << matchedSubcasesDirectory << std::endl;
    std::cout << "All matched cases are in the directory: " << matchedCasesDirectory << std::endl;
    return 0;
}
