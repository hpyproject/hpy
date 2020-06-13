from copy import deepcopy
import attr
import re
import pycparser
from pycparser import c_ast
from pycparser.c_generator import CGenerator

def toC(node):
    return toC.gen.visit(node)
toC.gen = CGenerator()



@attr.s
class Function:
    _BASE_NAME = re.compile(r'^_?HPy_?')

    name = attr.ib()
    cpython_name = attr.ib()
    node = attr.ib(repr=False)

    def _find_typedecl(self, node):
        while not isinstance(node, c_ast.TypeDecl):
            node = node.type
        return node

    def base_name(self):
        return self._BASE_NAME.sub('', self.name)

    def ctx_name(self):
        # e.g. "ctx_Module_Create"
        return self._BASE_NAME.sub(r'ctx_', self.name)

    def is_varargs(self):
        return (len(self.node.type.args.params) > 0 and
                isinstance(self.node.type.args.params[-1], c_ast.EllipsisParam))


    def trampoline_def(self):
        # static inline HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *def) {
        #      return ctx->ctx_Module_Create ( ctx, def );
        # }
        if self.name in NO_TRAMPOLINES:
            return None
        rettype = toC(self.node.type.type)
        parts = []
        w = parts.append
        w('static inline')
        w(toC(self.node))
        w('{\n    ')

        # trampolines cannot deal with varargs easily
        assert not self.is_varargs()

        if rettype == 'void':
            w('ctx->%s' % self.ctx_name())
        else:
            w('return ctx->%s' % self.ctx_name())
        w('(')
        params = [p.name for p in self.node.type.args.params]
        w(', '.join(params))
        w(');')

        w('\n}')
        return ' '.join(parts)

    def implementation(self):
        def signature(base_name):
            # HPy _HPy_API_NAME(Number_Add)(HPyContext ctx, HPy x, HPy y)
            newnode = deepcopy(self.node)
            typedecl = self._find_typedecl(newnode)
            # rename the function
            if self.name.startswith('HPy_'):
                typedecl.declname = '_HPy_IMPL_NAME_NOPREFIX(%s)' % base_name
            else:
                typedecl.declname = '_HPy_IMPL_NAME(%s)' % base_name
            return toC(newnode)
        #
        def call(pyfunc, return_type):
            # return _py2h(PyNumber_Add(_h2py(x), _h2py(y)))
            args = []
            for p in self.node.type.args.params:
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
        pyfunc = self.cpython_name
        if not pyfunc:
            raise ValueError(f"Cannot generate implementation for {self}")
        return_type = toC(self.node.type.type)
        w('HPyAPI_STORAGE %s' % signature(self.base_name()))
        w('{')
        w('    return %s;' % call(pyfunc, return_type))
        w('}')
        return '\n'.join(lines)


    def ctx_pypy_type(self):
        return 'void *'

    def pypy_stub(self):
        signature = toC(self.node)
        if self.is_varargs():
            return '# %s' % signature
        #
        argnames = [p.name for p in self.node.type.args.params]
        lines = []
        w = lines.append
        w('@API.func("%s")' % signature)
        w('def %s(space, %s):' % (self.name, ', '.join(argnames)))
        w('    from rpython.rlib.nonconst import NonConstant # for the annotator')
        w('    if NonConstant(False): return 0')
        w('    raise NotImplementedError')
        w('')
        return '\n'.join(lines)


@attr.s
class GlobalVar:
    name = attr.ib()
    node = attr.ib(repr=False)

    def ctx_name(self):
        return self.name

    def trampoline_def(self):
        return None

    def ctx_pypy_type(self):
        return 'struct _HPy_s'

    def pypy_stub(self):
        return ''


class FuncDeclVisitor(pycparser.c_ast.NodeVisitor):
    def __init__(self, api, convert_name):
        self.api = api
        self.convert_name = convert_name

    def visit_Decl(self, node):
        if isinstance(node.type, c_ast.FuncDecl):
            self._visit_function(node)
        elif isinstance(node.type, c_ast.TypeDecl):
            self._visit_global_var(node)

    def _visit_function(self, node):
        name = node.name
        if not name.startswith('HPy') and not name.startswith('_HPy'):
            print('WARNING: Ignoring non-hpy declaration: %s' % name)
            return
        for p in node.type.args.params:
            if hasattr(p, 'name') and p.name is None:
                raise ValueError("non-named argument in declaration of %s" %
                                 name)
        cpy_name = self.convert_name(name)
        func = Function(name, cpy_name, node)
        self.api.declarations.append(func)
        self.api.functions.append(func)

    def _visit_global_var(self, node):
        name = node.name
        if not name.startswith('h_'):
            print('WARNING: Ignoring non-hpy variable declaration: %s' % name)
            return
        assert toC(node.type.type) == "HPy"
        var = GlobalVar(name, node)
        self.api.declarations.append(var)
        self.api.variables.append(var)

SPECIAL_CASES = {
    'HPy_Dup': None,
    'HPy_Close': None,
    'HPyModule_Create': None,
    'HPy_GetAttr': 'PyObject_GetAttr',
    'HPy_GetAttr_s': 'PyObject_GetAttrString',
    'HPy_HasAttr': 'PyObject_HasAttr',
    'HPy_HasAttr_s': 'PyObject_HasAttrString',
    'HPy_SetAttr': 'PyObject_SetAttr',
    'HPy_SetAttr_s': 'PyObject_SetAttrString',
    'HPy_GetItem': 'PyObject_GetItem',
    'HPy_GetItem_i': None,
    'HPy_GetItem_s': None,
    'HPy_SetItem': 'PyObject_SetItem',
    'HPy_SetItem_i': None,
    'HPy_SetItem_s': None,
    '_HPy_Cast': None,
    'HPy_FromPyObject': None,
    'HPy_AsPyObject': None,
    '_HPy_CallRealFunctionFromTrampoline': None,
    'HPyErr_Occurred': None,
    'HPy_Add': 'PyNumber_Add',
    'HPy_Subtract': 'PyNumber_Subtract',
    'HPy_Multiply': 'PyNumber_Multiply',
    'HPy_MatrixMultiply': 'PyNumber_MatrixMultiply',
    'HPy_FloorDivide': 'PyNumber_FloorDivide',
    'HPy_TrueDivide': 'PyNumber_TrueDivide',
    'HPy_Remainder': 'PyNumber_Remainder',
    'HPy_Divmod': 'PyNumber_Divmod',
    'HPy_Power': 'PyNumber_Power',
    'HPy_Negative': 'PyNumber_Negative',
    'HPy_Positive': 'PyNumber_Positive',
    'HPy_Absolute': 'PyNumber_Absolute',
    'HPy_Invert': 'PyNumber_Invert',
    'HPy_Lshift': 'PyNumber_Lshift',
    'HPy_Rshift': 'PyNumber_Rshift',
    'HPy_And': 'PyNumber_And',
    'HPy_Xor': 'PyNumber_Xor',
    'HPy_Or': 'PyNumber_Or',
    'HPy_Index': 'PyNumber_Index',
    'HPy_Long': 'PyNumber_Long',
    'HPy_Float': 'PyNumber_Float',
    'HPy_InPlaceAdd': 'PyNumber_InPlaceAdd',
    'HPy_InPlaceSubtract': 'PyNumber_InPlaceSubtract',
    'HPy_InPlaceMultiply': 'PyNumber_InPlaceMultiply',
    'HPy_InPlaceMatrixMultiply': 'PyNumber_InPlaceMatrixMultiply',
    'HPy_InPlaceFloorDivide': 'PyNumber_InPlaceFloorDivide',
    'HPy_InPlaceTrueDivide': 'PyNumber_InPlaceTrueDivide',
    'HPy_InPlaceRemainder': 'PyNumber_InPlaceRemainder',
    'HPy_InPlacePower': 'PyNumber_InPlacePower',
    'HPy_InPlaceLshift': 'PyNumber_InPlaceLshift',
    'HPy_InPlaceRshift': 'PyNumber_InPlaceRshift',
    'HPy_InPlaceAnd': 'PyNumber_InPlaceAnd',
    'HPy_InPlaceXor': 'PyNumber_InPlaceXor',
    'HPy_InPlaceOr': 'PyNumber_InPlaceOr',
    '_HPy_New': None,
    'HPyType_FromSpec': None,
}

NO_TRAMPOLINES = set([
    '_HPy_New',
    ])


def convert_name(hpy_name):
    if hpy_name in SPECIAL_CASES:
        return SPECIAL_CASES[hpy_name]
    return re.sub(r'^_?HPy_?', 'Py', hpy_name)


class HPyAPI:

    def __init__(self, filename):
        self.ast = pycparser.parse_file(filename, use_cpp=True)
        #self.ast.show()
        self.collect_declarations()

    @classmethod
    def parse(cls, filename):
        return cls(filename)


    def get(self, name):
        for d in self.declarations:
            if d.name == name:
                return d
        raise KeyError(name)

    def collect_declarations(self):
        self.declarations = [] # this should be removed, eventually
        self.functions = []
        self.variables = []
        v = FuncDeclVisitor(self, convert_name)
        v.visit(self.ast)


    def gen_func_trampolines(self):
        lines = []
        for f in self.declarations:
            trampoline = f.trampoline_def()
            if trampoline:
                lines.append(trampoline)
                lines.append('')
        return '\n'.join(lines)

    def gen_func_implementations(self):
        lines = []
        for f in self.declarations:
            if not isinstance(f, Function):
                continue
            if not f.cpython_name:
                continue
            lines.append(f.implementation())
            lines.append('')
        return '\n'.join(lines)

    def gen_pypy_decl(self):
        lines = []
        w = lines.append
        w("typedef struct _HPyContext_s {")
        w("    int ctx_version;")
        for f in self.declarations:
            w("    %s %s;" % (f.ctx_pypy_type(), f.ctx_name()))
        w("} _struct_HPyContext_s;")
        w("")
        w("")
        # generate stubs for all the API functions
        for f in self.declarations:
            w(f.pypy_stub())
        return '\n'.join(lines)
