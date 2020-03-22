
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "pplive",
    srcs = glob(
        ['src/*.cc'],
    ),
    hdrs = glob(
        ['include/*.h'],
    ),
    copts = ["-Iinclude"],
    deps = [
        "@boost//:log",
        "@handy//:main"
    ],
    visibility=['//visibility:public']
)