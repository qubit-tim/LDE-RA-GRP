#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "zmatrix.hpp"
#include "case-matrix.hpp"
#include "pattern-matrix.hpp"

using namespace std;

int main(int argc, char **argv) {
    zmatrix z1(6, 6, 3);
    z1.z = {{0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3}};
    z1.updateMetadata();
    // z1.printDebug();
    zmatrix z2(6, 6, 3);
    z2.z = {{0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 3},
           {0, 0, 1, 1, 2, 2}};
    z2.updateMetadata();
    // z2.printDebug();
    std::string cm = "[1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][1,1,0,0,0,0][0,0,0,0,0,0][0,0,0,0,0,0]";
    caseMatrix c(2,cm);
    // c.printDebug();
    std::string pm = "[0 0,0 0,0 0,0 1,1 1,1 1][0 0,0 0,0 0,0 1,1 1,1 1][0 1,1 0,1 0,0 0,0 0,0 0][0 1,1 0,1 0,0 0,0 0,0 0][0 1,1 0,1 0,0 1,1 1,1 1][0 1,1 0,1 0,0 1,1 1,1 1]";
    patternMatrix p(567,pm);
    // p.printDebug();
    std::cout << "Now for straight prints" << std::endl;
    std::cout << "z  = " << z1 << std::endl;
    std::cout << "c  = " << c << std::endl;
    std::cout << "p  = " << p << std::endl;
    std::cout << "Now to test equalities" << std::endl;
    std::cout << "z1 == z2 = " << (z1 == z2) << std::endl;
    std::vector <int> a1 = {1,2,3,4,5,6};
    std::vector <int> a2 = {1,2,3,4,5,6};
    std::cout << "a1 == a2 = " << (a1 == a2) << std::endl;
}
