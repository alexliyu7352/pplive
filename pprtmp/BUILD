# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "pprtmp_lib",
    srcs = glob(
        ['src/*.cc'],
    ),
    hdrs = glob(
        ['include/*.h', 'include/*.hpp'],
    ),
    copts = ["-Ipprtmp/include"],
    deps = [
        "@boost//:format",
        "@com_github_open_source_parsers_jsoncpp//:jsoncpp",
    ],
    visibility=['//visibility:public']
)

