def _impl(ctx):
    # https://github.com/bazelbuild/rules_scala/pull/286/files
    ctx.actions.run(
        inputs = [ctx.attr.lib.java.outputs.native_headers],
        tools = [ctx.executable._zipper],
        outputs = ctx.outputs.outs,
        executable = ctx.executable._zipper.path,
        arguments = ["vxf", ctx.attr.lib.java.outputs.native_headers.path, "-d", ctx.outputs.outs[0].dirname],
    )

extract_native_header_jar = rule(
    implementation=_impl,
    attrs={
        "lib": attr.label(mandatory=True, single_file=True),
        "outs": attr.output_list(),
        # https://github.com/bazelbuild/bazel/issues/2414
        "_zipper": attr.label(
            executable=True, 
            cfg="host", 
            default=Label("@bazel_tools//tools/zip:zipper"), 
            allow_files=True,
        )
    },
    output_to_genfiles = True,
)
