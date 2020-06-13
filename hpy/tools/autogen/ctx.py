from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC


class autogen_ctx_h(AutoGenFile):
    PATH = 'hpy/devel/include/autogen_ctx.h'

    ## struct _HPyContext_s {
    ##     int ctx_version;
    ##     HPy h_None;
    ##     ...
    ##     HPy (*ctx_Module_Create)(HPyContext ctx, HPyModuleDef *def);
    ##     ...
    ## }

    def generate(self):
        lines = []
        w = lines.append
        w('struct _HPyContext_s {')
        w('    int ctx_version;')
        for var in self.api.variables:
            w('    %s;' % self.declare_var(var))
        for func in self.api.functions:
            w('    %s;' % self.declare_func(func))
        w('};')
        return '\n'.join(lines)

    def declare_func(self, func):
        # e.g. "HPy (*ctx_Module_Create)(HPyContext ctx, HPyModuleDef *def)"
        #
        # turn the function declaration into a function POINTER declaration
        newnode = deepcopy(func.node)
        newnode.type = c_ast.PtrDecl(type=newnode.type, quals=[])
        # fix the name of the function pointer
        typedecl = func._find_typedecl(newnode)
        typedecl.declname = func.ctx_name()
        return toC(newnode)

    def declare_var(self, var):
        return toC(var.node)
