#ifndef PATTERN_DEDUPER_HPP
#define PATTERN_DEDUPER_HPP

#include <map>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "pattern-matrix.hpp"
#include "zmatrix.hpp"
#include "data/patterns928.hpp"

class patternDeduper {
    public:
        patternDeduper();
        bool isDuplicate(patternMatrix, int &, bool);
        int getUniqueCaseCount(int);

    private:
        void loadPatterns();
        // Case number -> Pattern Sum -> ID* -> Pattern
        //  * This ID is unique to the map and does not correspond to the pattern ID
        //      this is because pattern IDs might not be unique across sets
        std::unordered_map <int, std::unordered_map <int, std::unordered_map <int, patternMatrix>>> caseSumPatternMap;
        int nextID = 0;
};

#endif // PATTERN_DEDUPER_HPP