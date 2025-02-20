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
#include "LDE-Matrix/possible-pattern-deduper.hpp"
#include "LDE-Matrix/run-utils.hpp"

std::string TFC_OUT_DIR = "user-output";
bool useNewEncoding = false;

std::string USER_OUT_DIR = "user-output";
std::regex R_T_GATE_REGEX("(xT[1-6][1-6])");
std::regex L_T_GATE_REGEX("(T[1-6][1-6]x)");

void bulkPossiblePatternAllGateRun(int startPattern, int step) {
    for (int i = startPattern; i <= 928; i+=step) {
        standardPossiblePatternAllGateRun(i);
    }
}

void dedupPossiblePatternAllGatesRun() {
    possiblePatternDeduper ppd;
    int pPatternCount = 0;
    for (int i = 1; i <= 928; i++) {
        patternMatrix test = patternMatrix(i);
        if (test.findAllTGateOptions()) {
            std::cout << "Found T-Gate options for pattern " << i << std::endl;
            for (auto tGateOps : test.tGateOperationSets) {
                std::cout << "Running pattern " << i << " with T-Gate options: ";
                for (auto tGateOp : tGateOps) {
                    std::cout << tGateOp << " ";
                }
                std::cout << std::endl << std::endl;
                std::string tGateOpsString = "";
                for (auto tGateOp : tGateOps) {
                    tGateOpsString += tGateOp + "-";
                }

                std::string fileNameBase = "/p" + std::to_string(i) + "-" + tGateOpsString;
                std::string runInfo = "possible-values-run";
                std::string logFileName = USER_OUT_DIR + fileNameBase + "log-" + runInfo + ".txt";
                std::string humanOutputFileName = USER_OUT_DIR + fileNameBase + runInfo + ".txt";
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
                test.multilineOutput = true;
                logOutput << "Pattern Number: " << i << std::endl;
                logOutput << "Before T-Gate multiplication:" << std::endl;
                logOutput << test << std::endl;
                humanOutput << "Pattern Number: " << i << std::endl;
                humanOutput << "Before T-Gate multiplication:" << std::endl;
                humanOutput << test << std::endl;

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

                logOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << std::endl;
                humanOutput << "LDEs Before Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << std::endl;
                test.doLDEReduction();
                logOutput << std::endl;
                
                logOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(logOutput);
                logOutput << "Possible values:" << std::endl;
                test.printPossibleValues(logOutput);
                humanOutput << "LDEs After Reduction(s):" << std::endl;
                test.printLDEs(humanOutput);
                humanOutput << "Possible values:" << std::endl;
                test.printPossibleValues(humanOutput);
                possiblePatternMatrix ppmTest = possiblePatternMatrix(++pPatternCount, std::string matrix, !test.printOldEncoding);
            }
        }
    }
}

int main(int argc, char **argv) {
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