#ifndef LDE_MATRIX_RUN_UTILS_HPP
#define LDE_MATRIX_RUN_UTILS_HPP

#include <vector>
#include <string>

bool validTGateOps(std::vector<std::string> tGateOps);
void runWithOptions(int pNum, std::vector<std::string> tGateOps, bool printDebug, bool patternDebug, bool fullReduction, bool optimizedGenerate, bool o2Generate, bool onlyPossiblePatterns);
void silentRun(int pNum, std::vector<std::string> tGateOps);
void standardRun(int pNum, std::vector<std::string> tGateOps);
void fullDebugRun(int pNum, std::vector<std::string> tGateOps);
void allGateRunWithOptions(int pNum, bool printDebug, bool patternDebug, bool fullReduction, bool optimizedGenerate, bool o2Generate);
void silentAllGateRun(int pNum);
void standardAllGateRun(int pNum);
void allGateRunWithDebug(int pNum);
void silentPossiblePatternAllGateRun(int pNum);
void standardPossiblePatternAllGateRun(int pNum);
void debugPossiblePatternAllGateRun(int pNum);

#endif // LDE_MATRIX_RUN_UTILS_HPP