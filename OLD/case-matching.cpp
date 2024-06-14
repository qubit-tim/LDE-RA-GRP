#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "pattern-matrix.hpp"
#include "case-matrix.hpp"

std::vector<caseMatrix> loadCases() {
    std::ifstream file("cases/cases.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<caseMatrix> cases;
    std::string line;

    while (std::getline(file, line)) {
        int caseNumber = std::stoi(line.substr(0, line.find(" ")));
        cases.push_back(caseMatrix(caseNumber, line.substr(line.find(" ")+1)));
    }
    file.close();
    return cases;
}

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int patternNumber = 1;
    while (std::getline(file, line)) {
        patterns.push_back(patternMatrix(patternNumber++, line));
    }
    file.close();
    return patterns;
}

int main(int argc, char **argv) {
    std::vector<caseMatrix> cases = loadCases();
    for (caseMatrix cm : cases) {
        std::cout << cm << std::endl;
    }
    // There's an assumption that the folder "matched-cases" exists during the pattern matching
    //std::vector<std::string> patternFiles = {"patterns-test"};  // Use this for testing so there's not a ton of output to go through
    std::vector<std::string> patternFiles = {"patterns785", "patterns928", "patterns2704"};
    for (std::string patternFile : patternFiles) {
        std::vector<patternMatrix> patterns = loadPatterns("patterns/" + patternFile + ".txt");
        std::vector<std::ofstream> matchedCasesFiles;
        // Since cases are numbered starting at 1, it's safe to put the no-matches file at index 0 which then aligns the index with the case numbers
        matchedCasesFiles.push_back(std::ofstream("matched-cases/no-matches-" + patternFile + ".txt"));
        for (caseMatrix cm : cases) {
            std::string filename = "matched-cases/case" + std::to_string(cm.id) + "-matches-" + patternFile + ".txt";
            matchedCasesFiles.push_back(std::ofstream(filename));
            if (!matchedCasesFiles[cm.id].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
        }
        for (patternMatrix pm : patterns) {
            std::cout << "Pattern " << pm << std::endl;
            int matchNumber = pm.matchedCase(cases);
            // If there are no matches, put the pattern in the no-matches file
            if (matchNumber == -1) {
                std::cout << "Pattern " << pm.id << " has no matches." << std::endl << std::endl;
                matchedCasesFiles[0] << pm << std::endl;
            } else {
                std::cout << "Pattern " << pm.id << " matches: " << matchNumber << std::endl << std::endl;
                matchedCasesFiles[matchNumber] << pm.id << " " << pm << std::endl;
            }
        }
        for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
    }
    return 0;
}