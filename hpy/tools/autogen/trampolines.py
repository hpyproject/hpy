from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl


class autogen_trampolines_h(AutoGenFile):
    PATH = 'hpy/devel/include/universal/autogen_trampolines.h'

    NO_TRAMPOLINES = set([
        '_HPy_New',
        'HPy_FatalError',
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


class autogen_impl_h(AutoGenFile):
    PATH = 'hpy/devel/include/common/autogen_impl.h'

    def generate(self):
        lines = []
        for func in self.api.functions:
            if not func.cpython_name:
                continue
            lines.append(self.gen_implementation(func))
            lines.append('')
        return '\n'.join(lines)

    def gen_implementation(self, func):
        def signature(base_name):
            # HPy _HPy_API_NAME(Number_Add)(HPyContext ctx, HPy x, HPy y)
            newnode = deepcopy(func.node)
            typedecl = find_typedecl(newnode)
            # rename the function
            if func.name.startswith('HPy_'):
                typedecl.declname = '_HPy_IMPL_NAME_NOPREFIX(%s)' % base_name
            else:
                typedecl.declname = '_HPy_IMPL_NAME(%s)' % base_name
            return toC(newnode)
        #
        def call(pyfunc, return_type):
            # return _py2h(PyNumber_Add(_h2py(x), _h2py(y)))
            args = []
            for p in func.node.type.args.params:
                if toC(p.type) == 'HPyContext':
                    continue
                elif toC(p.type) == 'HPy':
                    arg = '_h2py(%s)' % p.name
                else:
                    arg = p.name
                args.append(arg)
            result = '%s(%s)' % (pyfunc, ', '.join(args))
            if return_type == 'HPy':
                result = '_py2h(%s)' % result
            return result
        #
        lines = []
        w = lines.append
        pyfunc = func.cpython_name
        if not pyfunc:
            raise ValueError(f"Cannot generate implementation for {self}")
        return_type = toC(func.node.type.type)
        return_stmt = '' if return_type == 'void' else 'return '
        w('HPyAPI_STORAGE %s' % signature(func.base_name()))
        w('{')
        w('    %s%s;' % (return_stmt, call(pyfunc, return_type)))
        w('}')
        return '\n'.join(lines)
