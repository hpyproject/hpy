"""
Parse public_api.h and generate various stubs around
"""
import sys
import py
import pycparser
from packaging import version
if version.parse(pycparser.__version__) < version.parse('2.20'):
    raise ImportError('You need pycparsers>=2.20 to run autogen')

from .parse import HPyAPI
from .ctx import autogen_ctx_h, autogen_ctx_def_h
from .trampolines import autogen_trampolines_h, autogen_impl_h
from .pypy import autogen_pypy_txt

def main():
    if len(sys.argv) != 2:
        print('Usage: python -m hpy.tools.autogen OUTDIR')
        sys.exit(1)
    outdir = py.path.local(sys.argv[1])
    autogen_pypy = outdir.join('hpy', 'tools', 'autogen', 'autogen_pypy.txt')

    public_api_h = py.path.local(__file__).dirpath('public_api.h')
    api = HPyAPI.parse(public_api_h)
    ## for func in api.functions:
    ##     print(func)

    for cls in (autogen_ctx_h,
                autogen_ctx_def_h,
                autogen_trampolines_h,
                autogen_impl_h,
                autogen_pypy_txt):
        cls(api).write(outdir)


if __name__ == '__main__':
    main()
