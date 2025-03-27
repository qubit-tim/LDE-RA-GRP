#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

#include <gtest/gtest.h>

/*  Here, we can take case 2, apply T gates, generate the possible value patterns, and see where they go.
  They should all into the following groups and will be useful for testing case transitions and LDE reductions
group 1: (lead to entire case 2) 3, 4, 5, 6
group 2: (lead to entire case 5a+b) 7,8,9,10,11,12
group 3: (lead to entire case 4) 19,20,24,115,116,117
group 5: (lead to entire case 3) 93,94,188,189
group 6: (lead to entire case 6) 95,96,98,365,366,368
group 7: (lead to entire case 6) 48,49,50,55,72,73
group 8: (lead to entire case 8) 190,191,192,194,197,198
*/

