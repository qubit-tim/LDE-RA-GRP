#include <algorithm>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <map>
#include <regex>
#include <future>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"
#include "LDE-Matrix/data/patterns928.hpp"
#include "LDE-Matrix/pattern-deduper.hpp"


std::string MING_OUT_DIR = "ming-output";

int main(int argc, char **argv) {
    std::filesystem::create_directory(MING_OUT_DIR);
    
    // CHANGE THESE FOR FILE NAME CHANGES
    // ------------------------------------------------------------------------------------------------------
    std::ofstream mingout = std::ofstream(MING_OUT_DIR + "/ming-output.txt");
    std::ofstream mingUniques = std::ofstream(MING_OUT_DIR + "/ming-uniques.txt");
    // ------------------------------------------------------------------------------------------------------
    
    if (!mingout.is_open()) {
        std::cerr << "Error opening file:" << MING_OUT_DIR + "/tfc-output.txt" << std::endl;
        return 0;
    }

    if (!mingUniques.is_open()) {
        std::cerr << "Error opening file:" << MING_OUT_DIR + "/tfc-uniques.txt" << std::endl;
        return 0;
    }

    // 352 is the pattern number from the 928 file
    //  Feel free to replace with any other pattern number you want to test
    // ------------------------------------------------------------------------------------------------------
    patternMatrix test = patternMatrix(352);
    test.multilineOutput = true;
    test.printDebugInfo = true;
    // ------------------------------------------------------------------------------------------------------ 

    std::cout << "Before T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // This is the T-Gate multiplication
    // Change these to either rightTGateMultiply(i,j) or leftTGateMultiply(i,j)
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
    // This generates all possible value patterns and stores them in the old encoding scheme
    test.generateAllPossibleValuePatterns();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    std::cout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    std::cout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    mingout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    mingout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    mingout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    mingout << "Deduping:" << std::endl;
    patternDeduper pd = patternDeduper();
    int newPatternID = 100000000;
    std::map<int, int> dupCount;
    for (auto pm : test.allPossibleValuePatterns) {
        int duplicateID = -1;
        // By default, these are in the old encoding but this could change :(
        patternMatrix pmCopy = patternMatrix(++newPatternID, pm.first);
        if (pd.isDuplicate(pmCopy, duplicateID, true)) {
            std::cout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            mingout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            dupCount[duplicateID]++;
        } else {
            std::cout << pmCopy.id << " is unique" << std::endl;
            mingout << pmCopy.id << " is unique" << std::endl;
            mingUniques << pmCopy << std::endl;
        }
    }
    mingout << "Duplicate Counts:" << std::endl;
    mingUniques << "Duplicate Counts:" << std::endl;
    std::map<char, std::vector<int>> dupSubCase;
    for (auto const& [id, count] : dupCount) {
        std::cout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        mingout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        mingUniques << "Duplicate ID: " << id << " Count: " << count << std::endl;
        patternMatrix temp = patternMatrix(id);
        temp.determineSubCase();
        dupSubCase[temp.subCaseMatch].push_back(id);
    }
    // Print out the subcases of 3x
    for (auto const& [subCase, ids] : dupSubCase) {
        std::cout << "SubCase: " << subCase << " Count: " << ids.size() << std::endl;
        mingout << "SubCase: " << subCase << " Count: " << ids.size() << std::endl;
        mingUniques << "SubCase: " << subCase << " Count: " << ids.size() << std::endl;
        for (auto id : ids) {
            std::cout << id << std::endl;
            mingout << id << std::endl;
            mingUniques << id << std::endl;
        }
    }
    mingout.close();
    mingUniques.close();
    std::cout << "Done" << std::endl;

    return 0;
}