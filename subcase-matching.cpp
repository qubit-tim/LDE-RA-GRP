#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <map>

#include "LDE-Matrix/pattern-matrix.hpp"

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

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    while (std::getline(file, line)) {
        // ignore comments
        if (line[0] == '#') {
            std::cout << "Ignoring comment: " << line << std::endl;
            continue;
        }
        int id = std::stoi(line.substr(0, line.find(' ')));
        std::string p = line.substr(line.find(' ')+1);
        patterns.push_back(patternMatrix(id, p, true));
    }
    file.close();
    return patterns;
}

int main(int argc, char **argv) {
    std::filesystem::create_directory("matched-subcases");
    std::map<int, std::string> patternFiles = {
        // {1, matchedCasesDirectory + "/case1-matches-patterns928.txt"},  // No subcases
        // {2, matchedCasesDirectory + "/case2-matches-patterns928.txt"},  // No subcases
        {3, matchedCasesDirectory + "/case3-matches-patterns928.txt"},
        {4, matchedCasesDirectory + "/case4-matches-patterns928.txt"},
        {5, matchedCasesDirectory + "/case5-matches-patterns928.txt"},
        {6, matchedCasesDirectory + "/case6-matches-patterns928.txt"},
        // {7, matchedCasesDirectory + "/case7-matches-patterns928.txt"},  // No subcases
        {8, matchedCasesDirectory + "/case8-matches-patterns928.txt"},
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
    return 0;
}
