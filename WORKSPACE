load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1570056263 -0700",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

new_git_repository(
    name="handy",
    commit="23b419c92736fef7aa471a91d4d638ef1d348b7e",
    remote="https://github.com/IcyCC/handy.git",
    build_file="@//:third_party/handy.BUILD"
)


git_repository(
    name = "org_cloudabi_bazel_third_party",
    commit = "91ca2167219c612a89334fa09ddf15fbdc5d0592",
    remote = "https://github.com/NuxiNL/bazel-third-party.git",
)
load("@org_cloudabi_bazel_third_party//:third_party.bzl", "third_party_repositories")
third_party_repositories()


