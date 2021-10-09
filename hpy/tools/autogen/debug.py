from copy import deepcopy
import textwrap
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl
from .hpyfunc import NO_CALL

class HPy_2_DHPy_Visitor(c_ast.NodeVisitor):
    "Visitor which renames all HPy types to DHPy"

    def visit_IdentifierType(self, node):
        if node.names == ['HPy']:
            node.names = ['DHPy']

    def visit_TypeDecl(self, node):
        if node.declname == 'ctx':
            node.declname = 'dctx'
        self.generic_visit(node)

def funcnode_with_new_name(node, name):
    newnode = deepcopy(node)
    typedecl = find_typedecl(newnode)
    typedecl.declname = name
    return newnode

def get_debug_wrapper_node(func):
    newnode = funcnode_with_new_name(func.node, 'debug_%s' % func.ctx_name())
    # fix all the types
    visitor = HPy_2_DHPy_Visitor()
    visitor.visit(newnode)
    return newnode


class autogen_debug_ctx_init_h(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_ctx_init.h'

    def generate(self):
        lines = []
        w = lines.append
        # emit the declarations for all the debug_ctx_* functions
        for func in self.api.functions:
            w(toC(get_debug_wrapper_node(func)) + ';')
        w('')
        w('static inline void debug_ctx_init_fields(HPyContext *dctx, HPyContext *uctx)')
        w('{')
        for var in self.api.variables:
            name = var.name
            w(f'    dctx->{name} = DHPy_open(dctx, uctx->{name});')
        for func in self.api.functions:
            name = func.ctx_name()
            w(f'    dctx->{name} = &debug_{name};')

        w('}')
        return '\n'.join(lines)


class autogen_debug_wrappers(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_wrappers.c'

    NO_WRAPPER = set([
        '_HPy_CallRealFunctionFromTrampoline',
        'HPy_Close',
        'HPyTuple_FromArray',
        'HPyType_GenericNew',
        'HPyType_FromSpec',
        'HPyTracker_New',
        'HPyTracker_Add',
        'HPyTracker_ForgetAll',
        'HPyTracker_Close',
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
        node = get_debug_wrapper_node(func)
        signature = toC(node)
        rettype = toC(node.type.type)
        #
        def get_params():
            lst = []
            for p in node.type.args.params:
                if p.name == 'ctx':
                    lst.append('get_info(dctx)->uctx')
                elif toC(p.type) == 'DHPy':
                    lst.append('DHPy_unwrap(dctx, %s)' % p.name)
                elif toC(p.type) in ('DHPy *', 'DHPy []'):
                    assert False, ('C type %s not supported, please write the wrapper '
                                   'for %s by hand' % (toC(p.type), func.name))
                else:
                    lst.append(p.name)
            return ', '.join(lst)
        params = get_params()
        #
        lines = []
        w = lines.append
        w(signature)
        w('{')
        if rettype == 'void':
            w(f'    {func.name}({params});')
        elif rettype == 'DHPy':
            w(f'    return DHPy_open(dctx, {func.name}({params}));')
        else:
            w(f'    return {func.name}({params});')
        w('}')
        return '\n'.join(lines)


class autogen_debug_ctx_call_i(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_ctx_call.i'

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
            args = ['dctx']
            dhpys = []
            for i, param in enumerate(hpyfunc.params()[1:]):
                pname = param.name
                if pname is None:
                    pname = 'arg%d' % i
                if toC(param.type) == 'HPy':
                    dhpys.append(pname)
                    args.append(f'dh_{pname}')
                else:
                    args.append(f'a->{pname}')
            args = ', '.join(args)
            #
            w(f'    case HPyFunc_{NAME}: {{')
            w(f'        HPyFunc_{name} f = (HPyFunc_{name})func;')
            w(f'        _HPyFunc_args_{NAME} *a = (_HPyFunc_args_{NAME}*)args;')
            for pname in dhpys:
                w(f'        DHPy dh_{pname} = _py2dh(dctx, a->{pname});')
            #
            if c_ret_type == 'void':
                w(f'        f({args});')
            elif c_ret_type == 'HPy':
                w(f'        DHPy dh_result = f({args});')
                w(f'        a->result = _dh2py(dctx, dh_result);')
                dhpys.append('result')
            else:
                w(f'        a->result = f({args});')
            #
            for pname in dhpys:
                w(f'        DHPy_close(dctx, dh_{pname});')
            #
            w(f'        return;')
            w(f'    }}')
        return '\n'.join(lines)
