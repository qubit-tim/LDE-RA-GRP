#ifndef PATTERN_DEDUPER_HPP
#define PATTERN_DEDUPER_HPP

#include <map>
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

    private:
        void loadPatterns();
        // Case number -> Pattern Sum -> Pattern ID -> Pattern
        std::map <int, std::map <int, std::map <int, patternMatrix>>> caseSumPatternMap;
        int newPatternID = 1000;
};

#endif // PATTERN_DEDUPER_HPP