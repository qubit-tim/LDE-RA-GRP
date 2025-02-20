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
    name = "lde-ming-testing",
    srcs = ["ming-testing.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:pattern-deduper",
        "//LDE-Matrix:lde-matrix-run-utils",
    ],
    data = [
        ":patterns",
        ":ming-output",
        ":user-output",
    ],
)

cc_binary(
    name = "lde-tfc-testing",
    srcs = ["tfc-testing.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:pattern-deduper",
        "//LDE-Matrix:lde-matrix-run-utils",
    ],
    data = [
        ":patterns",
        ":tfc-output",
        ":user-output",
    ],
)

cc_binary(
    name = "lde-tfc-single-testing",
    srcs = ["tfc-single-testing.cpp"],
    deps = [
        "//LDE-Matrix:pattern-matrix",
        "//LDE-Matrix:pattern-deduper",
        "//LDE-Matrix:possible-pattern-matrix",
        "//LDE-Matrix:possible-pattern-deduper",
        "//LDE-Matrix:lde-matrix-run-utils",
    ],
    data = [
        ":patterns",
        ":tfc-output",
        ":user-output",
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
        ":tfc-output",
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

filegroup(
        name = 'tfc-output',
        srcs = glob(['tfc-output/**'], allow_empty=True)
)

filegroup(
        name = 'ming-output',
        srcs = glob(['ming-output/**'], allow_empty=True)
)

filegroup(
        name = 'user-output',
        srcs = glob(['user-output/**'], allow_empty=True)
)