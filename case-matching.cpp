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

void duplicatePatternChecks() {
    // Here we want to check for duplicate patterns within all of the pattern files
    // This needs to include any rearranging of patterns and matching on transposes w/ rearrages
    // My thought on doing this is to narrow down possibilities and then work from there
    // - First, sort into cases
    // - Second, compare each pattern to each other pattern in the same case
    //   - To make this easier, the patternMatrix class could have a == operator setup
    //   - To make that a bit quicker, we could count the number of 0s, 1s, 2s, 3s in each pattern and compare that first
    //   - Then it might make sense to do something similar to what's done with the patterns and have a row and column count for each value
    //   - Then we can compare those like what's done for the cases and go from there
    //   - Actually, this makes a lot of sense.  The case matching looks at counts for 1s which could be extended into 0s, 2s, and 3s
    //   - This will act like a signature for the pattern and make it a lot easier to compare
    //   - I wonder if a unique signature or hash could be generated based on these counts and then do comparisons based on that
    //   - Thinking a bit more, it might be quicker to do sort the patterns into buckets based on things we know about them, like what case they match,
    //   - how many 0-3s they have, etc.  Then we can compare within the buckets
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
            //pm.printDebug();
        }
        for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
    }
    return 0;
}