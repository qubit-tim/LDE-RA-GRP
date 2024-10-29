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
#include <unordered_map>
#include <regex>
#include <future>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"
#include "LDE-Matrix/data/patterns928.hpp"
#include "LDE-Matrix/pattern-deduper.hpp"

std::string USER_OUT_DIR = "user-output";
std::regex R_T_GATE_REGEX("(xT[1-6][1-6])");
std::regex L_T_GATE_REGEX("(T[1-6][1-6]x)");

bool validTGateOps(std::vector<std::string> tGateOps) {
    // T-Gate operations should be in one of the following formats:
    //  xTij - This is a right T-Gate operation on columns i and j
    //  Tijx - This is a left T-Gate operation on rows i and j
    // std::regex rightTGateRegex("(xT[1-6][1-6])");
    // std::regex leftTGateRegex("(T[1-6][1-6]x)");

    for (auto tGateOp : tGateOps) {
        if (tGateOp.size() != 4) {
            return false;
        }
        if (!std::regex_match(tGateOp, R_T_GATE_REGEX) && !std::regex_match(tGateOp, L_T_GATE_REGEX)) {
            return false;
        }
        if (std::regex_match(tGateOp, R_T_GATE_REGEX) && tGateOp[2] != tGateOp[3]) {
            continue;
        }
        if (std::regex_match(tGateOp, L_T_GATE_REGEX) && tGateOp[1] != tGateOp[2]) {
            continue;
        }

    }
    return true;
}

void runWithOptions(int pNum, std::vector<std::string> tGateOps, bool printDebug, bool patternDebug, bool fullReduction, bool optimizedGenerate, bool o2Generate) {
    // Limit this to 3 T Gate Ops per side (3x left and 3x right but not more)
    /*
    if (tGateOps.size() < 0 || tGateOps.size() > 4) {
        std::cerr << "Invalid number of T-Gate operations provided:" << std::endl;
        std::cerr << "Got: " << std::to_string(tGateOps.size()) << " Wanted: 1 <= x <= 4"  << std::endl;
        return;
    }
    */
    if (pNum < 1 || pNum > 928) {
        std::cerr << "Invalid pattern number" << std::endl;
        std::cerr << "Got: " << std::to_string(pNum) << " Wanted: 1 <= x <= 928"  << std::endl;
        return;
    }
    if (!validTGateOps(tGateOps)) {
        std::cerr << "Invalid T-Gate operations provided" << std::endl;
        return;
    }

    std::string tGateOpsString = "";
    for (auto tGateOp : tGateOps) {
        tGateOpsString += tGateOp + "-";
    }

    std::string fileNameBase = "/p" + std::to_string(pNum) + "-" + tGateOpsString;
    std::string optimizedVersion = (o2Generate) ? "opt2" : (optimizedGenerate) ? "opt1" : "";
    std::string logFileName = USER_OUT_DIR + fileNameBase + "log-" + optimizedVersion + ".txt";
    std::string humanOutputFileName = USER_OUT_DIR + fileNameBase + optimizedVersion + ".txt";
    std::filesystem::create_directory(USER_OUT_DIR);
    std::ofstream logOutput = std::ofstream(logFileName);
    std::ofstream humanOutput = std::ofstream(humanOutputFileName);
    if (!logOutput.is_open()) {
        std::cerr << "Error opening file:" << logFileName << std::endl;
        return;
    }
    if (!humanOutput.is_open()) {
        std::cerr << "Error opening file:" << humanOutputFileName << std::endl;
        return;
    }

    patternMatrix test = patternMatrix(pNum);
    test.multilineOutput = true;
    test.printDebugInfo = patternDebug;
    if(patternDebug) {
        test.debugOutput = &logOutput;
    }

    logOutput << "Pattern Number: " << pNum << std::endl;
    logOutput << "Before T-Gate multiplication:" << std::endl;
    logOutput << test << std::endl;
    humanOutput << "Pattern Number: " << pNum << std::endl;
    humanOutput << "Before T-Gate multiplication:" << std::endl;
    humanOutput << test << std::endl;
    if (printDebug) {
        std::cout << "Pattern Number: " << pNum << std::endl;
        std::cout << "Before T-Gate multiplication:" << std::endl;
        std::cout << test << std::endl;
    }

    for (auto tGateOp : tGateOps) {
        if (std::regex_match(tGateOp, R_T_GATE_REGEX)) {
            std::string c1(1, tGateOp[2]);
            std::string c2(1, tGateOp[3]);
            test.rightTGateMultiply(std::stoi(c1), std::stoi(c2));
        }
        if (std::regex_match(tGateOp, L_T_GATE_REGEX)) {
            std::string r1(1, tGateOp[1]);
            std::string r2(1, tGateOp[2]);
            test.leftTGateMultiply(std::stoi(r1), std::stoi(r2));
        }
    }

    logOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    logOutput << test << std::endl;
    humanOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    humanOutput << test << std::endl;
    if (printDebug) {
        std::cout << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
        std::cout << test << std::endl;
    }

    logOutput << "LDEs Before Reduction(s):" << std::endl;
    test.printLDEs(logOutput);
    logOutput << std::endl;
    humanOutput << "LDEs Before Reduction(s):" << std::endl;
    test.printLDEs(humanOutput);
    humanOutput << std::endl;
    if (printDebug) {
        std::cout << "LDEs Before Reduction(s):" << std::endl;
        test.printLDEs(std::cout);
        std::cout << std::endl;
    }

    int maxLDE = test.getMaxLDEValue();
    for (int i = maxLDE; i > 0; i--) {
        logOutput << "Doing an LDE reduction on values of " << i << std::endl;
        if (printDebug) {
            std::cout << "Doing an LDE reduction on values of " << i << std::endl;
        }
        test.ldeReductionOnPattern(i);
    }
    if (fullReduction && test.canFullyReduceLDE()) {
        logOutput << "Doing a full LDE reduction on the pattern." << std::endl;
        if (printDebug) {
            std::cout << "Doing a full LDE reduction on the pattern." << std::endl;
        }
        test.ldeReductionOnPattern(0);
    }

    logOutput << std::endl;
    if (printDebug) {
        std::cout << std::endl;
    }
    
    logOutput << "LDEs After Reduction(s):" << std::endl;
    test.printLDEs(logOutput);
    logOutput << "Possible values:" << std::endl;
    test.printPossibleValues(logOutput);
    logOutput << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
    humanOutput << "LDEs After Reduction(s):" << std::endl;
    test.printLDEs(humanOutput);
    humanOutput << "Possible values:" << std::endl;
    test.printPossibleValues(humanOutput);
    humanOutput << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
    if (printDebug) {
        std::cout << "LDEs After Reduction(s):" << std::endl;
        test.printLDEs(std::cout);
        std::cout << "Possible values:" << std::endl;
        test.printPossibleValues(std::cout);
        std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
    }

    logOutput << "Starting to generate all possible patterns" << std::endl;
    if (printDebug) {
        std::cout << "Starting to generate all possible patterns" << std::endl;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    // This generates all possible value patterns and stores them in the old encoding scheme
    if (o2Generate) {
        test.opt2GenerateAllPossibleValuePatterns();
    } else if (optimizedGenerate) {
        test.optimizedGenerateAllPossibleValuePatterns();
    } else {
        test.generateAllPossibleValuePatterns();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto allPatternsTime = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    auto onePatternTime = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    if (test.allPossibleValuePatterns.size() > 0) {
        onePatternTime = onePatternTime / test.allPossibleValuePatterns.size();
    }
    if (printDebug) {
        std::cout << "Time to generate all possible patterns: " << allPatternsTime  << " milliseconds" << std::endl;
        std::cout << "Time to generate 1 valid pattern: " << onePatternTime  << " microseconds" << std::endl;
        std::cout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
        std::cout << "Deduping:" << std::endl;
    }
    logOutput << "Time to generate all possible patterns: " << allPatternsTime << " milliseconds" << std::endl;
    logOutput << "Time to generate 1 valid pattern: " << onePatternTime << " microseconds" << std::endl;
    logOutput << "Number of valid patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    humanOutput << "Time to generate all possible patterns: " << allPatternsTime << " milliseconds" << std::endl;
    humanOutput << "Time to generate 1 valid pattern: " << onePatternTime << " microseconds" << std::endl;
    humanOutput << "Number of valid patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    if (test.allPossibleValuePatterns.size() == 0) {
        if (printDebug) {
            std::cout << "No valid patterns to dedupe" << std::endl;
        }
        logOutput << "No valid patterns to dedupe" << std::endl;
        logOutput << "Invalid pattern" << std::endl;
        humanOutput << "Invalid pattern" << std::endl;
        logOutput.close();
        humanOutput.close();
        return;
    }
    logOutput << "Deduping:" << std::endl;
    patternDeduper pd = patternDeduper();
    int newPatternID = 1000000 * pNum;
    std::map<int, int> dupCount;
    for (auto pm : test.allPossibleValuePatterns) {
        int duplicateID = -1;
        // By default, these are in the old encoding but this could change :(
        patternMatrix pmCopy = patternMatrix(++newPatternID, pm.first);
        if (pd.isDuplicate(pmCopy, duplicateID, true)) {
            if (printDebug) {
                std::cout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            }
            logOutput << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            dupCount[duplicateID]++;
        } else {
            if (printDebug) {
                std::cout << pmCopy.id << " is unique" << std::endl;
            }
            logOutput << pmCopy.id << " is unique" << std::endl;
            humanOutput << pmCopy << " is unique" << std::endl;
        }
    }
    if (printDebug) {
        std::cout << "Duplicate Counts:" << std::endl;
    }
    logOutput << "Duplicate Counts:" << std::endl;
    humanOutput << "Duplicate Counts:" << std::endl;
    std::map<int, std::map<char, std::vector<int>>> dupCaseSubcase;
    for (auto const& [id, count] : dupCount) {
        std::cout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        logOutput << "Duplicate ID: " << id << " Count: " << count << std::endl;
        humanOutput << "Duplicate ID: " << id << " Count: " << count << std::endl;
        patternMatrix temp = patternMatrix(id);
        temp.determineSubCase();
        dupCaseSubcase[temp.caseMatch][temp.subCaseMatch].push_back(id);
    }
    
    for (auto const& [caseNum, subCaseMatch] : dupCaseSubcase) {
        for (auto const& [subCase, ids] : subCaseMatch) {
            std::cout << "Case: " << caseNum << " SubCase: " << subCase << " Count: " << ids.size() << std::endl;
            logOutput << "Case: " << caseNum << " SubCase: " << subCase << " Count: " << ids.size() << std::endl;
            humanOutput << "Case: " << caseNum << " SubCase: " << subCase << " Count: " << ids.size() << std::endl;
            for (auto id : ids) {
                std::cout << id << std::endl;
                logOutput << id << std::endl;
                humanOutput << id << std::endl;
            }
        }
    }
    logOutput.close();
    humanOutput.close();
    if (printDebug) {
        std::cout << "Done" << std::endl << std::endl;
    }
}

void silentRun(int pNum, std::vector<std::string> tGateOps) {
    runWithOptions(pNum, tGateOps, false, false, true, true, true);
}

void standardRun(int pNum, std::vector<std::string> tGateOps) {
    runWithOptions(pNum, tGateOps, true, false, true, true, true);
}

void fullDebugRun(int pNum, std::vector<std::string> tGateOps) {
    runWithOptions(pNum, tGateOps, true, true, true, true, true);
}

void allGateRunWithOptions(int pNum, bool printDebug, bool patternDebug, bool fullReduction, bool optimizedGenerate, bool o2Generate) {
    patternMatrix test = patternMatrix(pNum);
    std::cout << std::endl << "=============================================" << std::endl;
    if (test.findAllTGateOptions()) {
        std::cout << "Found T-Gate options for pattern " << pNum << std::endl;
        for (auto tGateOps : test.tGateOperationSets) {
            std::cout << "Running pattern " << pNum << " with T-Gate options: ";
            for (auto tGateOp : tGateOps) {
                std::cout << tGateOp << " ";
            }
            std::cout << std::endl << std::endl;
            runWithOptions(pNum, tGateOps, printDebug, patternDebug, fullReduction, optimizedGenerate, o2Generate);
        }
    } else {
        std::string fileNameBase = "/p" + std::to_string(pNum) + "-" + "no-t-gate-options-";
        std::string logFileName = USER_OUT_DIR + fileNameBase + "log.txt";
        std::string humanOutputFileName = USER_OUT_DIR + fileNameBase + "uniques.txt";
        std::filesystem::create_directory(USER_OUT_DIR);
        std::ofstream logOutput = std::ofstream(logFileName);
        std::ofstream humanOutput = std::ofstream(humanOutputFileName);
        if (!logOutput.is_open()) {
            std::cerr << "Error opening file:" << logFileName << std::endl;
            return;
        }
        if (!humanOutput.is_open()) {
            std::cerr << "Error opening file:" << humanOutputFileName << std::endl;
            return;
        }
        std::cout << "No T-Gate options found for pattern " << pNum << std::endl;
        logOutput << "No T-Gate options found for pattern " << pNum << std::endl;
        humanOutput << "No T-Gate options found for pattern " << pNum << std::endl;
        logOutput.close();
        humanOutput.close();
    }
    std::cout << "=============================================" << std::endl;
}

void silentAllGateRun(int pNum) {
    allGateRunWithOptions(pNum, false, false, true, true, true);
}

void standardAllGateRun(int pNum) {
    allGateRunWithOptions(pNum, true, false, true, true, true);
}

void allGateRunWithDebug(int pNum) {
    allGateRunWithOptions(pNum, true, true, true, true, true);
}