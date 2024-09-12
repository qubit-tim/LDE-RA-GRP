#include <string>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"

int main(int argc, char **argv) {
    // 352 is the pattern number from the 928 file
    //  Feel free to replace with any other pattern number you want to test
    patternMatrix test = patternMatrix(352);
    test.multilineOutput = true;

    std::cout << "Before T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // Here, you can mulitply the pattern with T-Gates.  Feel free to update as desired.
    //  You can use the following functions to multiply the pattern with T-Gates:
    //    test.rightTGateMultiply(column1, column2) - Multiply the pattern with T-Gate on the right
    //    test.leftTGateMultiply(row1, row2) - Multiply the pattern with T-Gate on the left
    test.rightTGateMultiply(1,4);
    test.rightTGateMultiply(2,3);

    std::cout << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // If you want to reduce and see the LDEs then see possible values, uncomment this block
    test.ldeReductionOnPattern(1);
    std::cout << "LDEs:" << std::endl;
    test.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    test.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;

    /*
    // If you want to see groupings, uncomment this block
    test.pGroupings.multilineOutput = true;
    std::cout << "Groupings:\n" << test.pGroupings << std::endl;
    */

    return 0;
}