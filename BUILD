# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "server",
    srcs = ["server_main.cpp"],
    copts = [""],
    deps = [
        "//pplive:pplive",
        "//pprtmp:pprtmp_lib",
        ]
)

cc_binary(
    name = "client",
    srcs = ["client_main.cpp"],
    copts = [""],
    deps = ["//pplive:pplive",
            "@boost//:format",
            "//pprtmp:pprtmp_lib"]
)