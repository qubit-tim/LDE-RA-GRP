#ifndef LDE_MATRIX_RUN_UTILS_HPP
#define LDE_MATRIX_RUN_UTILS_HPP

#include <vector>

bool validTGateOps(std::vector<std::string> tGateOps);
void runWithOptions(int pNum, std::vector<std::string> tGateOps, bool printDebug, bool patternDebug, bool fullReduction, bool optimizedGenerate);
void standardRun(int pNum, std::vector<std::string> tGateOps);
void fullDebugRun(int pNum, std::vector<std::string> tGateOps);
void standardAllGateRun(int pNum);
void allGateRunWithDebug(int pNum);

#endif // LDE_MATRIX_RUN_UTILS_HPP