from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl


class autogen_ctx_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/universal/autogen_ctx.h'

    ## struct _HPyContext_s {
    ##     const char *name;
    ##     void *_private;
    ##     int ctx_version;
    ##     HPy h_None;
    ##     ...
    ##     HPy (*ctx_Module_Create)(HPyContext *ctx, HPyModuleDef *def);
    ##     ...
    ## }

    def generate(self):
        lines = []
        w = lines.append
        w('struct _HPyContext_s {')
        w('    const char *name; // used just to make debugging and testing easier')
        w('    void *_private;   // used by implementations to store custom data')
        w('    int ctx_version;')
        for var in self.api.variables:
            w('    %s;' % self.declare_var(var))
        for func in self.api.functions:
            w('    %s;' % self.declare_func(func))
        w('};')
        return '\n'.join(lines)

    def declare_var(self, var):
        return toC(var.node)

    def declare_func(self, func):
        # e.g. "HPy (*ctx_Module_Create)(HPyContext *ctx, HPyModuleDef *def)"
        #
        # turn the function declaration into a function POINTER declaration
        newnode = deepcopy(func.node)
        newnode.type = c_ast.PtrDecl(type=newnode.type, quals=[])
        # fix the name of the function pointer
        typedecl = find_typedecl(newnode)
        typedecl.declname = func.ctx_name()
        return toC(newnode)


class autogen_ctx_def_h(AutoGenFile):
    PATH = 'hpy/universal/src/autogen_ctx_def.h'

    ## struct _HPyContext_s g_universal_ctx = {
    ##     .name = "...",
    ##     ._private = NULL,
    ##     .ctx_version = 1,
    ##     .h_None = {CONSTANT_H_NONE},
    ##     ...
    ##     .ctx_Module_Create = &ctx_Module_Create,
    ##     ...
    ## }

    def generate(self):
        lines = []
        w = lines.append
        w('struct _HPyContext_s g_universal_ctx = {')
        w('    .name = "HPy Universal ABI (CPython backend)",')
        w('    ._private = NULL,')
        w('    .ctx_version = 1,')
        w('    /* h_None & co. are initialized by init_universal_ctx() */')
        for func in self.api.functions:
            w('    .%s = &%s,' % (func.ctx_name(), func.ctx_name()))
        w('};')
        return '\n'.join(lines)
