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

// Here, name1 and name2 are the names of the pattern files to compare minus the .txt extension and must be in the patterns/ directory
// This function will create a file in the patterns/ directory with the name name1-minus-name2.txt
void comparePatternFiles(std::string name1, std::string name2) {
    std::vector<patternMatrix> patterns1 = loadPatterns("patterns/" + name1 + ".txt");
    std::vector<patternMatrix> patterns2 = loadPatterns("patterns/" + name2 + ".txt");
    std::ofstream p1m2("patterns/" + name1 + "-minus-" + name2 + ".txt");
    for (patternMatrix pm1 : patterns1) {
        bool found = false;
        for (patternMatrix pm2 : patterns2) {
            if (pm1.p == pm2.p) {
                found = true;
                break;
            }
            /* This shouldn't be necessary
            if (pm1.p == pm2.pT) {
                std::cout << "Found a transpose match" << std::endl;
                found = true;
                break;
            }
            if (pm1.p == pm2.swap23) {
                std::cout << "Found a 2/3 swap match" << std::endl;
                found = true;
                break;
            }
            if (pm1.p == pm2.swap23T) {
                std::cout << "Found a 2/3 swap transpose match" << std::endl;
                found = true;
                break;
            }
            */
        }
        if (!found) {
            p1m2 << pm1 << std::endl;
        }
    }
    p1m2.close();
}

int main(int argc, char **argv) {
    std::vector<patternMatrix> patterns2704G3 = loadPatterns("deduping/patterns2704-s3-g3-no-23-swap-T-dupes.txt");
    std::cout << "Verifing 785-numerical == 785" << std::endl;
    comparePatternFiles("patterns785-numerical", "patterns785");
    std::cout << "Verifing 928-numerical == 928" << std::endl;
    comparePatternFiles("patterns928-numerical", "patterns928");
    std::cout << "Verifing 2704-numerical == 2704" << std::endl;
    comparePatternFiles("patterns2704-numerical", "patterns2704");
    std::cout << "Diffing 928 with 785" << std::endl;
    comparePatternFiles("patterns928", "patterns785");
    std::cout << "Diffing 2704 with 785" << std::endl;
    comparePatternFiles("patterns2704", "patterns785");
    std::cout << "Diffing 2704 with 928" << std::endl;
    comparePatternFiles("patterns2704", "patterns928");
    return 0;
}