#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "pattern-matrix.hpp"
#include "pattern-deduper.hpp"
#include "zmatrix.hpp"
#include "data/patterns928.hpp"

patternDeduper::patternDeduper() {
    loadPatterns();
}

void patternDeduper::loadPatterns() {
    for (auto const& [caseNumber, sumMap] : CASE_SUM_MAP_PATTERNS_928) {
        for (auto const& [sum, idMap] : sumMap) {
            for (auto const& [id, pattern] : idMap) {
                caseSumPatternMap[caseNumber][sum][++nextID] = patternMatrix(id, pattern);
            }
        }
    }
}

bool patternDeduper::isDuplicate(patternMatrix pattern, int &duplicateID, bool addUniquePatterns) {
    pattern.matchOnCases();
    //std::cout << "Pattern " << pattern.id << " matches case " << pattern.caseMatch << std::endl;
    for (auto const& [id, pm] : caseSumPatternMap[pattern.caseMatch][pattern.p.zSum]) {
        //std::cout << "Comparing to pattern " << id << std::endl;
        if (pattern.isDuplicate(pm)) {
                duplicateID = pm.id;
                return true;
        }
    }
    if (addUniquePatterns) {
        caseSumPatternMap[pattern.caseMatch][pattern.p.zSum][++nextID] = pattern;
    }
    return false;
}