#include "pattern-matrix.hpp"
#include "zmatrix.hpp"
#include "test-utils.hpp"

#include <gtest/gtest.h>

std::string TOO_FEW_ROWS = "[0 0,0 0,0 0,0 0,0 0,0 0]";
std::string TOO_FEW_COLUMNS = "[0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0]";
std::string VALID_BINARY_PATTERN = "[0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 1,0 1,0 1,0 1,0 1,0 1][1 0,1 0,1 0,1 0,1 0,1 0][1 1,1 1,1 1,1 1,1 1,1 1]";
std::string VALID_BINARY_PATTERN_IN_NUMERICAL_FORM = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,1,1,1,1][2,2,2,2,2,2][3,3,3,3,3,3]";
std::string VALID_NUMERICAL_PATTERN = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,1,1,1,1][2,2,2,2,2,2][3,3,3,3,3,3]";
std::string ALL_ZEROS_PATTERN = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";


// These are a base test set which are guaranteed to be valid
std::map <int, std::vector<std::string>> CASE_TO_VALID_PATTERN_MAP = {
    {
        -1,  // The case where the pattern is not a valid pattern
        {
            "[0,0,0,0,0,2][0,0,0,0,2,0][0,0,0,2,0,0][0,0,2,0,0,0][0,2,0,0,0,0][2,0,0,0,0,0]",
            "[2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,1,1,1,1,1][3,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1]",
        }
    },
    {
        0,
        {
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1]",
        }
    },
    {
        1, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0]",
        }
    },
    {
        2, // "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,0,0][3,2,0,0,0,0][2,2,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,3,0,0,3][0,0,0,0,0,0][0,0,2,0,0,2][0,0,0,0,0,0][0,0,3,0,0,2][0,0,2,0,0,3]",
            // transpose cases
            "[2,2,2,2,0,0][2,2,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0][3,0,2,0,3,2][0,0,0,0,0,0]",
        }
    },
    {
        3, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,2,2,0,0][2,2,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,2,3,0,0][2,2,2,2,0,0][3,3,3,3,0,0][3,2,3,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,3,3,0,0][3,3,3,3,0,0][3,3,3,3,0,0][3,3,3,3,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,0,3,2,0,3][0,0,0,0,0,0][0,0,0,0,0,0][3,0,3,2,0,2][2,0,2,2,0,2][3,0,3,3,0,3]",
            "[2,3,0,0,2,3][0,0,0,0,0,0][3,3,0,0,2,2][0,0,0,0,0,0][2,2,0,0,2,2][3,3,0,0,3,3]",
        }
    },
    {
        4,  // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,2,3,0,0][3,2,2,3,0,0][2,2,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,3,3,0,0][3,3,3,3,0,0][3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,2,2][3,2,0,0,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][2,2,0,0,3,2]",
            "[2,0,3,0,3,2][2,0,3,0,0,0][0,0,0,0,0,0][2,0,3,0,0,0][0,0,0,0,0,0][3,0,2,0,2,2]",
        }
    },
    {
        5, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][0,0,3,3,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[2,3,0,0,0,0][0,0,3,2,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,2,3,0,0]",
            "[0,0,0,0,2,3][0,0,0,0,0,0][0,0,0,0,3,2][3,2,0,0,0,0][2,3,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        6, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,2,2][2,2,0,0,2,2][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,3,3,3,0,0][3,3,3,3,0,0][3,3,0,0,3,3][3,3,0,0,3,3][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[3,2,3,0,0,3][2,3,0,3,3,0][0,0,0,0,0,0][3,2,0,3,3,0][0,0,0,0,0,0][3,2,3,0,0,3]",
        }
    },
    {
        7, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"
        {
            "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,2,2][0,0,0,0,2,2]",
            "[3,3,0,0,0,0][3,3,0,0,0,0][0,0,3,3,0,0][0,0,3,3,0,0][0,0,0,0,3,3][0,0,0,0,3,3]",
            "[2,3,0,0,0,0][0,0,0,0,3,2][0,0,3,3,0,0][3,3,0,0,0,0][0,0,2,3,0,0][0,0,0,0,3,2]",
        }
    },
    {
        8, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"
        {
            "[2,2,2,2,0,0][2,2,2,2,0,0][2,2,0,0,2,2][2,2,0,0,2,2][0,0,2,2,2,2][0,0,2,2,2,2]",
            "[3,3,3,3,0,0][3,3,3,3,0,0][3,3,0,0,3,3][3,3,0,0,3,3][0,0,3,3,3,3][0,0,3,3,3,3]",
            "[3,3,0,0,3,2][3,2,3,3,0,0][0,0,2,3,3,3][3,2,3,2,0,0][0,0,3,3,2,3][2,3,0,0,3,3]",
            "[3,3,0,0,3,2][0,0,2,3,3,3][2,3,0,0,3,3][3,2,3,2,0,0][0,0,3,3,2,3][3,2,3,3,0,0]",
        }
    },
};

// TODO - Rearrangement Refactor + Test Overhaul
// These are a limited set of the patterns which are guaranteed to be valid
std::map <int, std::vector<std::string>> REARRANGE_CASES_SHORT = {
    {
        1, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,3,3,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0][0,2,0,0,2,0][0,0,0,0,0,0]",
        }
    },
    {
        2, // "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[0,0,3,0,0,3][0,0,0,0,0,0][0,0,2,0,0,2][0,0,0,0,0,0][0,0,3,0,0,2][0,0,2,0,0,3]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0][3,0,2,0,3,2][0,0,0,0,0,0]",
        }
    },
    {
        3, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,0,3,2,0,3][0,0,0,0,0,0][0,0,0,0,0,0][3,0,3,2,0,2][2,0,2,2,0,2][3,0,3,3,0,3]",
            "[2,3,0,0,2,3][0,0,0,0,0,0][3,3,0,0,2,2][0,0,0,0,0,0][2,2,0,0,2,2][3,3,0,0,3,3]",
        }
    },
    {
        4,  // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,3,0,0,2,2][3,2,0,0,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][2,2,0,0,3,2]",
            "[2,0,3,0,3,2][2,0,3,0,0,0][0,0,0,0,0,0][2,0,3,0,0,0][0,0,0,0,0,0][3,0,2,0,2,2]",
        }
    },
    {
        5, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[2,3,0,0,0,0][0,0,3,2,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,2,3,0,0]",
            "[0,0,0,0,2,3][0,0,0,0,0,0][0,0,0,0,3,2][3,2,0,0,0,0][2,3,0,0,0,0][0,0,0,0,0,0]",
        }
    },
    {
        6, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"
        {
            "[3,2,3,0,0,3][2,3,0,3,3,0][0,0,0,0,0,0][3,2,0,3,3,0][0,0,0,0,0,0][3,2,3,0,0,3]",
        }
    },
    // TODO - Rearrangement Refactor + Test Overhaul
    /* Case 7 and Case 8 take a long time to run, like 5+ minutes for both of them with only 1 case each
       Because of this, they are removed from this short list and are included in the full rearrange test.
    {
        // This test case can take a very long time to complete
        7, // "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"
        {
            "[2,3,0,0,0,0][0,0,0,0,3,2][0,0,3,3,0,0][3,3,0,0,0,0][0,0,2,3,0,0][0,0,0,0,3,2]",
        }
    },
    {
        // This test case can take a very long time to complete
        8, // "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"
        {
            "[3,3,0,0,3,2][0,0,2,3,3,3][2,3,0,0,3,3][3,2,3,2,0,0][0,0,3,3,2,3][3,2,3,3,0,0]",
        }
    },
    */
};

// These are a base test set which are guaranteed to be valid
std::map <bool, std::vector<std::string>> ORTHONORMAL_PATTERN_CHECKS = {
    {
        true,  // The case where the pattern is orthonormal
        {
            // These were taken from pattern785 case matches
            // Case 1
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,1,1][0,0,0,1,2,2][0,0,0,1,2,2]",
            "[0,0,0,0,1,1][0,0,0,0,1,1][0,1,2,2,1,1][0,1,2,2,1,1][1,1,0,0,1,1][1,1,1,1,1,1]",
            // Case 2
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,1,1,1,1][0,0,2,2,2,2][0,0,2,2,2,2]",
            "[0,0,2,2,2,2][0,0,2,2,2,2][1,1,0,0,0,0][1,1,1,1,1,1][1,1,1,1,1,1][1,1,1,1,1,1]",
            // Case 3
            "[0,0,0,0,1,1][0,0,0,0,1,1][1,1,2,2,2,2][1,1,2,2,2,2][1,1,2,2,2,2][1,1,2,2,2,2]",
            "[1,1,1,1,1,1][1,1,1,1,1,1][1,1,2,2,2,2][1,1,2,2,2,2][1,1,2,2,2,2][1,1,2,2,2,2]",
            // Case 4
            "[0,0,0,0,0,0][0,0,0,1,2,2][0,0,0,1,2,2][0,0,1,1,0,0][1,1,2,3,2,3][1,1,2,3,2,3]",
            "[0,0,2,2,2,2][0,0,2,2,2,2][0,1,1,1,2,2][0,1,1,1,2,2][1,1,0,0,1,1][1,1,1,1,1,1]",
            // Case 5
            "[0,0,0,0,0,0][0,1,1,1,2,2][0,1,1,1,2,2][1,0,0,1,1,1][2,1,1,2,0,1][2,1,1,2,0,1]",
            "[0,1,1,1,3,3][1,0,1,1,3,3][1,1,1,1,1,1][1,1,1,1,1,1][2,2,1,1,0,1][2,2,1,1,1,0]",
            // Case 6
            "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,2,2,3,3][1,1,2,2,2,2][2,2,0,0,3,3][2,2,1,1,2,2]",
            "[0,0,0,0,1,1][0,0,1,1,0,0][2,2,1,1,2,2][2,2,1,1,2,2][3,3,2,2,1,1][3,3,2,2,1,1]",
            // Case 7
            // "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,2,2][0,0,0,0,2,2]",
            // Case 8
            "[0,0,2,2,3,3][0,0,2,2,3,3][2,3,1,3,1,2][2,3,1,3,1,2][3,2,2,1,3,1][3,2,2,1,3,1]",
            "[1,1,3,3,3,3][1,1,3,3,3,3][2,2,1,1,3,3][2,2,1,1,3,3][2,2,2,2,1,1][2,2,2,2,1,1]",
        }
    },
    {
        false,
        {
            "[3,3,0,0,3,2][0,0,2,3,3,3][2,3,0,0,3,3][3,2,3,2,0,0][0,0,3,3,2,3][3,2,3,3,0,0]",
            // Should these fail???
            "[2,2,0,0,0,0][2,2,0,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0][0,0,0,0,2,2][0,0,0,0,2,2]",  // Case 7
            "[0,0,0,0,2,2][0,0,0,0,2,2][0,0,0,1,0,0][0,0,1,0,0,0][0,1,0,0,0,0][1,0,0,0,0,0]",  // Case 1

        }
    },
};


TEST(PatternMatrixTest, PatternDefaultMatrixConstructor) {
    patternMatrix pm = patternMatrix();
    zmatrix pmz = zmatrix(6,6,3);
    zmatrix cz = zmatrix(6,6,1);
    zmatrix groupings = zmatrix(6,6,36);
    EXPECT_EQ(pm.id, 0);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 0);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, -1);
    EXPECT_EQ(pm.subCaseMatch, -1);
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_EQ(pm.p, pmz);
    EXPECT_EQ(pm.pT, pmz);
    EXPECT_EQ(pm.swap23, pmz);
    EXPECT_EQ(pm.swap23T, pmz);
    EXPECT_EQ(pm.cV, cz);
    EXPECT_EQ(pm.cVT, cz);
    EXPECT_EQ(pm.originalMatrix, "[0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0][0,0,0,0,0]");
    EXPECT_EQ(pm.pGroupings, groupings);
}

TEST(PatternMatrixTest, PatternMatrixLoadFromString) {
    patternMatrix pm = patternMatrix();
    EXPECT_THROW(pm.loadFromString(""),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(TOO_FEW_ROWS),std::runtime_error);
    EXPECT_THROW(pm.loadFromString(TOO_FEW_COLUMNS),std::runtime_error);

    // TODO - add more tests to validate the pattern matrix is populated correctly for these cases
    EXPECT_NO_THROW(pm.loadFromString(VALID_BINARY_PATTERN));
    EXPECT_NO_THROW(pm.loadFromString(VALID_NUMERICAL_PATTERN));
}

TEST(PatternMatrixTest,PatternMatrixStringConstructor) {
    patternMatrix pm = patternMatrix(1, ALL_ZEROS_PATTERN);
    zmatrix pmz = zmatrix(6,6,3);
    pmz.updateMetadata();
    zmatrix cz = zmatrix(6,6,1);
    cz.updateMetadata();
    zmatrix groupings = initGroupings();
    EXPECT_EQ(pm.id, 1);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 0);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, -1);
    EXPECT_EQ(pm.subCaseMatch, -1);
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_EQ(pm.originalMatrix, ALL_ZEROS_PATTERN);
    EXPECT_EQ(pm.pGroupings, groupings);
    EXPECT_TRUE(pm.p == pmz);
    EXPECT_TRUE(pm.pT == pmz);
    EXPECT_TRUE(pm.swap23 == pmz);
    EXPECT_TRUE(pm.swap23T == pmz);
    EXPECT_TRUE(pm.cV == cz);
    EXPECT_TRUE(pm.cVT == cz);
    // Make sure that the originalMatrix is converted properly
    patternMatrix pm2 = patternMatrix(1, VALID_BINARY_PATTERN);
    EXPECT_EQ(pm2.originalMatrix, VALID_BINARY_PATTERN_IN_NUMERICAL_FORM);
}

TEST(PatternMatrixTest,PatternMatrixNewEncodingConstructor) {
    GTEST_SKIP() << "Not implemented yet";
}

TEST(PatternMatrixTest,PatternMatrixIsTranspose) {
    patternMatrix pm1 = patternMatrix(1, "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[1,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,0,0,0,0,0][0,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[1,0,1,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    EXPECT_TRUE(pm1.isTranspose(pm2));
    EXPECT_FALSE(pm1.isTranspose(pm3));
    // Check for transposes that would need to be rearranged to match
    EXPECT_TRUE(pm1.isTranspose(pm4));
    EXPECT_TRUE(pm2.isTranspose(pm5));
    // pm1 and pm5 are equivalent but not transposes
    EXPECT_FALSE(pm1.isTranspose(pm5));
}

TEST(PatternMatrixTest,PatternMatrixIs23Swap) {
    patternMatrix pm1 = patternMatrix(1, "[2,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm6 = patternMatrix(1, "[3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm7 = patternMatrix(1, "[0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm8 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][3,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");

    EXPECT_FALSE(pm1.is23Swap(pm2));
    EXPECT_FALSE(pm1.is23Swap(pm3));
    EXPECT_FALSE(pm1.is23Swap(pm4));
    EXPECT_FALSE(pm1.is23Swap(pm5));
    EXPECT_TRUE(pm1.is23Swap(pm6));
    // Check for 23 swaps that would need to be rearranged to match
    EXPECT_TRUE(pm1.is23Swap(pm7));
    EXPECT_TRUE(pm1.is23Swap(pm8));
    // This is also a 23 swap that could be rearranged to match
    EXPECT_TRUE(pm1.is23Swap(pm1));
    // This time the 23 swap cannot be rearranged to match
    EXPECT_FALSE(pm2.is23Swap(pm2));
    // But does match pm3
    EXPECT_TRUE(pm2.is23Swap(pm3));
}

TEST(PatternMatrixTest,PatternMatrixIs23SwapT) {
    patternMatrix pm1 = patternMatrix(1, "[2,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm2 = patternMatrix(1, "[3,0,0,0,0,0][3,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm3 = patternMatrix(1, "[2,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm4 = patternMatrix(1, "[1,0,0,0,0,0][1,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm5 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm6 = patternMatrix(1, "[3,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm7 = patternMatrix(1, "[0,0,0,0,0,0][3,0,0,0,0,0][2,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    patternMatrix pm8 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,0,0,0,0][3,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
    
    EXPECT_FALSE(pm1.is23SwapT(pm1));
    EXPECT_FALSE(pm1.is23SwapT(pm2));
    EXPECT_FALSE(pm1.is23SwapT(pm3));
    EXPECT_FALSE(pm1.is23SwapT(pm4));
    EXPECT_FALSE(pm1.is23SwapT(pm5));
    EXPECT_TRUE(pm1.is23SwapT(pm6));
    // Check for 23 swaps that would need to be rearranged to match
    EXPECT_TRUE(pm1.is23SwapT(pm7));
    EXPECT_TRUE(pm1.is23SwapT(pm8));
}

TEST(PatternMatrixTest,PatternMatrixLoadCases) {
    patternMatrix pm1 = patternMatrix(1, ALL_ZEROS_PATTERN);
    // These are the cases we expect to have and the test is to ensure we load all the cases we intend to
    std::vector<caseMatrix> baseCases;
    baseCases.push_back(caseMatrix(0, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(1, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(2, "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(3, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][1,1,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(4, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(5, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(6, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,0,0,0,0][0,0,0,0,0,0]"));
    baseCases.push_back(caseMatrix(7, "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"));
    baseCases.push_back(caseMatrix(8, "[1,1,1,1,0,0][1,1,1,1,0,0][1,1,0,0,1,1][1,1,0,0,1,1][0,0,1,1,1,1][0,0,1,1,1,1]"));
    for (int i = 0; i < baseCases.size(); i++) {
        EXPECT_TRUE(pm1.cases[i].id == baseCases[i].id);
        EXPECT_TRUE(pm1.cases[i].c == baseCases[i].c);
        EXPECT_TRUE(pm1.cases[i].cT == baseCases[i].cT);
    }
}

// TODO - Use the test data as well
TEST(PatternMatrixTest,PatternMatrixMatchesCase) {
    for (auto const& [caseNumber, patterns] : CASE_TO_VALID_PATTERN_MAP) {
        // Skip cases that are not valid here
        if (caseNumber == -1) {
            continue;
        }
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
            EXPECT_TRUE(pm.matchesCase(caseNumber)) << "Case: " << caseNumber << " Pattern: " << pattern;
        }
    }    
}

// TODO - Use the test data as well
TEST(PatternMatrixTest,PatternMatrixMatchOnCases) {
    for (auto const& [caseNumber, patterns] : CASE_TO_VALID_PATTERN_MAP) {
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
            pm.matchOnCases();
            EXPECT_TRUE(pm.caseMatch == caseNumber) << "Expected: " << caseNumber << " Got: " << pm.caseMatch << " with Pattern: " << pattern;
        }
    }
}

TEST(PatternMatrixTest,PatternMatrixDetermineSubCase) {
    GTEST_SKIP() << "Not implemented yet";
}

// TODO - Rearrangement Refactor + Test Overhaul
//  This is a shortened version of the rearrangement test
//  To do a full up test, run the test file dedicated to rearrangements
TEST(PatternMatrixTest,PatternMatrixRearrangeMatrixShort) {
    // Test all of the possible rearrangements but for a limited set of cases
    for (auto const& [caseNumber, patterns] : REARRANGE_CASES_SHORT) {
        if (caseNumber == -1 || caseNumber == 0) {
            continue;
        }
        // Uncomment this to when printing rearrangements
        //std::cout << "Case: " << caseNumber << std::endl;
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
            EXPECT_TRUE(pm.rearrangeMatrix());
            /*  Uncomment this to see the first rearrangement for each case
            std::cout << "Pattern: " << pattern << " has " << pm.caseRearrangements.size() << " rearrangements" << std::endl;
            bool print = true;
            for (auto const& pair : pm.caseRearrangements) {
                if (!print) {
                    break;
                }
                auto key = pair.first;
                std::cout << key << std::endl;
                print = false;
            }
            */
        }
    }
    // Test single rearrangements for a larger set of patterns
    for (auto const& [caseNumber, patterns] : CASE_TO_VALID_PATTERN_MAP) {
        if (caseNumber == -1 || caseNumber == 0) {
            continue;
        }
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
            pm.singleCaseRearrangement = true;
            EXPECT_TRUE(pm.rearrangeMatrix());
            EXPECT_TRUE(pm.caseRearrangements.size() == 1);
        }
    }
}

// TODO - Rearrangement Refactor + Test Overhaul
TEST(PatternMatrixTest,PatternMatrixOptimalCaseRearrangements) {
    GTEST_SKIP() << "Not implemented yet";
}

// TODO: Implement the following tests
TEST(PatternMatrixTest,PatternMatrixLeftTGateMultiply) {
    patternMatrix pm1 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,1,1][0,0,0,1,2,2][0,0,0,1,2,2]");
    if(!pm1.rearrangeMatrix()) FAIL() << "Failed to rearrange matrix";
    pm1.multilineOutput = true;
    pm1.printOldEncoding = true;
    std::cout << "Original Matrix: " << std::endl;
    std::cout << pm1 << std::endl;
    patternMatrix pmR = patternMatrix(1, pm1.caseRearrangements.begin()->first);
    pmR.multilineOutput = true;
    pmR.printOldEncoding = true;
    std::cout << "Rearranged Matrix: " << std::endl;
    std::cout << pmR << std::endl;
    pmR.leftTGateMultiply(0,1);
    std::cout << "leftTGateMultiply: " << std::endl;
    std::cout << pmR << std::endl;
    std::cout << "Groupings" << std::endl;
    pmR.pGroupings.multilineOutput = true;
    std::cout << pmR.pGroupings << std::endl;
    std::cout << "LDEs - NO FACTORING DONE" << std::endl;
    for (int i=0; i < 6; i++) {
        std::cout << "[";
        for (int j=0; j < 6; j++) {
            std::cout << pmR.entryLDEs[i][j] << " ";
        }
        std::cout << "]" << std::endl;
    }
    GTEST_SKIP() << "Not finished";
}

TEST(PatternMatrixTest,PatternMatrixRightTGateMultiply) {
    patternMatrix pm1 = patternMatrix(1, "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,1,1][0,0,0,1,2,2][0,0,0,1,2,2]");
    if(!pm1.rearrangeMatrix()) FAIL() << "Failed to rearrange matrix";
    pm1.multilineOutput = true;
    pm1.printOldEncoding = true;
    std::cout << "Original Matrix: " << std::endl;
    std::cout << pm1 << std::endl;
    patternMatrix pmR = patternMatrix(1, pm1.caseRearrangements.begin()->first);
    pmR.multilineOutput = true;
    pmR.printOldEncoding = true;
    std::cout << "Rearranged Matrix: " << std::endl;
    std::cout << pmR << std::endl;
    pmR.rightTGateMultiply(0,1);
    std::cout << "RightTGateMultiply: " << std::endl;
    std::cout << pmR << std::endl;
    std::cout << "Groupings" << std::endl;
    pmR.pGroupings.multilineOutput = true;
    std::cout << pmR.pGroupings << std::endl;
    std::cout << "LDEs - NO FACTORING DONE" << std::endl;
    for (int i=0; i < 6; i++) {
        std::cout << "[";
        for (int j=0; j < 6; j++) {
            std::cout << pmR.entryLDEs[i][j];
            if (j < 5) std::cout << ",";
        }
        std::cout << "]" << std::endl;
    }
    GTEST_SKIP() << "Not finished";
}

TEST(PatternMatrixTest,PatternMatrixPrintMatchComparison) {
    GTEST_SKIP() << "Not implemented yet";
}

// TODO - This might be better to put outside of the PatternMatrix and into another file
//  But should be determined after figuring out how to handle LDEs
TEST(PatternMatrixTest,PatternMatrixPatternElementAddition) {
    patternMatrix pm = patternMatrix(1, ALL_ZEROS_PATTERN);
    //GTEST_SKIP() << "Not implemented yet";
    // Pattern addition properties based on above:
    //  0 + 0,1,2,3 = 0,1,2,3
    //  1 + 1,2,3 = 0,3,2
    //  2 + 2,3 = 0,1
    //  3 + 3 = 0
    std::vector<std::vector<int>> results = {
        // 0, 1, 2, 3
        {0, 1, 2, 3}, // 0
        {1, 0, 3, 2}, // 1
        {2, 3, 0, 1}, // 2
        {3, 2, 1, 0}, // 3
    };
    for (int i=0; i < results.size(); i++) {
        for (int j=0; j < results[i].size(); j++) {
            // Test the addition of the pattern elements
            EXPECT_EQ(results[i][j], pm.patternElementAddition(i,j)) << "i: " << i << " j: " << j;
        }
    }
}

TEST(PatternMatrixTest,PatternMatrixIsOrthonormal) {
    for (auto const& [expected, patterns] : ORTHONORMAL_PATTERN_CHECKS) {
        for (auto const& pattern : patterns) {
            patternMatrix pm = patternMatrix(1, pattern);
            pm.matchOnCases();
            // Uncomment this to see the pattern that is not orthonormal
            //if (!expected) {
            //    pm.printDebugInfo = true;
            //   std::cout << "Pattern: " << pattern << "; Case: " << pm.caseMatch << std::endl;
            //}
            bool got = pm.isOrthonormal();
            EXPECT_EQ(got, expected) << "Expected: " << expected << " Got: " << got << " with Pattern: " << pattern << "; Case: " << pm.caseMatch;
            // Debugging for failed tests
            if (got != expected) {
                pm.printDebugInfo = true;
                pm.isOrthonormal();
            }
        }
    }
}
