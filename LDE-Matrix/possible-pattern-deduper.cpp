#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

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
    for (auto & [id, ppm] : sumPatternMap[possiblePattern.pp.zSum]) {
        if (possiblePattern.isDuplicate(ppm)) {
                duplicateID = ppm.id;
                for (auto const& origin : possiblePattern.origins) {
                    ppm.origins.push_back(origin);
                }
                for (auto const& patternOrigin : possiblePattern.patternOrigins) {
                    ppm.patternOrigins.insert(patternOrigin);
                }
                return true;
        }
    }
    if (addUniquePatterns) {
        sumPatternMap[possiblePattern.pp.zSum][++nextID] = possiblePattern;
    }
    return false;
}

std::vector<possiblePatternMatrix> possiblePatternDeduper::getUniquePossiblePatterns() {
    std::vector<possiblePatternMatrix> uniquePatterns;
    for (auto const& [sum, idMap] : sumPatternMap) {
        for (auto const& [id, ppm] : idMap) {
            uniquePatterns.push_back(ppm);
        }
    }
    return uniquePatterns;
}

void possiblePatternDeduper::printUniquePossiblePatterns(std::ostream& os) {
    for (auto & [sum, idMap] : sumPatternMap) {
        for (auto & [id, ppm] : idMap) {
            ppm.printID = false;
            os << ppm << std::endl;
        }
    }
}
