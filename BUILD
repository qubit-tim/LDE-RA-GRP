cc_binary(
    name = "lde-main",
    srcs = ["main.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
    ],
    data = [
        ":patterns",
    ],
)

cc_binary(
    name = "lde-pattern-generator",
    srcs = ["pattern-generator.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:zmatrix",
    ],
)

cc_binary(
    name = "lde-subcase-matching",
    srcs = ["subcase-matching.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:zmatrix",
    ],
    data = [
        ":patterns",
        ":matched-cases",
    ],
)

cc_binary(
    name = "lde-transition-testing",
    srcs = ["transition-testing.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:zmatrix",
    ],
)

filegroup(
        name = 'patterns',
        srcs = glob(['patterns/**'])
)

filegroup(
        name = 'matched-cases',
        srcs = glob(['matched-cases/**'])
)
