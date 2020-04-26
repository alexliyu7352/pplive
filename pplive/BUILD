
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "pplive",
    srcs = glob(
        ['src/*.cc'],
    ),
    hdrs = glob(
        ['include/*.h'],
    ),
    copts = ["-Ipplive/include"],
    deps = [
        "@boost//:log",
        "@handy//:main",
        "@boost//:format",
        "@boost//:algorithm",
        "@com_github_open_source_parsers_jsoncpp//:jsoncpp"
    ],
    visibility=['//visibility:public']
)