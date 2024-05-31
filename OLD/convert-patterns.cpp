#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "pattern-matrix.hpp"
#include "case-matrix.hpp"

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
    std::vector<std::string> patternFiles = {"patterns785", "patterns928", "patterns2704"};
    for (std::string patternFile : patternFiles) {
        std::vector<patternMatrix> patterns = loadPatterns("patterns/" + patternFile + ".txt");
        std::ofstream pnumerical("patterns-numerical/" + patternFile + "-numerical.txt");
        for (patternMatrix pm : patterns) {
            pnumerical << pm.p << std::endl;
        }
        pnumerical.close();
    }
    return 0;
}
