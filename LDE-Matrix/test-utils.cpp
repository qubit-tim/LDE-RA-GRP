#include "pattern-matrix.hpp"
#include "zmatrix.hpp"

zmatrix initGroupings() {
    zmatrix groupings = zmatrix(6, 6, 36);
    int g = 1;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            groupings.z[i][j] = g;
            g++;
        }
    }
    groupings.updateMetadata();
    return groupings;
}
