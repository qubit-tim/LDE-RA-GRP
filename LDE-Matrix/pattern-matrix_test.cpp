#include "pattern-matrix.hpp"
#include "zmatrix.hpp"
#include "test-utils.hpp"

#include <gtest/gtest.h>

std::string TOO_FEW_ROWS = "[0 0,0 0,0 0,0 0,0 0,0 0]";
std::string TOO_FEW_COLUMNS = "[0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0]";
std::string VALID_BINARY_PATTERN = "[0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 0,0 0,0 0,0 0,0 0,0 0][0 1,0 1,0 1,0 1,0 1,0 1][1 0,1 0,1 0,1 0,1 0,1 0][1 1,1 1,1 1,1 1,1 1,1 1]";
std::string VALID_BINARY_PATTERN_IN_NUMERICAL_FORM = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][1,1,1,1,1,1][2,2,2,2,2,2][3,3,3,3,3,3]";
std::string VALID_BINARY_PATTERN_IN_NUMERICAL_FORM_NEW_ENCODING = "[0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][2,2,2,2,2,2][1,1,1,1,1,1][3,3,3,3,3,3]";
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
            "[3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][3,3,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,3,0,0,3][0,0,0,0,0,0][0,0,2,0,0,2][0,0,0,0,0,0][0,0,3,0,0,3][0,0,2,0,0,2]",
            // transpose cases
            "[2,2,2,2,0,0][2,2,2,2,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0]",
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

// These are a limited set of the patterns which were taken from case files
std::map <int, std::map<char, std::string> > SUBCASE_NEW_ENCODING_MAP = {
    {
        1,  {
            {'-', "[1,1,0,2,0,0][1,1,0,2,0,0][0,0,0,0,0,0][2,2,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]"},
        }
    },
    {
        2, {
            {'-', "[1,1,0,2,0,0][1,1,0,2,0,0][3,3,2,2,0,2][3,3,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0]"},
        },
    },
    {
        3, {
            {'a', "[1,1,3,3,0,0][3,3,1,1,0,0][1,1,1,1,2,2][3,3,3,3,2,2][0,0,0,0,0,0][0,0,0,0,0,0]"},
            {'b', "[1,1,1,1,0,0][3,3,3,3,0,0][1,3,1,3,2,2][3,1,3,1,2,2][0,0,2,2,0,0][0,0,2,2,0,0]"},
            {'c', "[1,1,3,3,0,0][3,3,1,1,0,0][1,3,3,1,2,2][3,1,1,3,2,2][0,2,0,2,2,2][0,2,0,2,2,2]"},
        },
    },
    {
        4, {
            {'a', "[1,1,1,1,2,2][1,1,1,1,2,2][1,1,2,2,0,2][1,1,2,2,0,2][0,0,0,0,0,0][0,0,2,2,0,0]"},
            {'b', "[1,3,1,3,2,2][3,1,1,3,2,2][1,1,0,2,0,0][3,3,2,0,0,0][0,0,2,2,0,0][0,0,2,2,0,0]"},
        },
    },
    {
        5, {
            {'a', "[1,1,2,2,0,2][1,1,2,2,0,2][2,2,3,3,0,2][2,2,3,3,0,2][0,0,0,0,0,0][2,2,2,2,0,0]"},
            {'b', "[1,1,0,2,2,2][1,1,2,0,2,2][0,2,3,3,2,2][2,0,3,3,2,2][0,0,0,0,2,2][0,0,0,0,2,2]"},
        },
    },
    {
        6, {
            {'a', "[3,3,1,1,0,0][3,3,1,1,0,0][1,1,0,0,3,3][1,1,0,0,3,3][0,0,0,0,0,0][0,0,2,2,2,2]"},
            {'b', "[1,1,3,3,2,2][3,3,3,3,0,0][1,1,0,0,3,3][3,3,2,2,3,3][0,0,0,0,2,2][0,0,0,0,2,2]"},
            {'c', "[1,3,1,3,2,2][3,1,1,3,0,0][1,3,0,0,1,3][3,1,2,2,1,3][0,0,2,2,2,2][0,0,2,2,2,2]"},
        },
    },
    {
        7, {
            {'-', "[1,1,0,0,0,0][1,1,0,0,0,0][0,0,1,1,0,0][0,0,1,1,0,0][0,0,0,0,1,1][0,0,0,0,1,1]"},
        },
    },
    {
        8, {
            {'a', "[1,1,1,1,0,0][3,3,3,3,0,0][1,1,0,0,1,1][3,3,0,0,3,3][0,0,1,1,1,1][0,0,3,3,3,3]"},
            {'a', "[3,1,3,1,2,2][3,1,3,1,2,2][1,3,2,2,3,1][1,3,2,2,3,1][2,2,1,3,1,3][2,2,1,3,1,3]"},
        },
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
            "[0,0,3,0,0,3][0,0,0,0,0,0][0,0,2,0,0,2][0,0,0,0,0,0][0,0,3,0,0,3][0,0,2,0,0,2]",
            "[0,0,0,0,0,0][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0][2,0,3,0,2,3][0,0,0,0,0,0]",
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
    EXPECT_EQ(pm.subCaseMatch, '-');
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

// TODO - Add a few more test cases and verify that the entire data structure is populated correctly
//  I found that possible values wasn't but has been fixed so there might be more issues
TEST(PatternMatrixTest, PatternMatrixLoadFrom928Pattern) {
    patternMatrix pm = patternMatrix(1);
    pm.printOldEncoding = false;
    EXPECT_EQ(pm.id, 1);
    EXPECT_EQ(pm.id2704, 0);
    EXPECT_EQ(pm.id928, 1);
    EXPECT_EQ(pm.id785, 0);
    EXPECT_EQ(pm.caseMatch, 1);
    EXPECT_EQ(pm.subCaseMatch, '-');
    EXPECT_EQ(pm.printID, false);
    EXPECT_EQ(pm.printCaseMatch, false);
    EXPECT_EQ(pm.printAllIDs, false);
    EXPECT_EQ(pm.originalMatrix, "[2,2,0,1,0,0][2,2,0,1,0,0][0,0,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]");
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
    EXPECT_EQ(pm.subCaseMatch, '-');
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

// TODO - Add a few more test cases
TEST(PatternMatrixTest,PatternMatrixNewEncodingConstructor) {
    // Using a case match from the 928 group to test the new encoding
    // Case 2 - 27 [1,1,0,2,0,0][1,1,2,0,0,0][1,1,0,0,0,2][1,1,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0]
    patternMatrix pm = patternMatrix(27, "[1,1,0,2,0,0][1,1,2,0,0,0][1,1,0,0,0,2][1,1,2,2,0,2][0,0,0,0,0,0][0,0,0,0,0,0]", true);
    pm.printOldEncoding = true;
    EXPECT_EQ(pm.caseMatch, -1) << "Case Match should be -1";
    EXPECT_EQ(pm.subCaseMatch, '-');
    pm.matchOnCases();
    EXPECT_EQ(pm.caseMatch, 2) << "Case Match should be 2";
    EXPECT_EQ(pm.subCaseMatch, '-');
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
    for (auto const& [caseNumber, subcases] : SUBCASE_NEW_ENCODING_MAP) {
        for (auto const& [subcase, pattern] : subcases) {
            patternMatrix pm = patternMatrix(1, pattern, true);
            pm.matchOnCases();
            std::cout << "Case: " << caseNumber << " Subcase: " << subcase << " Pattern: " << pattern << std::endl;
            EXPECT_TRUE(pm.caseMatch == caseNumber) << "Expected: " << caseNumber << " Got: " << pm.caseMatch << " with Pattern: " << pattern;
            pm.determineSubCase();
            EXPECT_TRUE(pm.subCaseMatch == subcase) << "Expected: " << subcase << " Got: " << pm.subCaseMatch << " with Pattern: " << pattern;
        }
    }
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
            EXPECT_TRUE(pm.rearrangeMatrix()) << "Failed to rearrange matrix for pattern: " << pattern;
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
    /*
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
    pmR.leftTGateMultiply(1,2);
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
    */
    GTEST_SKIP() << "Not finished";
}

TEST(PatternMatrixTest,PatternMatrixRightTGateMultiply) {
    /*
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
    pmR.rightTGateMultiply(1,2);
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
    */
    GTEST_SKIP() << "Not finished";
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

// TODO: Split this into 2 tests, one for isNormalized and one for isOrthogonal
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
            bool got = pm.isNormalized() && pm.isOrthogonal();
            EXPECT_EQ(got, expected) << "Expected: " << expected << " Got: " << got << " with Pattern: " << pattern << "; Case: " << pm.caseMatch;
            // Debugging for failed tests
            if (got != expected) {
                pm.printDebugInfo = true;
                pm.isNormalized();
                pm.isOrthogonal();
            }
        }
    }
}

TEST(PatternMatrixTest,PatternMatrixGenerateAllPossibleValuePatterns) {
    GTEST_SKIP() << "Not finished";
}

TEST(PatternMatrixTest,PatternMatrixDoLDEReduction) {
    GTEST_SKIP() << "Not finished";
}

/*
Check these off based on test status
void loadFromString(std::string m);
void updatePairCounts();
// Case Matching Functions
void matchOnCases();
bool matchesCase(int caseIndex);
bool determineSubCase();
bool case1SubCaseMatch();
bool case2SubCaseMatch();
bool case3SubCaseMatch();
bool case3aSubCaseCheck();
bool case3bSubCaseCheck();
bool case3cSubCaseCheck();
bool case4SubCaseMatch();
bool case5SubCaseMatch();
bool case6SubCaseMatch();
bool case7SubCaseMatch();
bool case8SubCaseMatch();
std::string getFirstCaseRearrangement();

// Duplicate Pattern Checks
bool isDuplicate(patternMatrix other);
bool isTranspose(patternMatrix other);
bool is23Swap(patternMatrix other);
bool is23SwapT(patternMatrix other);
// Extra Printing Functions
void printDebug(std::ostream& os);
void printLDEs(std::ostream& os);
std::string rowPossibleValueToString(int row);
void printPossibleValues(std::ostream& os);
void printPairCounts(std::ostream& os);
void printRowPairCounts(std::ostream& os);
void printColPairCounts(std::ostream& os);
void printCounts(std::ostream& os);
void printRowCounts(std::ostream& os);
void printColCounts(std::ostream& os);
void printCountRows(std::ostream& os);
void printCountCols(std::ostream& os);
// Get a list of T-Gate operations applied
std::string printTGateOperations();
std::string getMaxOfPossibleValues();
bool possibleValuesLeadToAllPatterns();
void generateAllPossibleValuePatterns();
void optimizedGenerateAllPossibleValuePatterns();
void generateRowSet(int pvRow, int rsPos, std::vector<int> newRow, int pos);
void opt2GenerateAllPossibleValuePatterns();
void recursiveRowSetPatternGeneration(int curRow, std::vector<std::string> rowSelections);
void recursiveAllPossibleValueSet(int position, zmatrix z);
void optimizedAllPossibleValuePatterns(int position, zmatrix z);
// T-Gate Multiplication Functions
void leftTGateMultiply(int p, int q);
void rightTGateMultiply(int p, int q);
int patternElementAddition(int a, int b);
bool findOptimalTGateOperations();
bool optimalTGatesCase1();
bool optimalTGatesCase2();
bool optimalTGatesCase3();
bool optimalTGatesCase4();
bool optimalTGatesCase5();
bool optimalTGatesCase6();
bool optimalTGatesCase7();
bool optimalTGatesCase8();
bool findAllTGateOptions();
bool allTGatesCase1();
bool allTGatesCase2();
bool allTGatesCase3();
bool allTGatesCase4();
bool allTGatesCase5();
bool allTGatesCase6();
bool allTGatesCase7();
bool allTGatesCase8();
// LDE Reduction Functions
void ldeReductionOnEntry(int row, int col, int ldeReduction);
void ldeReductionOnEntryToTarget(int row, int col, int ldeReduction, int target);
void ldeReductionOnPattern(int ldeValue);
bool canFullyReduceLDE();  // This will check if the LDE can be fully reduced for the whole pattern
int getMaxLDEValue();
void doLDEReduction();
// These could be private but are public for testing
bool rearrangeMatrix();
void rearrangeColumns(zmatrix patternVersion, zmatrix caseVersion, int currentColumn);
void rearrangeRows(zmatrix patternVersion, zmatrix caseVersion, int currentRow);
std::string toString();
// TODO: Add a csv output for the pattern matrix
bool isSymmetric();
bool isNormalized();
bool isOrthogonal();
bool isRowNormalized(int row, zmatrix z);
bool isRowNormalized(std::vector<int> row);
bool areRowsOrthogonal(int row1, int row2, zmatrix z);
bool areRowsOrthogonal(std::vector<int> row1, std::vector<int> row2);
*/