package(default_visibility = ["//visibility:public"])

cc_library(
    name = "kdtree-lib",
    hdrs = [
            "kdtree.h",
            "point.h",
            ],
)

cc_binary(
    name = "kdtree-demo",
    srcs = ["demo.cc"],
    deps = [":kdtree-lib"],
)

cc_test(
    name = "kdtree-test",
    srcs = ["test_kdtree.cc"],
    deps = [":kdtree-lib"],
)
