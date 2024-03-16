case-matching: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-matching.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-matching.cpp -std=c++11 -o case-matching.out
ztest: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp ztest.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp ztest.cpp -std=c++11 -o ztest.out
dupe-checks: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp dupe-checks.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp dupe-checks.cpp -std=c++11 -o dupe-checks.out
convert-patterns: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp convert-patterns.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp convert-patterns.cpp -std=c++11 -o convert-patterns.out
pattern-file-checks: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp pattern-file-checks.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp pattern-file-checks.cpp -std=c++11 -o pattern-file-checks.out
case-8b: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-8b.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-8b.cpp -std=c++11 -o case-8b.out
case-3: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-3.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-3.cpp -std=c++11 -o case-3.out
case-traversal: case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-traversal.cpp
	g++ case-matrix.cpp pattern-matrix.cpp zmatrix.cpp case-traversal.cpp -std=c++11 -o case-traversal.out