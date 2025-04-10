#ifndef LDE_MATRIX_POSSIBLE_PATTERNS_HPP
#define LDE_MATRIX_POSSIBLE_PATTERNS_HPP

#include <map>
#include <string>

extern std::map <int, std::string> POSSIBLE_PATTERNS;
// I want to rename this to something that describes that the possible patterns lead
//  to either valid patterns or a dead end in the tree
extern std::map <std::string, bool> POSSIBLE_PATTERN_VALIDITY;

#endif // LDE_MATRIX_POSSIBLE_PATTERNS_HPP