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

filegroup(
        name = 'patterns',
        srcs = glob(['patterns/*'])
)
