

config_setting(
    name = "osx",
    constraint_values = [
        "@bazel_tools//platforms:osx",
    ],
)

config_setting(
    name = "ios",
    constraint_values = [
        "@bazel_tools//platforms:ios",
    ],
)

config_setting(
    name = "linux",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
)

cc_library(
    name = "main",
    srcs = glob(
        ['handy/**']
    ),
    hdrs = glob(
        ['handy/**']
    ),
    local_defines=select({'linux':["OS_LINUX"], 'osx': ["OS_MACOSX"]}),
    visibility=['//visibility:public']
)
