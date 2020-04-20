# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "pprtmp_server",
    srcs = glob(
        ['src/*.cc'],
    ),
    hdrs = glob(
        ['include/*.h'],
    ),
    copts = ["-Ipplive_rtmp/include"],
    deps = [
        "@boost//:log",
        "@boost//:format",
        "@com_github_open_source_parsers_jsoncpp//:jsoncpp",
        "//pplive:pplive"
    ],
    visibility=['//visibility:public']
)
