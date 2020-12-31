from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl

def get_debug_signature(func):
    newnode = deepcopy(func.node)
    typedecl = find_typedecl(newnode)
    # rename the function
    typedecl.declname = 'debug_%s' % func.ctx_name()
    return toC(newnode)

class autogen_debug_ctx_def_h(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_ctx.h'

    def generate(self):
        lines = []
        w = lines.append
        # emit the declarations for all the debug_ctx_* functions
        for func in self.api.functions:
            w(get_debug_signature(func) + ';')

        # emit a static ctx which uses the various debug_ctx_* functions
        w('')
        w('static struct _HPyContext_s g_debug_ctx = {')
        w('    .name = "HPy Debug Mode ABI",')
        w('    ._private = NULL,')
        w('    .ctx_version = 1,')
        for var in self.api.variables:
            w('    .%s = HPy_NULL,' % (var.name,))
        for func in self.api.functions:
            w('    .%s = &debug_%s,' % (func.ctx_name(), func.ctx_name()))
        w('};')
        return '\n'.join(lines)


class autogen_debug_wrappers(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_wrappers.c'

    NO_WRAPPER = set([
        '_HPy_CallRealFunctionFromTrampoline',
        ])

    def generate(self):
        lines = []
        w = lines.append
        w('#include "debug_internal.h"')
        w('')
        for func in self.api.functions:
            debug_wrapper = self.gen_debug_wrapper(func)
            if debug_wrapper:
                w(debug_wrapper)
                w('')
        return '\n'.join(lines)

    def gen_debug_wrapper(self, func):
        if func.name in self.NO_WRAPPER:
            return
        #
        assert not func.is_varargs()
        def get_params():
            lst = [p.name for p in func.node.type.args.params]
            assert lst[0] == 'ctx'
            lst[0] = 'get_info(ctx)->original_ctx'
            return ', '.join(lst)
        #
        params = get_params()
        rettype = toC(func.node.type.type)
        #
        lines = []
        w = lines.append
        w(get_debug_signature(func))
        w('{')
        if rettype == 'void':
            w(f'    {func.name}({params});')
        else:
            w(f'    return {func.name}({params});')
        w('}')
        return '\n'.join(lines)
