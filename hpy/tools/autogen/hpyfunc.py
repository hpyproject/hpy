from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC


class autogen_hpyfunc_declare_h(AutoGenFile):
    PATH = 'hpy/devel/include/common/autogen_hpyfunc_declare.h'

    ## #define _HPyFunc_DECLARE_HPyFunc_NOARGS(SYM)  \
    ##     static HPy SYM(HPyContext ctx, HPy self)

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            # declare a function named 'SYM' of the appropriate type
            funcdecl = hpyfunc.node.type.type
            symdecl = deepcopy(funcdecl)
            symdecl.type.declname = 'SYM'
            symdecl = toC(symdecl)
            #
            # generate a macro emitting 'symdecl'
            name = hpyfunc.base_name().upper()
            w(f'#define _HPyFunc_DECLARE_HPyFunc_{name}(SYM) static {symdecl}')
        w('')

        for hpyfunc in self.api.hpyfunc_typedefs:
            # generate the typedef for HPyFunc_{base_name}
            w(f'{toC(hpyfunc.node)};')

        return '\n'.join(lines)


class autogen_hpyfunc_trampoline_h(AutoGenFile):
    PATH = 'hpy/devel/include/universal/autogen_hpyfunc_trampolines.h'

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            base_name = hpyfunc.base_name().upper()
            if base_name in ['NOARGS', 'O', 'VARARGS', 'KEYWORDS']:
                continue
            #
            # generate the struct that will contain all parameters
            w('typedef struct {')
            params = hpyfunc.params()
            assert toC(params[0].type) == 'HPyContext'
            for param in params[1:]:
                node = self.fix_type_of_arg(param)
                w(f'    {toC(node)};')
            w('} _HPyFunc_args_%s;' % (base_name,))
            w('')
            #
            # generate the trampoline itself
            tramp_node = deepcopy(hpyfunc.node.type.type)
            tramp_node.type.declname = 'SYM'
            tramp_node = self.fix_type_of_arg(tramp_node)
            tramp_node.args.params = [self.fix_type_of_arg(p)
                                      for p in tramp_node.args.params[1:]]
            arg_names = [param.name for param in tramp_node.args.params]
            arg_names = ', '.join(arg_names)

            w(f'#define _HPyFunc_TRAMPOLINE_HPyFunc_{base_name}(SYM, IMPL) \\')
            w(f'    static {toC(tramp_node)} \\')
            w('    { \\')
            w(f'        _HPyFunc_args_{base_name} a = {{ {arg_names} }}; \\')
            w('        return _HPy_CallRealFunctionFromTrampoline( \\')
            w(f'            _ctx_for_trampolines, HPyFunc_{base_name},'
              ' IMPL, &a); \\')
            w('    }')
            w('')
        return '\n'.join(lines)

    def fix_type_of_arg(self, declnode):
        if toC(declnode.type) == 'HPy':
            declnode = deepcopy(declnode)
            declnode.type.type.names = ['cpy_PyObject']
            declnode.type = c_ast.PtrDecl(type=declnode.type, quals=[])
        return declnode


class autogen_ctx_call_i(AutoGenFile):
    PATH = 'hpy/universal/src/autogen_ctx_call.i'

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            name = hpyfunc.base_name()
            NAME = name.upper()
            if NAME in ['NOARGS', 'O', 'VARARGS', 'KEYWORDS']:
                continue
            #
            result = '_h2py'    # xxx hard-coded for now
            args = ['ctx']
            for param in hpyfunc.params()[1:]:
                if toC(param.type) == 'HPy':
                    args.append(f'_py2h(a->{param.name})')
                else:
                    args.append(f'a->{param.name}')
            args = ', '.join(args)
            #
            w(f'    case HPyFunc_{NAME}: {{')
            w(f'        HPyFunc_{name} f = (HPyFunc_{name})func;')
            w(f'        _HPyFunc_args_{NAME} *a = (_HPyFunc_args_{NAME}*)args;')
            w(f'        return {result}(f({args}));')
            w(f'    }}')
        return '\n'.join(lines)
