from copy import deepcopy
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl

NO_CALL = ('NOARGS', 'O', 'VARARGS', 'KEYWORDS', 'INITPROC', 'DESTROYFUNC',
    'GETBUFFERPROC', 'RELEASEBUFFERPROC')
NO_TRAMPOLINE = NO_CALL + ('RICHCMPFUNC',)

class autogen_hpyfunc_declare_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/autogen_hpyfunc_declare.h'

    ## #define _HPyFunc_DECLARE_HPyFunc_NOARGS(SYM)  \
    ##     static HPy SYM(HPyContext *ctx, HPy self)

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


def hpy_to_cpy(declnode):
    if toC(declnode.type) == 'HPy':
        declnode = deepcopy(declnode)
        declnode.type.type.names = ['cpy_PyObject']
        declnode.type = c_ast.PtrDecl(type=declnode.type, quals=[])
    return declnode


class autogen_hpyfunc_trampoline_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/universal/autogen_hpyfunc_trampolines.h'

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            NAME = hpyfunc.base_name().upper()
            if NAME in NO_TRAMPOLINE:
                continue
            #
            tramp_node = deepcopy(hpyfunc.node.type.type)
            tramp_node.type.declname = 'SYM'
            tramp_node = hpy_to_cpy(tramp_node)
            assert toC(tramp_node.args.params[0].type) == 'HPyContext *'
            tramp_node.args.params = [hpy_to_cpy(p)
                                      for p in tramp_node.args.params[1:]]
            for i, param in enumerate(tramp_node.args.params):
                typedecl = find_typedecl(param.type)
                if typedecl.declname is None:
                    param.name = 'arg%d' % i
                    typedecl.declname = 'arg%d' % i
            arg_names = [param.name for param in tramp_node.args.params]
            arg_names = ', '.join(arg_names)
            #
            # generate the struct that will contain all parameters
            w(f'typedef struct {{')
            for param in tramp_node.args.params:
                w(f'    {toC(param)};')
            if toC(tramp_node.type) != 'void':
                w(f'    {toC(tramp_node.type)} result;')
            w(f'}} _HPyFunc_args_{NAME};')
            w('')
            #
            # generate the trampoline itself
            w(f'#define _HPyFunc_TRAMPOLINE_HPyFunc_{NAME}(SYM, IMPL) \\')
            w(f'    static {toC(tramp_node)} \\')
            w(f'    {{ \\')
            w(f'        _HPyFunc_args_{NAME} a = {{ {arg_names} }}; \\')
            w(f'        _HPy_CallRealFunctionFromTrampoline( \\')
            w(f'           _ctx_for_trampolines, HPyFunc_{NAME}, IMPL, &a); \\')
            if toC(tramp_node.type) == 'void':
                w(f'        return; \\')
            else:
                w(f'        return a.result; \\')
            w(f'    }}')
            w('')
        return '\n'.join(lines)


class autogen_ctx_call_i(AutoGenFile):
    PATH = 'hpy/universal/src/autogen_ctx_call.i'

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            name = hpyfunc.base_name()
            NAME = name.upper()
            if NAME in NO_CALL:
                continue
            #
            c_ret_type = toC(hpyfunc.return_type())
            args = ['ctx']
            for i, param in enumerate(hpyfunc.params()[1:]):
                pname = param.name
                if pname is None:
                    pname = 'arg%d' % i
                if toC(param.type) == 'HPy':
                    args.append(f'_py2h(a->{pname})')
                else:
                    args.append(f'a->{pname}')
            args = ', '.join(args)
            #
            w(f'    case HPyFunc_{NAME}: {{')
            w(f'        HPyFunc_{name} f = (HPyFunc_{name})func;')
            w(f'        _HPyFunc_args_{NAME} *a = (_HPyFunc_args_{NAME}*)args;')
            if c_ret_type == 'void':
                w(f'        f({args});')
            elif c_ret_type == 'HPy':
                w(f'        a->result = _h2py(f({args}));')
            else:
                w(f'        a->result = f({args});')
            w(f'        return;')
            w(f'    }}')
        return '\n'.join(lines)


class autogen_cpython_hpyfunc_trampoline_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/cpython/autogen_hpyfunc_trampolines.h'

    def generate(self):
        lines = []
        w = lines.append
        for hpyfunc in self.api.hpyfunc_typedefs:
            name = hpyfunc.base_name()
            NAME = name.upper()
            if NAME in NO_TRAMPOLINE:
                continue
            #
            tramp_node = deepcopy(hpyfunc.node.type.type)
            tramp_node.type.declname = 'SYM'
            tramp_node = hpy_to_cpy(tramp_node)
            tramp_node.args.params = [hpy_to_cpy(p)
                                      for p in tramp_node.args.params[1:]]
            for i, param in enumerate(tramp_node.args.params):
                typedecl = find_typedecl(param.type)
                if typedecl.declname is None:
                    param.name = 'arg%d' % i
                    typedecl.declname = 'arg%d' % i

            if toC(hpyfunc.return_type()) == 'HPy':
                result = '_h2py'
            else:
                result = ''
            args = ['_HPyGetContext()']
            for i, param in enumerate(hpyfunc.params()[1:]):
                pname = param.name
                if pname is None:
                    pname = 'arg%d' % i
                if toC(param.type) == 'HPy':
                    args.append(f'_py2h({pname})')
                else:
                    args.append(f'{pname}')
            args = ', '.join(args)
            #
            w(f'#define _HPyFunc_TRAMPOLINE_HPyFunc_{NAME}(SYM, IMPL) \\')
            w(f'    static {toC(tramp_node)} \\')
            w(f'    {{ \\')
            w(f'        return {result}(IMPL({args})); \\')
            w(f'    }}')
        return '\n'.join(lines)
