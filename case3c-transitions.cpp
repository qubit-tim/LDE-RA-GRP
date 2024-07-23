#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <future>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"

int main(int argc, char **argv) {
    patternMatrix p352 = patternMatrix(352, "[1,3,1,3,0,0][3,1,3,1,0,0][1,3,3,1,2,2][3,1,1,3,2,2][0,0,2,2,0,0][0,0,2,2,0,0]", true);
    patternMatrix p879 = patternMatrix(879, "[1,1,3,3,0,0][3,3,1,1,0,0][1,3,3,1,2,2][3,1,1,3,2,2][0,2,0,2,2,2][0,2,0,2,2,2]", true);

    //p352.printOldEncoding = true;
    p352.pGroupings.multilineOutput = true;
    p352.multilineOutput = true;

    //p879.printOldEncoding = true;
    p879.pGroupings.multilineOutput = true;
    p879.multilineOutput = true;    

    std::cout << "Case 3c: pattern 352:\n" << p352 << std::endl;
    p352.leftTGateMultiply(0,3);
    p352.leftTGateMultiply(1,2);

    std::cout << "After T-Gate multiplication:\n" << p352 << std::endl;
    std::cout << "Groupings:\n" << p352.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p352.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p352.printPossibleValues(std::cout);

    std::cout << "\n----------------------------------------\n" << std::endl;

    std::cout << "Case 3c: pattern 879:\n" << p879 << std::endl;
    p879.leftTGateMultiply(0,1);
    p879.leftTGateMultiply(2,3);
    std::cout << "After T-Gate multiplication:\n" << p879 << std::endl;
    std::cout << "Groupings:\n" << p879.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p879.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p879.printPossibleValues(std::cout);

    return 0;
}
