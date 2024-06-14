#ifndef CASE_MATRIX_HPP
#define CASE_MATRIX_HPP

#include "zmatrix.hpp"
class caseMatrix {
    public:
        caseMatrix(int caseNumber, std::string m);
        void loadFromString(std::string m);
        int id;
        zmatrix c; // This is the case matrix
        zmatrix cT;  // This is the transposed case matrix
        void printDebug();
        friend std::ostream& operator<<(std::ostream&,const caseMatrix &);
    private:
        int rows = 6;
        int cols = 6;
};

#endif // CASE_MATRIX_HPP