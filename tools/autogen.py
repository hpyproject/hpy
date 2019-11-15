"""
Parse public_api.h and generates various stubs around
"""
import attr
import re
from copy import deepcopy
import pycparser
from pycparser import c_ast
from pycparser.c_generator import CGenerator

def toC(node):
    return toC.gen.visit(node)
toC.gen = CGenerator()


@attr.s
class Function:
    _CTX_NAME = re.compile(r'^_?HPy_?')
    
    name = attr.ib()
    node = attr.ib(repr=False)

    def _find_typedecl(self, node):
        while not isinstance(node, c_ast.TypeDecl):
            node = node.type
        return node

    def ctx_name(self):
        return self._CTX_NAME.sub(r'ctx_', self.name)

    def ctx_decl(self):
        # turn the function declaration into a function POINTER declaration
        newnode = deepcopy(self.node)
        newnode.type = c_ast.PtrDecl(type=newnode.type, quals=[])
        # fix the name of the function pointer
        typedecl = self._find_typedecl(newnode)
        typedecl.declname = self.ctx_name()
        return toC(newnode)

    def trampoline_def(self):
        parts = []
        w = parts.append
        w('static inline')
        w(toC(self.node))
        w('{\n')
        w('    return ctx->%s' % self.ctx_name())
        w('(')
        params = [p.name for p in self.node.type.args.params]
        w(', '.join(params))
        w(');')
        w('\n}')
        return ' '.join(parts)


class FuncDeclVisitor(pycparser.c_ast.NodeVisitor):
    def __init__(self):
        self.functions = []
    
    def visit_Decl(self, node):
        if not isinstance(node.type, c_ast.FuncDecl):
            return
        name = node.name
        if not name.startswith('HPy') and not name.startswith('_HPy'):
            print('WARNING: Ignoring non-hpy declaration: %s' % name)
            return
        self.functions.append(Function(name, node))


class AutoGen:

    def __init__(self, filename):
        self.ast = pycparser.parse_file(filename, use_cpp=True)
        self.ast.show()
        self.collect_functions()

    def collect_functions(self):
        v = FuncDeclVisitor()
        v.visit(self.ast)
        self.functions = v.functions

    def gen_ctx(self):
        lines = []
        w = lines.append
        w('struct _HPyContext_s {')
        w('    int ctx_version;')
        for f in self.functions:
            w('    %s;' % f.ctx_decl())
        w('};')
        return '\n'.join(lines)

    def gen_func_trampolines(self):
        lines = []
        for f in self.functions:
            lines.append(f.trampoline_def())
            lines.append('')
        return '\n'.join(lines)


def main():
    autogen = AutoGen('public_api.h')
    for func in autogen.functions:
        print(func)
    print()
    print(autogen.gen_ctx())
    print()
    print(autogen.gen_func_trampolines())

if __name__ == '__main__':
    main()
