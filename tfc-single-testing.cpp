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
#include <set>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"
#include "LDE-Matrix/data/patterns928.hpp"
#include "LDE-Matrix/pattern-deduper.hpp"
#include "LDE-Matrix/possible-pattern-deduper.hpp"
#include "LDE-Matrix/run-utils.hpp"

std::string TFC_OUT_DIR = "user-output";
bool useNewEncoding = false;

std::string OUT_DIR = "user-output";
std::string POUT_DIR = OUT_DIR + "/stop-at-possible-pattern";
std::regex RT_GATE_REGEX("(xT[1-6][1-6])");
std::regex LT_GATE_REGEX("(T[1-6][1-6]x)");

void bulkPossiblePatternAllGateRun(int startPattern, int step) {
    for (int i = startPattern; i <= 928; i+=step) {
        standardPossiblePatternAllGateRun(i);
    }
}

void dedupPossiblePatternAllGatesRun(bool possiblePatternOnly, bool onlyInvalids) {
    possiblePatternDeduper ppd;
    int pPatternCount = 0;
    std::filesystem::create_directory(OUT_DIR);
    std::ofstream ppLogOutput = std::ofstream(OUT_DIR + "/possible-patterns-log.txt");
    if (!ppLogOutput.is_open()) {
        std::cerr << "Error opening file:" << OUT_DIR + "/possible-patterns-log.txt" << std::endl;
        return;
    }

    std::ofstream ppHumanOutput = std::ofstream(OUT_DIR + "/possible-patterns.txt");
    if (!ppHumanOutput.is_open()) {
        std::cerr << "Error opening file:" << OUT_DIR + "/possible-patterns.txt" << std::endl;
        return;
    }

    for (int i = 1; i <= 928; i++) {
        patternMatrix getGates = patternMatrix(i);
        if (getGates.findAllTGateOptions()) {
            std::cout << "Found T-Gate options for pattern " << i << std::endl;
            for (auto tGateOps : getGates.tGateOperationSets) {
                patternMatrix test = patternMatrix(i);
                test.singleValidPattern = onlyInvalids;
                std::cout << "Running pattern " << i << " with T-Gate options: ";
                for (auto tGateOp : tGateOps) {
                    std::cout << tGateOp << " ";
                }
                std::cout << std::endl << std::endl;
                std::string tGateOpsString = "";
                for (auto tGateOp : tGateOps) {
                    tGateOpsString += tGateOp + "-";
                }
                if(tGateOpsString.size() > 0) {
                    tGateOpsString.pop_back();
                }
                std::string patternIDandTGateOps = "p" + std::to_string(i) + "-" + tGateOpsString;
                std::string fileNameBase = "/" + patternIDandTGateOps + "-";
                std::string runInfo = "possible-values-run";
                std::string logFileName = POUT_DIR + fileNameBase + "log-" + runInfo + ".txt";
                std::string humanOutputFileName = POUT_DIR + fileNameBase + runInfo + ".txt";
                std::filesystem::create_directory(POUT_DIR);
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
                test.multilineOutput = true;
                logOutput << "Pattern Number: " << i << std::endl;
                logOutput << "Before T-Gate multiplication:" << std::endl;
                logOutput << test << std::endl;
                humanOutput << "Pattern Number: " << i << std::endl;
                humanOutput << "Before T-Gate multiplication:" << std::endl;
                humanOutput << test << std::endl;
                
                std::cout << "Attempting tGate operations on pattern " << i << std::endl;
                for (auto tGateOp : tGateOps) {
                    if (std::regex_match(tGateOp, RT_GATE_REGEX)) {
                        std::string c1(1, tGateOp[2]);
                        std::string c2(1, tGateOp[3]);
                        test.rightTGateMultiply(std::stoi(c1), std::stoi(c2));
                    }
                    if (std::regex_match(tGateOp, LT_GATE_REGEX)) {
                        std::string r1(1, tGateOp[1]);
                        std::string r2(1, tGateOp[2]);
                        test.leftTGateMultiply(std::stoi(r1), std::stoi(r2));
                    }
                }
                std::cout << "Completed tGate operations on pattern " << i << std::endl;

                logOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
                logOutput << test << std::endl;
                humanOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
                humanOutput << test << std::endl;

                logOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << std::endl;
                humanOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << std::endl;
                std::cout << "Doing LDE reductions on pattern " << i << std::endl;
                test.doLDEReduction();
                std::cout << "Completed LDE reductions on pattern " << i << std::endl;
                logOutput << std::endl;
                
                logOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << "Possible values:" << std::endl;
                test.printPossibleValues(logOutput);
                humanOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << "Possible values:" << std::endl;
                test.printPossibleValues(humanOutput);
                test.multilineOutput = false;
                std::ostringstream ppmString;
                test.printPossibleValues(ppmString);  // A bit of a hack to get the string but works
                std::cout << "Creating possible pattern matrix from " << ppmString.str() << std::endl;
                std::cout << "New encoding: " << !test.printOldEncoding << std::endl;
                pPatternCount++;
                possiblePatternMatrix ppmTest = possiblePatternMatrix(pPatternCount, ppmString.str(), !test.printOldEncoding);
                ppmTest.origins.push_back(patternIDandTGateOps);
                ppmTest.printID = true;
                ppmTest.printOrigins = true;
                ppmTest.printLeadsToPatternIDs = true;
                std::cout << "Possible Pattern Matrix Created: " << ppmTest << std::endl;
                int duplicateID = -1;
                std::cout << "Checking for possible pattern duplicates on " << ppmTest << std::endl;
                ppd.isDuplicate(ppmTest,duplicateID,true);
                if (duplicateID != 0) {
                    ppLogOutput << "Duplicate possible pattern found.  Original ID: " << duplicateID << "; Duplicate: " << ppmTest << std::endl;
                } else {
                    ppLogOutput << "New possible pattern found: " << ppmTest << std::endl;
                }
                std::cout << "Duplicate check done, going to the next pattern and tGate combo." << std::endl;
            }
        }
    }
    std::cout << "Done with all patterns and tGate combinations." << std::endl;
    std::cout << "Printing out all unique possible patterns." << std::endl;
    std::cout << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    ppLogOutput << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    ppHumanOutput << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    std::cout << "Unique possible patterns: " << std::endl;
    ppLogOutput << "Unique possible patterns: " << std::endl;
    ppHumanOutput << "Unique possible patterns: " << std::endl;
    for (auto & ppm : ppd.getUniquePossiblePatterns()) {
        ppm.printID = true;
        std::cout << ppm << std::endl;
        ppLogOutput << ppm << std::endl;
        ppHumanOutput << ppm << std::endl;
    }
    std::vector<possiblePatternMatrix> uniquePossiblePatterns = ppd.getUniquePossiblePatterns();
    if (!possiblePatternOnly) {
        std::cout << "Attempting to generate all possible patterns." << std::endl;
        patternDeduper pd = patternDeduper();
        int newPatternID = 1000;
        for (auto & ppm : uniquePossiblePatterns) {
            std::cout << "Working on Unique Possible Pattern: " << ppm << std::endl;
            patternMatrix pm = patternMatrix();
            pm.singleValidPattern = onlyInvalids;
            pm.possibleValues = ppm.getPossibleValues();
            if (pm.possibleValuesLeadToAllPatterns()) {
                std::cout << ppm << " leads to all patterns" << std::endl;
                ppLogOutput << ppm << " leads to all patterns" << std::endl;
                for (int i = 1; i <= 928; i++) {
                    ppm.leadsToPatternIDs[i] = true;
                }
                continue;   
            }
            std::cout << "Generating all possible value patterns for " << ppm << std::endl;
            pm.opt2GenerateAllPossibleValuePatterns();
            if (pm.allPossibleValuePatterns.size() == 0) {
                std::cout << ppm << " leads to no valid patterns" << std::endl;
                ppLogOutput << ppm << " leads to no valid patterns" << std::endl;
                ppm.leadsToPatternIDs[-1] = true;
                continue;
            }
            for (auto pm : pm.allPossibleValuePatterns) {
                int duplicateID = -1;
                // By default, these are in the old encoding but this could change :(
                patternMatrix pmCopy = patternMatrix(++newPatternID, pm.first);
                if (pd.isDuplicate(pmCopy, duplicateID, true)) {
                    std::cout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
                    ppLogOutput << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
                    ppm.leadsToPatternIDs[duplicateID] = true;
                } else { // This shouldn't happen and if it does, we need to stop
                    std::cout << pmCopy.id << " is a unique pattern, aborting!" << std::endl;
                    ppLogOutput << pmCopy.id << " is a unique pattern, aborting!" << std::endl;
                    ppHumanOutput << pmCopy << " is a unique pattern, aborting!" << std::endl;
                    return;
                }
            }
            std::cout << "Done with " << ppm << std::endl << std::endl;
        }
    }
    std::cout << "Printing out all unique possible patterns." << std::endl;
    std::cout << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    ppLogOutput << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    ppHumanOutput << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    std::cout << "Unique possible patterns: " << std::endl;
    ppLogOutput << "Unique possible patterns: " << std::endl;
    ppHumanOutput << "Unique possible patterns: " << std::endl;
    for (auto & ppm : uniquePossiblePatterns) {
        ppm.printID = false;
        std::cout << ppm << std::endl;
        ppLogOutput << ppm << std::endl;
        ppHumanOutput << ppm << std::endl;
    }
    
    ppHumanOutput.close();
    ppLogOutput.close();
}

void dedupPossiblePatternAllGatesRunByCase(int caseNumber) {
    if (caseNumber < 1 || caseNumber > 8) {
        std::cerr << "Invalid case number" << std::endl;
        return;
    }
    possiblePatternDeduper ppd;
    int pPatternCount = 0;
    std::filesystem::create_directory(OUT_DIR);
    std::string logFileName = "possible-patterns-log-case" + std::to_string(caseNumber) + ".txt";
    std::string humanOutputFileName = "possible-patterns-case" + std::to_string(caseNumber) + ".txt";
    std::ofstream ppLogOutput = std::ofstream(OUT_DIR + "/" + logFileName);
    if (!ppLogOutput.is_open()) {
        std::cerr << "Error opening file:" << OUT_DIR + "/" + logFileName << std::endl;
        return;
    }

    std::ofstream ppHumanOutput = std::ofstream(OUT_DIR + "/" + humanOutputFileName);
    if (!ppHumanOutput.is_open()) {
        std::cerr << "Error opening file:" << OUT_DIR + "/" + humanOutputFileName << std::endl;
        return;
    }
    for (const auto& pair : CASE_PATTERN_MAP_PATTERNS_928[caseNumber]) {
        int patternID = pair.first;
        if (!PATTERN_928_VALIDITY[patternID]) {
            std::cout << "Pattern " << patternID << " is not valid, skipping." << std::endl;
            ppLogOutput << "Pattern " << patternID << " is not valid, skipping." << std::endl;
            ppHumanOutput << "Pattern " << patternID << " is not valid, skipping." << std::endl;
            continue;
        }
        patternMatrix getGates = patternMatrix(patternID);
        if (getGates.findAllTGateOptions()) {
            std::cout << "Found T-Gate options for pattern " << patternID << std::endl;
            for (auto tGateOps : getGates.tGateOperationSets) {
                patternMatrix test = patternMatrix(patternID);
                std::cout << "Running pattern " << patternID << " with T-Gate options: ";
                for (auto tGateOp : tGateOps) {
                    std::cout << tGateOp << " ";
                }
                std::cout << std::endl << std::endl;
                std::string tGateOpsString = "";
                for (auto tGateOp : tGateOps) {
                    tGateOpsString += tGateOp + "-";
                }
                if(tGateOpsString.size() > 0) {
                    tGateOpsString.pop_back();
                }
                std::string patternIDandTGateOps = "p" + std::to_string(patternID) + "-" + tGateOpsString;
                std::string fileNameBase = "/" + patternIDandTGateOps + "-";
                std::string runInfo = "possible-values-run";
                std::string logFileName = POUT_DIR + fileNameBase + "log-" + runInfo + ".txt";
                std::string humanOutputFileName = POUT_DIR + fileNameBase + runInfo + ".txt";
                std::filesystem::create_directory(POUT_DIR);
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
                test.multilineOutput = true;
                logOutput << "Pattern Number: " << patternID << std::endl;
                logOutput << "Before T-Gate multiplication:" << std::endl;
                logOutput << test << std::endl;
                humanOutput << "Pattern Number: " << patternID << std::endl;
                humanOutput << "Before T-Gate multiplication:" << std::endl;
                humanOutput << test << std::endl;
                
                std::cout << "Attempting tGate operations on pattern " << patternID << std::endl;
                for (auto tGateOp : tGateOps) {
                    if (std::regex_match(tGateOp, RT_GATE_REGEX)) {
                        std::string c1(1, tGateOp[2]);
                        std::string c2(1, tGateOp[3]);
                        test.rightTGateMultiply(std::stoi(c1), std::stoi(c2));
                    }
                    if (std::regex_match(tGateOp, LT_GATE_REGEX)) {
                        std::string r1(1, tGateOp[1]);
                        std::string r2(1, tGateOp[2]);
                        test.leftTGateMultiply(std::stoi(r1), std::stoi(r2));
                    }
                }
                std::cout << "Completed tGate operations on pattern " << patternID << std::endl;

                logOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
                logOutput << test << std::endl;
                humanOutput << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
                humanOutput << test << std::endl;

                logOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << std::endl;
                humanOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << std::endl;
                std::cout << "Doing LDE reductions on pattern " << patternID << std::endl;
                test.doLDEReduction();
                std::cout << "Completed LDE reductions on pattern " << patternID << std::endl;
                logOutput << std::endl;
                
                logOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << "Possible values:" << std::endl;
                test.printPossibleValues(logOutput);
                humanOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << "Possible values:" << std::endl;
                test.printPossibleValues(humanOutput);
                test.multilineOutput = false;
                std::ostringstream ppmString;
                test.printPossibleValues(ppmString);  // A bit of a hack to get the string but works
                std::cout << "Creating possible pattern matrix from " << ppmString.str() << std::endl;
                std::cout << "New encoding: " << !test.printOldEncoding << std::endl;
                pPatternCount++;
                possiblePatternMatrix ppmTest = possiblePatternMatrix(pPatternCount, ppmString.str(), !test.printOldEncoding);
                ppmTest.origins.push_back(patternIDandTGateOps);
                ppmTest.patternOrigins.insert(patternID);
                //std::sort(ppmTest.patternOrigins.begin(), ppmTest.patternOrigins.end());
                ppmTest.printID = true;
                ppmTest.printOrigins = true;
                ppmTest.printLeadsToPatternIDs = true;
                std::cout << "Possible Pattern Matrix Created: " << ppmTest << std::endl;
                int duplicateID = -1;
                std::cout << "Checking for possible pattern duplicates on " << ppmTest << std::endl;
                ppd.isDuplicate(ppmTest,duplicateID,true);
                if (duplicateID != 0) {
                    ppLogOutput << "Duplicate possible pattern found.  Original ID: " << duplicateID << "; Duplicate: " << ppmTest << std::endl;
                } else {
                    ppLogOutput << "New possible pattern found: " << ppmTest << std::endl;
                }
                std::cout << "Duplicate check done, going to the next pattern and tGate combo." << std::endl;
            }
        }
    }
    std::cout << "Done with all patterns and tGate combinations." << std::endl;
    std::cout << "Printing out all unique possible patterns." << std::endl;
    std::cout << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    ppLogOutput << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    ppHumanOutput << "Unique possible patterns count: " << ppd.getUniquePossiblePatterns().size() << std::endl;
    std::cout << "Unique possible patterns: " << std::endl;
    ppLogOutput << "Unique possible patterns: " << std::endl;
    ppHumanOutput << "Unique possible patterns: " << std::endl;
    for (auto & ppm : ppd.getUniquePossiblePatterns()) {
        ppm.printID = true;
        std::cout << ppm << std::endl;
        ppLogOutput << ppm << std::endl;
        ppHumanOutput << ppm << std::endl;
    }
    std::vector<possiblePatternMatrix> uniquePossiblePatterns = ppd.getUniquePossiblePatterns();
    std::cout << "Attempting to generate all possible patterns." << std::endl;
    patternDeduper pd = patternDeduper();
    int newPatternID = 1000;
    for (auto & ppm : uniquePossiblePatterns) {
        std::cout << "Working on Unique Possible Pattern: " << ppm << std::endl;
        patternMatrix pm = patternMatrix();
        pm.singleValidPattern = true;
        pm.possibleValues = ppm.getPossibleValues();
        if (pm.possibleValuesLeadToAllPatterns()) {
            std::cout << ppm << " leads to all patterns" << std::endl;
            ppLogOutput << ppm << " leads to all patterns" << std::endl;
            for (int i = 1; i <= 928; i++) {
                ppm.leadsToPatternIDs[i] = true;
            }
            continue;   
        }
        std::cout << "Generating all possible value patterns for " << ppm << std::endl;
        pm.singleValidPattern = true;
        pm.opt2GenerateAllPossibleValuePatterns();
        if (pm.allPossibleValuePatterns.size() == 0) {
            std::cout << ppm << " leads to no valid patterns" << std::endl;
            ppLogOutput << ppm << " leads to no valid patterns" << std::endl;
            ppm.leadsToPatternIDs[-1] = true;
            continue;
        }
        for (auto pm : pm.allPossibleValuePatterns) {
            int duplicateID = -1;
            // By default, these are in the old encoding but this could change :(
            patternMatrix pmCopy = patternMatrix(++newPatternID, pm.first);
            if (pd.isDuplicate(pmCopy, duplicateID, true)) {
                std::cout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
                ppLogOutput << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
                ppm.leadsToPatternIDs[duplicateID] = true;
            } else { // This shouldn't happen and if it does, we need to stop
                std::cout << pmCopy.id << " is a unique pattern, aborting!" << std::endl;
                ppLogOutput << pmCopy.id << " is a unique pattern, aborting!" << std::endl;
                ppHumanOutput << pmCopy << " is a unique pattern, aborting!" << std::endl;
                return;
            }
        }
        std::cout << "Done with " << ppm << std::endl << std::endl;
        ppm.printID = false;
        ppHumanOutput << "Done with " << ppm << std::endl;
    }
    std::cout << "Printing out all unique possible patterns." << std::endl;
    std::cout << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    ppLogOutput << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    ppHumanOutput << "Unique possible patterns count: " << uniquePossiblePatterns.size() << std::endl;
    std::cout << "Unique possible patterns: " << std::endl;
    ppLogOutput << "Unique possible patterns: " << std::endl;
    ppHumanOutput << "Unique possible patterns: " << std::endl;
    for (auto & ppm : uniquePossiblePatterns) {
        ppm.printID = false;
        std::cout << ppm << std::endl;
        ppLogOutput << ppm << std::endl;
        ppHumanOutput << ppm << std::endl;
    }
    ppHumanOutput.close();
    ppLogOutput.close();
}

int main(int argc, char **argv) {
    // Create subcase groupings of patterns based on both the valid patterns and invalid patterns
    //   An example is of case 2 in the overleaf document where c2 g1 -> all c2

    // Next up would be to create a graph of patterns -> possible pattern -> patterns


    /*
    // TODOs:
    //  [{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2},{0,2}][{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2},{1,3}][{0,2,1,3},{0,2,1,3},{0,2,1,3},{0,2,1,3},{1,3},{0,2}] Origins: p578-xT12-xT34  Leads to no valid patterns
    //  [{0,2},{0,2},{0,2},{0,2},{0},{0}][{1,3},{1,3},{1,3},{1,3},{0},{0}][{0,2},{0,2},{0,2},{0,2},{2},{2}][{1,3},{1,3},{1,3},{1,3},{2},{2}][{0,2},{0,2},{0,2},{0,2},{0},{0}][{0,2},{0,2},{0,2},{0,2},{0},{0}] Origins: p63-xT13-xT24, p63-xT14-xT23, p64-xT13-xT24, p64-xT14-xT23, p65-xT13-xT24, p65-xT14-xT23, p81-xT12-xT34, p81-xT13-xT24, p82-xT12-xT34, p82-xT13-xT24, p83-xT12-xT34, p83-xT13-xT24, p314-xT12-xT34, p315-xT12-xT34, p316-xT12-xT34, p316-T12x-T34x, p317-xT12-xT34, p317-T12x-T34x, p334-xT12-xT34, p335-xT12-xT34, p410-T13x-T24x, p410-T14x-T23x, p413-T13x-T24x, p413-T14x-T23x, p415-T12x-T34x, p415-T13x-T24x, p416-T12x-T34x, p416-T13x-T24x, p418-T13x-T24x, p418-T14x-T23x, p420-T12x-T34x, p420-T13x-T24x  Leads to no valid patterns 
    //  p564-T12x-T34x
    std::string logFileName = "p593-debug-log.txt";

    std::ofstream ppLogOutput = std::ofstream(OUT_DIR + "/" + logFileName);
    if (!ppLogOutput.is_open()) {
        std::cerr << "Error opening file:" << OUT_DIR + "/" + logFileName << std::endl;
        return 0;
    }
    patternMatrix pm = patternMatrix(593);
    std::cout << pm << std::endl;
    pm.printID = true;
    pm.printDebugInfo = true;
    pm.printOldEncoding = false;
    pm.debugOutput = &ppLogOutput;
    //p593-T12x-T34x
    pm.leftTGateMultiply(1, 2);
    pm.leftTGateMultiply(3, 4);
    pm.doLDEReduction();
    pm.printPossibleValues(ppLogOutput);
    pm.printPossibleValues(std::cout);
    std::cout << pm << std::endl;
    std::cout << pm.getMaxOfPossibleValues() << std::endl;
    pm.printLDEs(std::cout);
    std::cout << "generating all possible values" << std::endl;
    pm.opt2GenerateAllPossibleValuePatterns();
    std::cout << "done generating all possible values" << std::endl;
    ppLogOutput.close();
    return 0;
    */
    std::vector<std::future<void>> futures;
    for (int i = 1; i <= 8; i++) {
        std::cout << "Running case " << i << std::endl;
        auto res = std::async(std::launch::async, dedupPossiblePatternAllGatesRunByCase, i);
        futures.push_back(std::move(res));
    }
    for (auto &f : futures) {
        f.wait();
    }
    /*
    for (int i=1; i <= 8; i++) {
        std::string caseString = "Case: " + std::to_string(i);
        std::cout << caseString << std::endl;
        dedupPossiblePatternAllGatesRunByCase(i);
    }
    */
    //dedupPossiblePatternAllGatesRun(false, true);
    // bulkPossiblePatternAllGateRun(1, 1);
    /*
    group 1: (lead to entire case 2) 3, 4, 5, 6
    group 2: (lead to entire case 5a+b) 7,8,9,10,11,12
    group 3: (lead to entire case 4) 19,20,24,115,116,117
    group 5: (lead to entire case 3) 93,94,188,189
    group 6: (lead to entire case 6) 95,96,98,365,366,368
    group 7: (lead to entire case 6) 48,49,50,55,72,73
    group 8: (lead to entire case 8) 190,191,192,194,197,198
    */
    return 0;
}