#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include "LDE-Matrix/pattern-matrix.hpp"

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
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
    std::filesystem::create_directory("matched-cases");
    //std::vector<std::string> patternFiles = {"patterns-test"};  // Use this for testing so there's not a ton of output to go through
    //std::vector<std::string> patternFiles = {"patterns785", "patterns928", "patterns2704"};
    std::vector<std::string> patternFiles = {"patterns928"};
    for (std::string patternFile : patternFiles) {
        std::vector<patternMatrix> patterns = loadPatterns("patterns/" + patternFile + ".txt");
        std::vector<std::ofstream> matchedCasesFiles;
        std::vector<std::ofstream> matchedCasesFilesHumanReadable;
        // Since cases are numbered starting at 1, it's safe to put the no-matches file at index 0 which then aligns the index with the case numbers
        matchedCasesFiles.push_back(std::ofstream("matched-cases/no-matches-" + patternFile + ".txt"));
        matchedCasesFilesHumanReadable.push_back(std::ofstream("matched-cases/no-matches-" + patternFile + "-human-readable.txt"));
        for (int i = 1; i <= 8; i++) {
            std::string filename = "matched-cases/case" + std::to_string(i) + "-matches-" + patternFile + ".txt";
            matchedCasesFiles.push_back(std::ofstream(filename));
            if (!matchedCasesFiles[i].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
            matchedCasesFiles[i] << "# Using the new encoding: 2y + x" << std::endl;

            std::string filenameHumanReadable = "matched-cases/case" + std::to_string(i) + "-matches-" + patternFile + "-human-readable.txt";
            matchedCasesFilesHumanReadable.push_back(std::ofstream(filenameHumanReadable));
            if (!matchedCasesFilesHumanReadable[i].is_open()) {
                std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
            }
            matchedCasesFilesHumanReadable[i] << "# Using the new encoding: 2y + x" << std::endl;
        }
        for (patternMatrix pm : patterns) {
            // std::cout << "Pattern " << pm << std::endl;
            // Set this flag to stop the case rearrangement code after a single solution is found
            pm.singleCaseRearrangement = true;
            pm.printID = true;
            bool success = pm.rearrangeMatrix();
            // If there are no matches, put the pattern in the no-matches file
            if (!success) {
                // std::cout << "Pattern " << pm.id << " has no matches." << std::endl << std::endl;
                matchedCasesFiles[0] << pm << std::endl;
                pm.multilineOutput = true;
                matchedCasesFilesHumanReadable[0] << pm << std::endl;
            } else {
                // std::cout << "Pattern " << pm.id << " matches: " << pm.caseMatch << std::endl << std::endl;
                // Toggle printing the first case match
                bool print = true;
                for (auto const& pair : pm.caseRearrangements) {
                    if (!print) {
                        break;
                    }
                    auto key = pair.first;
                    patternMatrix pmCopy = patternMatrix(pm.id,key);
                    pmCopy.printID = true;
                    matchedCasesFiles[pm.caseMatch] << pmCopy << std::endl;
                    pmCopy.multilineOutput = true;
                    matchedCasesFilesHumanReadable[pm.caseMatch] << pmCopy << std::endl;
                    print = false;
                    if (!pm.isOrthonormal()) {
                        std::cout << "========================================" << std::endl;
                        std::cout << "Pattern " << pm.id << " is NOT orthonormal." << std::endl;
                        std::cout << "Case match: " << pm.caseMatch << std::endl;
                        std::cout << "Original Pattern: " << pm << std::endl;
                        std::cout << "Rearranged Version: " << key << std::endl;
                        pm.printDebugInfo = true;
                        pm.isOrthonormal();
                        std::cout << "========================================" << std::endl;
                    }
                }
            }
        }
        for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
    }
    return 0;
}
