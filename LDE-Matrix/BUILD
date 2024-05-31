# Matrix Library

# TODO: Fix visibility of the libraries

# Libaries
cc_library(
    name = "zmatrix",
    srcs = ["zmatrix.cpp"],
    hdrs = ["zmatrix.hpp"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "zmatrix_test",
    size = "small",
    srcs = ["zmatrix_test.cpp"],
    deps = [
      "@googletest//:gtest_main",
      ":zmatrix",
    ],
)

cc_library(
    name = "case-matrix",
    srcs = ["case-matrix.cpp"],
    hdrs = ["case-matrix.hpp"],
    deps = [":zmatrix"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "case-matrix_test",
    size = "small",
    srcs = ["case-matrix_test.cpp"],
    deps = [
        "@googletest//:gtest_main",
        ":case-matrix",
    ],
)

cc_library(
    name = "pattern-matrix",
    srcs = ["pattern-matrix.cpp"],
    hdrs = ["pattern-matrix.hpp"],
    deps = [
        ":case-matrix",
        ":zmatrix",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "pattern-matrix_test",
    size = "small",
    srcs = ["pattern-matrix_test.cpp"],
    deps = [
      "@googletest//:gtest_main",
      ":pattern-matrix",
    ],
)

cc_test(
    name = "pattern-matrix_test-rearrangements",
    size = "large",
    srcs = ["pattern-matrix_test-rearrangements.cpp"],
    deps = [
      "@googletest//:gtest_main",
      ":pattern-matrix",
    ],
)
