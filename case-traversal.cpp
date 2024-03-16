/*
Alrighty...now the fun part.
Here we need to load all of the potential t-gates and then start working on the pattern matrix to go
through the case transition table in the paper, which I'll summarize here:

Case -> Case they can go to
8 -> 1-6, top
7 -> 5,6
6 -> 2,3,5
5 -> 1,2
4 -> 1,2
3 -> top
2 -> top
1 -> top

What we want to trace is the following path:
8 -> 6 -> 3 -> top
*/
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
        std::cerr << "Error opening pattern file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    while (std::getline(file, line)) {
        int id = std::stoi(line.substr(0, line.find(" ")));
        int case8bSubCase = std::stoi(line.substr(line.find(" ")+1, line.find(" ", line.find(" ")+1)));
        patternMatrix p = patternMatrix(id, line.substr(line.find(" ", line.find(" ")+1), line.size()-1));
        p.caseMatch = 8;
        p.subCaseMatch = case8bSubCase;
        patterns.push_back(p);
    }
    file.close();
    return patterns;
}

int main(int argc, char **argv) {
    // Load up the case 8b pattern matrix
    std::string case8bFile = "case8b/case8b-928-rearranged.txt";
    std::vector<patternMatrix> patterns = loadPatterns(case8bFile);

    std::cout << "WIP" << std::endl;
    
    return 0;
}
