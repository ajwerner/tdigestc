workspace(name = "ajwerner_tdigest")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

################################################################################
# go
################################################################################

http_archive(
    name = "io_bazel_rules_go",
    urls = ["https://github.com/bazelbuild/rules_go/releases/download/0.15.3/rules_go-0.15.3.tar.gz"],
    sha256 = "97cf62bdef33519412167fd1e4b0810a318a7c234f5f8dc4f53e2da86241c492",
)
http_archive(
    name = "bazel_gazelle",
    urls = ["https://github.com/bazelbuild/bazel-gazelle/releases/download/0.14.0/bazel-gazelle-0.14.0.tar.gz"],
    sha256 = "c0a5739d12c6d05b6c1ad56f2200cb0b57c5a70e03ebd2f7b87ce88cabf09c7b",
)
load("@io_bazel_rules_go//go:def.bzl", "go_rules_dependencies", "go_register_toolchains")
go_rules_dependencies()
go_register_toolchains()
load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")
gazelle_dependencies()

################################################################################
# python
################################################################################

git_repository(
    name = "io_bazel_rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "8b5d0683a7d878b28fffe464779c8a53659fc645",
)

load("@io_bazel_rules_python//python:pip.bzl", "pip_repositories", "pip_import")


pip_repositories()

pip_import(
    name = "my_deps",
    requirements = "//python:requirements.txt",
)

load("@my_deps//:requirements.bzl", "pip_install")
pip_install()

# Only needed for PIP support:


################################################################################
# javascript
################################################################################

git_repository(
    name = "build_bazel_rules_nodejs",
    remote = "https://github.com/bazelbuild/rules_nodejs.git",
    tag = "0.15.0", # check for the latest tag when you install
)

new_http_archive(
    name = 'emscripten_toolchain',
    url = 'https://github.com/kripken/emscripten/archive/1.38.13.tar.gz',
    build_file = 'emscripten-toolchain.BUILD',
    strip_prefix = "emscripten-1.38.13",
)

new_http_archive(
    name = 'emscripten_clang',
    url = 'https://s3.amazonaws.com/mozilla-games/emscripten/packages/llvm/tag/linux_64bit/emscripten-llvm-e1.38.13.tar.gz',
    build_file = 'emscripten-clang.BUILD',
    strip_prefix = "emscripten-llvm-e1.38.13",
)

load("@build_bazel_rules_nodejs//:package.bzl", "rules_nodejs_dependencies")
rules_nodejs_dependencies()

load("@build_bazel_rules_nodejs//:defs.bzl", "node_repositories", "yarn_install")

node_repositories(
    package_json = ["//js:package.json"],
    node_version = "8.12.0",
    yarn_version = "1.5.1",
)

yarn_install(
    name = "npm",
    package_json = "//js:package.json",
    yarn_lock = "//js:yarn.lock",
)
