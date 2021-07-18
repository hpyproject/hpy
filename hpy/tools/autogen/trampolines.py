from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl


class autogen_trampolines_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/universal/autogen_trampolines.h'

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
        # HPyAPI_FUNC HPy HPyModule_Create(HPyContext *ctx, HPyModuleDef *def) {
        #      return ctx->ctx_Module_Create ( ctx, def );
        # }
        if func.name in self.NO_TRAMPOLINES:
            return None
        rettype = toC(func.node.type.type)
        parts = []
        w = parts.append
        w('HPyAPI_FUNC')
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


class cpython_autogen_api_impl_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/cpython/autogen_api_impl.h'

    def signature(self, func):
        """
        Return the C signature of the impl function.

        In CPython mode, the name it's the same as in public_api:
           HPy_Add          ==> HPyAPI_FUNC HPy_Add
           HPyLong_FromLong ==> HPyAPI_FUNC HPyLong_FromLong

        See also universal_autogen_ctx_impl_h.
        """
        sig = toC(func.node)
        return 'HPyAPI_FUNC %s' % sig

    def generate(self):
        lines = []
        for func in self.api.functions:
            if not func.cpython_name:
                continue
            lines.append(self.gen_implementation(func))
            lines.append('')
        return '\n'.join(lines)

    def gen_implementation(self, func):
        def call(pyfunc, return_type):
            # return _py2h(PyNumber_Add(_h2py(x), _h2py(y)))
            args = []
            for p in func.node.type.args.params:
                if toC(p.type) == 'HPyContext *':
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
        w(self.signature(func))
        w('{')
        w('    %s%s;' % (return_stmt, call(pyfunc, return_type)))
        w('}')
        return '\n'.join(lines)


class universal_autogen_ctx_impl_h(cpython_autogen_api_impl_h):
    PATH = 'hpy/universal/src/autogen_ctx_impl.h'

    def signature(self, func):
        """
        Return the C signature of the impl function.

        In Universal mode, the name is prefixed by ctx_:
           HPy_Add          ==> HPyAPI_IMPL ctx_Add
           HPyLong_FromLong ==> HPyAPI_IMPL ctx_Long_FromLong

        See also cpython_autogen_api_impl_h.
        """
        newnode = deepcopy(func.node)
        typedecl = find_typedecl(newnode)
        # rename the function
        typedecl.declname = func.ctx_name()
        sig = toC(newnode)
        return 'HPyAPI_IMPL %s' % sig
