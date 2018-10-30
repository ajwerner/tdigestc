VERSION="0.0.7"

def _write_version(ctx):
    print(VERSION)
    ctx.actions.expand_template(template = ctx.file.template,
                                output = ctx.outputs.out,
                                substitutions = {"{VERSION}": VERSION})
    return [
        DefaultInfo(files=depset([ctx.outputs.out])),
    ]
 
_rule_write_version = rule(
    implementation=_write_version,
    attrs={
        "template": attr.label(
            allow_single_file = True,
        ),
        "out": attr.string(),
    },
    outputs={"out": "%{out}"},
)

def write_version(template, template_suffix=".tmpl"):
    _rule_write_version(
        name = template + "_write_version",
        template = template,
        out = template[:template.rindex(template_suffix)]
    )
    
