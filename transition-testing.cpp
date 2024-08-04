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
    bool printOldEncoding = true;
    if (printOldEncoding) {
        std::cout << "Using old encoding" << std::endl;
    } else {
        std::cout << "Using new encoding" << std::endl;
    }
    patternMatrix p352 = patternMatrix(352, "[1,3,1,3,0,0][3,1,3,1,0,0][1,3,3,1,2,2][3,1,1,3,2,2][0,0,2,2,0,0][0,0,2,2,0,0]", true);
    patternMatrix p879 = patternMatrix(879, "[1,1,3,3,0,0][3,3,1,1,0,0][1,3,3,1,2,2][3,1,1,3,2,2][0,2,0,2,2,2][0,2,0,2,2,2]", true);
    patternMatrix p64a = patternMatrix(64, "[1,1,1,1,0,0][1,1,3,3,0,0][1,3,1,3,2,2][1,3,3,1,2,2][0,0,0,0,0,0][0,0,0,0,0,0]", true);
    patternMatrix p64b = patternMatrix(64, "[1,1,1,1,0,0][1,1,3,3,0,0][1,3,1,3,2,2][1,3,3,1,2,2][0,0,0,0,0,0][0,0,0,0,0,0]", true);

    //p352.printOldEncoding = true;
    p352.pGroupings.multilineOutput = true;
    p352.multilineOutput = true;
    p352.printOldEncoding = printOldEncoding;

    //p879.printOldEncoding = true;
    p879.pGroupings.multilineOutput = true;
    p879.multilineOutput = true;
    p879.printOldEncoding = printOldEncoding;

    
    std::cout << "Case 3c: pattern 352:\n" << p352 << std::endl;
    p352.rightTGateMultiply(1,4);
    p352.rightTGateMultiply(2,3);
    p352.ldeReductionOnPattern(1);

    std::cout << "After T-Gate multiplication (xT14xT23):\n" << p352 << std::endl;
    std::cout << "Groupings:\n" << p352.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p352.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p352.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << p352.getMaxOfPossibleValues() << std::endl;
    patternMatrix p352Post = patternMatrix(352, p352.getMaxOfPossibleValues(), !printOldEncoding);
    p352Post.matchOnCases();
    std::cout << "Case Match of p352 post: " << p352Post.caseMatch << std::endl;

    /*
    std::cout << "\n----------------------------------------\n" << std::endl;

    std::cout << "Case 3c: pattern 879:\n" << p879 << std::endl;
    p879.rightTGateMultiply(1,2);
    p879.rightTGateMultiply(3,4);
    p879.ldeReductionOnPattern(1);
    std::cout << "After T-Gate multiplication (xT12xT34):\n" << p879 << std::endl;
    std::cout << "Groupings:\n" << p879.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p879.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p879.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << p879.getMaxOfPossibleValues() << std::endl;
    patternMatrix p879Post = patternMatrix(879, p879.getMaxOfPossibleValues(), !printOldEncoding);
    p879Post.matchOnCases();
    std::cout << "Case Match of p879 post: " << p879Post.caseMatch << std::endl;

    std::cout << "\n----------------------------------------\n" << std::endl;
    p64a.pGroupings.multilineOutput = true;
    p64a.multilineOutput = true;
    p64a.printOldEncoding = printOldEncoding;
    
    std::cout << "Case 3b: pattern 64 path A:\n" << p64a << std::endl;
    p64a.rightTGateMultiply(1,4);
    p64a.rightTGateMultiply(2,3);
    p64a.ldeReductionOnPattern(1);
    
    std::cout << "After T-Gate multiplication " << p64a.printTGateOperations() << ":" << std::endl;
    std::cout << p64a << std::endl;
    std::cout << "Groupings:\n" << p64a.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p64a.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p64a.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << p64a.getMaxOfPossibleValues() << std::endl;
    patternMatrix p64aPost = patternMatrix(64, p64a.getMaxOfPossibleValues(), !printOldEncoding);
    p64aPost.matchOnCases();
    std::cout << "Case Match of p64 path A post: " << p64aPost.caseMatch << std::endl;
    p64aPost.rearrangeMatrix();
    patternMatrix p64aPostRearranged = patternMatrix(64, p64aPost.getFirstCaseRearrangement(), false);
    p64aPostRearranged.multilineOutput = true;
    p64aPostRearranged.pGroupings.multilineOutput = true;
    p64aPostRearranged.printOldEncoding = printOldEncoding;
    std::cout << "After rearrangement:\n" << p64aPost.getFirstCaseRearrangement() << std::endl;
    p64aPostRearranged.rightTGateMultiply(1,2);
    //p64aPostRearranged.leftTGateMultiply(5,6);
    p64aPostRearranged.ldeReductionOnPattern(1);
    std::cout << "After T-Gate multiplication:\n" <<  p64aPostRearranged << std::endl;
    std::cout << "Groupings:\n" << p64aPostRearranged.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p64aPostRearranged.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p64aPostRearranged.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << p64aPostRearranged.getMaxOfPossibleValues() << std::endl;

    
    std::cout << "\n----------------------------------------\n" << std::endl;

    /*
    p64b.pGroupings.multilineOutput = true;
    p64b.multilineOutput = true;

    std::cout << "Case 3b: pattern 64 path B:\n" << p64b << std::endl;
    p64b.leftTGateMultiply(1,2);
    p64b.leftTGateMultiply(3,4);
    p64b.ldeReductionOnPattern(1);
    std::cout << "After T-Gate multiplication:\n" << p64b << std::endl;
    std::cout << "Groupings:\n" << p64b.pGroupings << std::endl;
    std::cout << "LDEs:" << std::endl;
    p64b.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    p64b.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << p64b.getMaxOfPossibleValues() << std::endl;
    patternMatrix p64bPost = patternMatrix(64, p64b.getMaxOfPossibleValues(), !printOldEncoding);
    p64bPost.matchOnCases();
    std::cout << "Case Match of p64 path B post: " << p64bPost.caseMatch << std::endl;

    std::cout << "\n----------------------------------------\n" << std::endl;
    /*
    patternMatrix pX = patternMatrix(3, "[1,1,0,2,0,0][1,1,0,2,0,0][1,1,0,2,0,0][1,1,0,2,0,0][0,2,2,0,0,0][0,2,2,0,0,0]", true);
    pX.printDebugInfo = true;
    if (pX.isOrthonormal()) {
        std::cout << "Pattern X is orthonormal" << std::endl;
    } else {
        std::cout << "Pattern X is not orthonormal" << std::endl;
    }
    */
    return 0;
}
