from copy import deepcopy
import attr
import re
import py
import pycparser
from pycparser import c_ast
from pycparser.c_generator import CGenerator

PUBLIC_API_H = py.path.local(__file__).dirpath('public_api.h')


def toC(node):
    return toC.gen.visit(node)
toC.gen = CGenerator()

def find_typedecl(node):
    while not isinstance(node, c_ast.TypeDecl):
        node = node.type
    return node


@attr.s
class Function:
    _BASE_NAME = re.compile(r'^_?HPy_?')

    name = attr.ib()
    cpython_name = attr.ib()
    node = attr.ib(repr=False)

    def base_name(self):
        return self._BASE_NAME.sub('', self.name)

    def ctx_name(self):
        # e.g. "ctx_Module_Create"
        return self._BASE_NAME.sub(r'ctx_', self.name)

    def is_varargs(self):
        return (len(self.node.type.args.params) > 0 and
                isinstance(self.node.type.args.params[-1], c_ast.EllipsisParam))


@attr.s
class GlobalVar:
    name = attr.ib()
    node = attr.ib(repr=False)

    def ctx_name(self):
        return self.name


@attr.s
class HPyFunc:
    _BASE_NAME = re.compile(r'^HPyFunc_?')

    name = attr.ib()
    node = attr.ib(repr=False)

    def base_name(self):
        return self._BASE_NAME.sub('', self.name)

    def params(self):
        return self.node.type.type.args.params

    def return_type(self):
        return self.node.type.type.type

@attr.s
class HPySlot:
    # represent a declaration contained inside enum HPySlot_Slot, such as:
    #        HPy_nb_add = SLOT(7, HPyFunc_BINARYFUNC)

    name = attr.ib()      # "HPy_nb_add"
    value = attr.ib()     # "7"
    hpyfunc = attr.ib()   # "HPyFunc_BINARYFUNC"


class HPyAPIVisitor(pycparser.c_ast.NodeVisitor):
    def __init__(self, api, convert_name):
        self.api = api
        self.convert_name = convert_name

    def _is_function_ptr(self, node):
        return (isinstance(node, c_ast.PtrDecl) and
                isinstance(node.type, c_ast.FuncDecl))

    def visit_Decl(self, node):
        if isinstance(node.type, c_ast.FuncDecl):
            self._visit_function(node)
        elif isinstance(node.type, c_ast.TypeDecl):
            self._visit_global_var(node)

    def visit_Typedef(self, node):
        # find only typedefs to function pointers whose name starts by HPyFunc_
        if node.name.startswith('HPyFunc_') and self._is_function_ptr(node.type):
            self._visit_hpyfunc_typedef(node)
        elif node.name == 'HPySlot_Slot':
            self._visit_hpyslot_slot(node)

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
        self.api.functions.append(func)

    def _visit_global_var(self, node):
        name = node.name
        if not name.startswith('h_'):
            print('WARNING: Ignoring non-hpy variable declaration: %s' % name)
            return
        assert toC(node.type.type) == "HPy"
        var = GlobalVar(name, node)
        self.api.variables.append(var)

    def _visit_hpyfunc_typedef(self, node):
        hpyfunc = HPyFunc(node.name, node)
        self.api.hpyfunc_typedefs.append(hpyfunc)

    def _visit_hpyslot_slot(self, node):
        for e in node.type.type.values.enumerators:
            call = e.value
            assert isinstance(call, c_ast.FuncCall) and call.name.name == 'SLOT'
            assert len(call.args.exprs) == 2
            const_value, id_hpyfunc = call.args.exprs
            assert isinstance(const_value, c_ast.Constant) and const_value.type == 'int'
            assert isinstance(id_hpyfunc, c_ast.ID)
            value = const_value.value
            hpyfunc = id_hpyfunc.name
            self.api.hpyslots.append(HPySlot(e.name, value, hpyfunc))


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
    'HPy_Length': 'PyObject_Length',
    'HPy_CallTupleDict': None,
    'HPy_FromPyObject': None,
    'HPy_AsPyObject': None,
    'HPy_AsStruct': None,
    'HPy_AsStructLegacy': None,
    '_HPy_CallRealFunctionFromTrampoline': None,
    '_HPy_CallDestroyAndThenDealloc': None,
    'HPyErr_Occurred': None,
    'HPy_FatalError': None,
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
    'HPyType_GenericNew': None,
    'HPy_Repr': 'PyObject_Repr',
    'HPy_Str': 'PyObject_Str',
    'HPy_ASCII': 'PyObject_ASCII',
    'HPy_Bytes': 'PyObject_Bytes',
    'HPy_IsTrue': 'PyObject_IsTrue',
    'HPy_RichCompare': 'PyObject_RichCompare',
    'HPy_RichCompareBool': 'PyObject_RichCompareBool',
    'HPy_Hash': 'PyObject_Hash',
    'HPyListBuilder_New': None,
    'HPyListBuilder_Set': None,
    'HPyListBuilder_Build': None,
    'HPyListBuilder_Cancel': None,
    'HPyTuple_FromArray': None,
    'HPyTupleBuilder_New': None,
    'HPyTupleBuilder_Set': None,
    'HPyTupleBuilder_Build': None,
    'HPyTupleBuilder_Cancel': None,
    'HPyTracker_New': None,
    'HPyTracker_Add': None,
    'HPyTracker_ForgetAll': None,
    'HPyTracker_Close': None,
    '_HPy_Dump': None,
    'HPy_Type': 'PyObject_Type',
    'HPy_TypeCheck': None,
    'HPy_Is': None,
    'HPyBytes_FromStringAndSize': None,
}


def convert_name(hpy_name):
    if hpy_name in SPECIAL_CASES:
        return SPECIAL_CASES[hpy_name]
    return re.sub(r'^_?HPy_?', 'Py', hpy_name)


class HPyAPI:
    _r_comment = re.compile(r"/\*.*?\*/|//([^\n\\]|\\.)*?$",
                            re.DOTALL | re.MULTILINE)

    def __init__(self, filename):
        with open(filename, 'r') as f:
            csource = f.read()
        # Remove comments.  NOTE: this assumes that comments are never inside
        # string literals, but there shouldn't be any here.
        def replace_keeping_newlines(m):
            return ' ' + m.group().count('\n') * '\n'
        csource = self._r_comment.sub(replace_keeping_newlines, csource)
        self.ast = pycparser.CParser().parse(csource)
        ## print(); self.ast.show()
        self.collect_declarations()

    @classmethod
    def parse(cls, filename):
        return cls(filename)

    def get_func(self, name):
        return self._lookup(name, self.functions)

    def get_var(self, name):
        return self._lookup(name, self.variables)

    def get_hpyfunc_typedef(self, name):
        return self._lookup(name, self.hpyfunc_typedefs)

    def get_slot(self, name):
        return self._lookup(name, self.hpyslots)

    def _lookup(self, name, collection):
        for x in collection:
            if x.name == name:
                return x
        raise KeyError(name)

    def collect_declarations(self):
        self.functions = []
        self.variables = []
        self.hpyfunc_typedefs = []
        self.hpyslots = []
        v = HPyAPIVisitor(self, convert_name)
        v.visit(self.ast)
