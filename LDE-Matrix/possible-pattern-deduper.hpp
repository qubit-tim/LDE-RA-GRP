#ifndef POSSIBLE_PATTERN_DEDUPER_HPP
#define POSSIBLE_PATTERN_DEDUPER_HPP

#include <map>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "possible-pattern-matrix.hpp"
#include "zmatrix.hpp"

class possiblePatternDeduper {
    public:
        possiblePatternDeduper();
        bool isDuplicate(possiblePatternMatrix, int &, bool);
        std::vector<possiblePatternMatrix> getUniquePossiblePatterns();
        void printUniquePossiblePatterns(std::ostream& os);

    private:
        void loadPossiblePatterns();
        // Pattern Sum -> ID* -> Pattern
        //  * This ID is unique to the map and does not correspond to the pattern ID
        //      this is because pattern IDs might not be unique across sets
        std::unordered_map <int, std::unordered_map <int, possiblePatternMatrix>> sumPatternMap;
        int nextID = 0;
};

#endif // POSSIBLE_PATTERN_DEDUPER_HPP