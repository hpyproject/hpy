from copy import deepcopy
import textwrap
from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC, find_typedecl

class HPy_2_DHPy_Visitor(c_ast.NodeVisitor):
    "Visitor which renames all HPy types to DHPy"

    def visit_IdentifierType(self, node):
        if node.names == ['HPy']:
            node.names = ['DHPy']

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

class autogen_debug_ctx_def_h(AutoGenFile):
    PATH = 'hpy/debug/src/autogen_debug_ctx.h'

    def generate(self):
        lines = []
        w = lines.append
        w(textwrap.dedent("""
        /*
        The debug_ctx_* functions contain the actualy logic: they receive and
        return handles of type DHPy.

        The _adapter_debug_* helpers cast DHPy into HPy and viceversa, to get
        functions those signature is compatible to what is declared in the
        HPyContext. Note that they are no-op, since the internal repr of DHPy
        and HPy is the same (but the first is seen as a pointer to DHPy_s the
        second as a small struct containining an integer), but the C standard
        forbids casting function pointers whose arguments have nominally
        different types, so we need to write the adapters manually.
        */
        """))
        # emit the declarations and adapters for all the debug_ctx_* functions
        for func in self.api.functions:
            self.generate_adapter(w, func)

        # emit a static ctx which uses the various debug_ctx_* functions
        w('')
        w('static struct _HPyContext_s g_debug_ctx = {')
        w('    .name = "HPy Debug Mode ABI",')
        w('    ._private = NULL,')
        w('    .ctx_version = 1,')
        for var in self.api.variables:
            w('    .%s = HPy_NULL,' % (var.name,))
        for func in self.api.functions:
            w('    .%s = &_adapter_debug_%s,' % (func.ctx_name(), func.ctx_name()))
        w('};')
        return '\n'.join(lines)

    def generate_adapter(self, w, func):
        name = 'debug_%s' % func.ctx_name()
        wrapper_node = get_debug_wrapper_node(func)
        w(toC(wrapper_node) + ';') # signature of the debug_ctx_* function
        #
        # emit the adapter
        node = funcnode_with_new_name(func.node, '_adapter_%s' % name)
        signature = toC(node)
        rettype = toC(node.type.type)
        def get_params():
            lst = []
            for p in node.type.args.params:
                if toC(p.type) == 'HPy':
                    lst.append('_h2d(%s)' % p.name)
                elif toC(p.type) in ('HPy *', 'HPy []'):
                    lst.append('(DHPy *)%s' % p.name)
                else:
                    lst.append(p.name)
            return ', '.join(lst)
        params = get_params()

        w(signature)
        w('{')
        if rettype == 'void':
            w(f'    {name}({params});')
        elif rettype == 'HPy':
            w(f'    return _d2h({name}({params}));')
        else:
            w(f'    return {name}({params});')
        w('}')
        w('')


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
        node = get_debug_wrapper_node(func)
        signature = toC(node)
        rettype = toC(node.type.type)
        #
        def get_params():
            lst = []
            for p in node.type.args.params:
                if p.name == 'ctx':
                    lst.append('get_info(ctx)->original_ctx')
                elif toC(p.type) == 'DHPy':
                    lst.append('%s->h' % p.name)
                elif toC(p.type) in ('DHPy *', 'DHPy []'):
                    lst.append('(HPy *)%s' % p.name)
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
            w(f'    return _h2d({func.name}({params}));')
        else:
            w(f'    return {func.name}({params});')
        w('}')
        return '\n'.join(lines)
