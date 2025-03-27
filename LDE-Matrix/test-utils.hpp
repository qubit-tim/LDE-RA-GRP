#ifndef LDE_MATRIX_TEST_UTILS_HPP
#define LDE_MATRIX_TEST_UTILS_HPP

#include <regex>

std::regex R_T_GATE_REGEX("(xT[1-6][1-6])");
std::regex L_T_GATE_REGEX("(T[1-6][1-6]x)");

zmatrix initGroupings();

#endif // LDE_MATRIX_TEST_UTILS_HPP