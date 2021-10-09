"""
Parse public_api.h and generate various stubs around
"""
import sys
import py
import pycparser
from packaging import version
if version.parse(pycparser.__version__) < version.parse('2.20'):
    raise ImportError('You need pycparser>=2.20 to run autogen')

from .parse import HPyAPI, PUBLIC_API_H
from .ctx import autogen_ctx_h, autogen_ctx_def_h
from .trampolines import (autogen_trampolines_h,
                          cpython_autogen_api_impl_h,
                          universal_autogen_ctx_impl_h)
from .hpyfunc import autogen_hpyfunc_declare_h
from .hpyfunc import autogen_hpyfunc_trampoline_h
from .hpyfunc import autogen_ctx_call_i
from .hpyfunc import autogen_cpython_hpyfunc_trampoline_h
from .hpyslot import autogen_hpyslot_h
from .debug import (autogen_debug_ctx_init_h,
                    autogen_debug_wrappers,
                    autogen_debug_ctx_call_i)
from .pypy import autogen_pypy_txt

def main():
    if len(sys.argv) != 2:
        print('Usage: python -m hpy.tools.autogen OUTDIR')
        sys.exit(1)
    outdir = py.path.local(sys.argv[1])

    api = HPyAPI.parse(PUBLIC_API_H)
    ## for func in api.functions:
    ##     print(func)

    for cls in (autogen_ctx_h,
                autogen_ctx_def_h,
                autogen_trampolines_h,
                cpython_autogen_api_impl_h,
                universal_autogen_ctx_impl_h,
                autogen_hpyfunc_declare_h,
                autogen_hpyfunc_trampoline_h,
                autogen_ctx_call_i,
                autogen_cpython_hpyfunc_trampoline_h,
                autogen_hpyslot_h,
                autogen_debug_ctx_init_h,
                autogen_debug_wrappers,
                autogen_debug_ctx_call_i,
                autogen_pypy_txt):
        cls(api).write(outdir)


if __name__ == '__main__':
    main()
