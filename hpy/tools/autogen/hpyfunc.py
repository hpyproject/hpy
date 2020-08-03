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
        return '\n'.join(lines)
