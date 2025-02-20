#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "possible-pattern-matrix.hpp"
#include "possible-pattern-deduper.hpp"
#include "zmatrix.hpp"

possiblePatternDeduper::possiblePatternDeduper() {
    loadPossiblePatterns();
}

void possiblePatternDeduper::loadPossiblePatterns() {
    // TODO - implement this after getting a set of possible patterns from pattern matrix operations
    return;
}

bool possiblePatternDeduper::isDuplicate(possiblePatternMatrix possiblePattern, int &duplicateID, bool addUniquePatterns) {
    duplicateID = 0;
    for (auto const& [id, ppm] : sumPatternMap[possiblePattern.pp.zSum]) {
        if (possiblePattern.isDuplicate(ppm)) {
                duplicateID = ppm.id;
                return true;
        }
    }
    if (addUniquePatterns) {
        sumPatternMap[possiblePattern.pp.zSum][++nextID] = possiblePattern;
    }
    return false;
}
