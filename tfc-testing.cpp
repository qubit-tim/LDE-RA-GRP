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

int main(int argc, char **argv) {
    std::filesystem::create_directory(TFC_OUT_DIR);
    std::ofstream tfcout = std::ofstream(TFC_OUT_DIR + "/tfc-output.txt");
    if (!tfcout.is_open()) {
        std::cerr << "Error opening file:" << TFC_OUT_DIR + "/tfc-output.txt" << std::endl;
        return 1;
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
    return 0;
}