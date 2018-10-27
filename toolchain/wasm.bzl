def wasm_library(name, srcs, deps = [], **kwargs):
    native.cc_binary(
        name = name+".js.tar.gz",
        deps = deps,
        srcs = srcs,
        **kwargs)
    native.genrule(
        name = name,
        srcs = [name+".js.tar.gz"],
        outs = [name+".js", name+".wasm"],
        cmd = "set -x; tar -tvf $(SRCS);  tar -xf $(SRCS) -C $(@D)",
    )

