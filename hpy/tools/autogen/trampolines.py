from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC


class autogen_trampolines_h(AutoGenFile):
    PATH = 'hpy/devel/include/universal/autogen_trampolines.h'

    NO_TRAMPOLINES = set([
        '_HPy_New',
        ])

    def generate(self):
        lines = []
        for func in self.api.functions:
            trampoline = self.gen_trampoline(func)
            if trampoline:
                lines.append(trampoline)
                lines.append('')
        return '\n'.join(lines)

    def gen_trampoline(self, func):
        # static inline HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def) {
        #      return ctx->ctx_Module_Create ( ctx, def );
        # }
        if func.name in self.NO_TRAMPOLINES:
            return None
        rettype = toC(func.node.type.type)
        parts = []
        w = parts.append
        w('static inline')
        w(toC(func.node))
        w('{\n    ')

        # trampolines cannot deal with varargs easily
        assert not func.is_varargs()

        if rettype == 'void':
            w('ctx->%s' % func.ctx_name())
        else:
            w('return ctx->%s' % func.ctx_name())
        w('(')
        params = [p.name for p in func.node.type.args.params]
        w(', '.join(params))
        w(');')

        w('\n}')
        return ' '.join(parts)
