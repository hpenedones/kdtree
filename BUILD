package(default_visibility = ["//visibility:public"])

cc_library(
    name = "kdtree-lib",
    srcs = [
            "kdtree.cc",
            ],
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