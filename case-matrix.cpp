#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "case-matrix.hpp"
#include "zmatrix.hpp"

caseMatrix::caseMatrix(int caseNumber, std::string matrix) {
    id = caseNumber;
    c = zmatrix(rows, cols, 1);
    cT = zmatrix(cols, rows, 1);
    c.strictMatch = false;
    cT.strictMatch = false;
    loadFromString(matrix);
}

void caseMatrix::loadFromString(std::string m) {
    std::stringstream ms(m);
    std::string r;
    int row = 0;
    while(std::getline(ms, r, ']'))
    {
        if(row == rows) throw std::runtime_error("Too many rows in pattern string");
        std::stringstream rs(r);
        std::string v;
        int mv;
        int col = 0;
        while(std::getline(rs, v, ','))
        {
            if(col == cols) throw std::runtime_error("Too many columns in pattern string");
            if(v[0] == '[') v.erase(0,1);
            
            if(v == "0") mv = 0;
            else if (v == "1") mv = 1;
            else {
                std::ostringstream verr;
                verr << "Invalid value in case string: " << v ;
                throw std::runtime_error(verr.str());
            }
            c.z[row][col] = mv;
            cT.z[col][row] = mv;
            col++;
        }
        row++;
    }
    c.updateMetadata();
    cT.updateMetadata();
}

void caseMatrix::printDebug() {
    std::cout << "Case Number: " << id << std::endl;
    std::cout << "Case Matrix Debug:" << std::endl;
    c.printDebug();
    std::cout << "Transposed Case Matrix Debug:" << std::endl;
    cT.printDebug();
}

std::ostream& operator<<(std::ostream& os, const caseMatrix &cm) {
    os << cm.id << " " << cm.c;
    return os;
}
