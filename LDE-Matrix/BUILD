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
      ":lde-matrix-test-utils",
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
    name = "patterns928",
    srcs = ["data/patterns928.cpp"],
    hdrs = ["data/patterns928.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "pattern-matrix",
    srcs = ["pattern-matrix.cpp"],
    hdrs = ["pattern-matrix.hpp"],
    deps = [
        ":patterns928",
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
      ":lde-matrix-test-utils",
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

cc_library(
    name = "pattern-deduper",
    srcs = ["pattern-deduper.cpp"],
    hdrs = ["pattern-deduper.hpp"],
    deps = [":pattern-matrix"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "pattern-deduper_test",
    size = "small",
    srcs = ["pattern-deduper_test.cpp"],
    deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":pattern-deduper",
      ":lde-matrix-test-utils",
    ],
)

cc_library(
    name = "lde-matrix-test-utils",
    srcs = ["test-utils.cpp"],
    hdrs = ["test-utils.hpp"],
    deps = [
        ":zmatrix",
        ":pattern-matrix",
        ":case-matrix",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lde-matrix-run-utils",
    srcs = ["run-utils.cpp"],
    hdrs = ["run-utils.hpp"],
    deps = [
        ":zmatrix",
        ":pattern-matrix",
        ":case-matrix",
        ":pattern-deduper",
    ],
    visibility = ["//visibility:public"],
)