"""
Parse public_api.h and generates various stubs around
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
from .autogenfile import DISCLAIMER # temporary


def main():
    if len(sys.argv) != 2:
        print('Usage: python -m hpy.tools.autogen OUTDIR')
        sys.exit(1)
    outdir = py.path.local(sys.argv[1])
    autogen_pypy = outdir.join('hpy', 'tools', 'autogen', 'autogen_pypy.txt')

    public_api_h = py.path.local(__file__).dirpath('public_api.h')
    api = HPyAPI.parse(public_api_h)
    for func in api.declarations:
        print(func)

    for cls in (autogen_ctx_h,
                autogen_ctx_def_h,
                autogen_trampolines_h,
                autogen_impl_h):
        cls(api).write(outdir)

    pypy_decl = api.gen_pypy_decl()

    with autogen_pypy.open('w') as f:
        print(pypy_decl, file=f)

if __name__ == '__main__':
    main()
