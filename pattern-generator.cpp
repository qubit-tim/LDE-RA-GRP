#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <future>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"

int rows = 6;
int cols = 6;
// int patternIterations = 0;
std::map<int, std::vector<patternMatrix>> caseToPatternMap;
auto start_time = std::chrono::high_resolution_clock::now();

void recursiveSet(int position, zmatrix z, std::vector<patternMatrix> &results) {
    for (int i = 0; i < 2; i++) {
        z.z[position / cols][position % cols] += i;
        if (position == (rows * cols) - 1) {
            /*
            patternIterations++;
            if (patternIterations % 100000 == 0) {
                std::cout << "Pattern Iterations: " << patternIterations << std::endl;
                std::cout << "Pattern: " << z << std::endl;
                auto current_time = std::chrono::high_resolution_clock::now();
                std::cout << "The time per iteration is " << std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count() / patternIterations << " microseconds" << std::endl;
            }
            */
            std::ostringstream os;
            os << z;
            patternMatrix pm = patternMatrix(1, os.str());
            pm.matchOnCases();
            if (pm.caseMatch > 0 && pm.isOrthogonal() && pm.isNormalized()) {
                std::cout << "Case: " << pm.caseMatch << " Valid Pattern:" << pm << std::endl;
                results.push_back(pm);
            }
        } else {
            recursiveSet(position + 1, z, results);
        }
    }
}

std::vector<patternMatrix> patternGen(patternMatrix base) {
    std::vector<patternMatrix> results;
    recursiveSet(0, base.p, results);
    return results;
}

int main(int argc, char **argv) {
    // Going to try and brute force generate all of the possible patterns based off of case matching
    //  and orthonormality
    bool fileError = false;
    std::vector<std::ofstream> generatedPatternFiles;
    // dummy file to keep the index the same as the case number
    generatedPatternFiles.push_back(std::ofstream("generated-patterns/no-matches.txt"));
    for (int i = 1; i <= 8; i++) {
        std::string filename = "generated-patterns/case" + std::to_string(i) + ".txt";
        generatedPatternFiles.push_back(std::ofstream(filename));
        if (!generatedPatternFiles[i].is_open()) {
            std::cerr << "Error opening file:" << filename << std::endl;
            fileError = true;
        }
    }
    if (fileError) {
        return 1;
    }
    // If we start with the base cases but in pattern format with 2s and 0s,
    //  we can generate all the possible patterns for a case by adding 0 or 1 to each position
    //  and then check for orthonormality
    std::vector<patternMatrix> baseCases;
    baseCases.push_back(patternMatrix(1, "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(2, "[2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(3, "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,2,2,0,0][2,2,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(4, "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(5, "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(6, "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,2,2][2,2,0,0,2,2][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(patternMatrix(7, "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,2,2][0,0,0,0,2,2]"));
    baseCases.push_back(patternMatrix(8, "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,2,2][2,2,0,0,2,2][0,0,2,2,2,2][0,0,2,2,2,2]"));
    std::vector<std::future<std::vector<patternMatrix>>> futures;
    for (patternMatrix pm : baseCases) {
        auto res = std::async(std::launch::async, patternGen, pm);
        futures.push_back(std::move(res));
    }
    for (auto &f : futures) {
        f.wait();
    }
    std::cout << "Done!\nResults are: ";
    for (auto &f : futures) {
        std::vector<patternMatrix> results = f.get();
        for (patternMatrix pm : results) {
            caseToPatternMap[pm.caseMatch].push_back(pm);
        }
    }

    for (auto const& [caseNumber, patterns] : caseToPatternMap) {
        // these should never happen
        if (caseNumber <= 0) continue;  // Skip the no-matches file (index 0)
        if (caseNumber >8) continue;
        std::cout << "Case: " << caseNumber << std::endl;
        for (patternMatrix pm : patterns) {
            std::cout << pm << std::endl;
            generatedPatternFiles[caseNumber] << pm << std::endl;
        }
    }

    for (std::ofstream& file : generatedPatternFiles) file.close();
    return 0;
}
