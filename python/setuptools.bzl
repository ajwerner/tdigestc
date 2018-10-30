

load("@my_deps//:requirements.bzl", "requirement")


def _setuptools_gen(ctx):
    out = ctx.actions.declare_file("tdigest/dist/tdigestc-"+ctx.attr.version+".linux-x86_64.tar.gz")
    print( ["bdist", "-d", "{}".format(out.dirname), "--verbose"])
    ctx.actions.run_shell(
        
        inputs = ctx.files.data,
        tools = [ctx.executable.setup_bin],
        command = " ".join([ctx.executable.setup_bin.path, "bdist", "-d", "{}".format( out.dirname), "--verbose"]) + "",
        outputs= [ out ]
    )
    print(out.path)
    return [
        DefaultInfo(files=depset([out])),
    ]

setuptools_gen = rule(
    implementation = _setuptools_gen,
    fragments = ["py", "cpp"],
    host_fragments = ["py", "cpp"],
    attrs = {
        "setup_bin": attr.label(
            cfg="host",
            executable = True,
        ),
        "data": attr.label_list(),
        "version": attr.string(),
    })

def setuptools_pkg(name,
                   lib,
                   setup_file,
                   version,
                   **kwargs):
    setup_name = name+".setup"
    native.py_binary(
        name=setup_name,
        srcs=[setup_file],
        main=setup_file,
        deps = [
            requirement("setuptools"),
            requirement("wheel"),
        ],
        data = [
            lib,
        ],
        **kwargs
    )
    setuptools_gen(
        name = name+".package",
        setup_bin = setup_name,
        data = [lib],
        version=version,
    )

