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
from .ctx import autogen_ctx_h
from .autogenfile import DISCLAIMER # temporary


def main():
    if len(sys.argv) != 2:
        print('Usage: python -m hpy.tools.autogen OUTDIR')
        sys.exit(1)
    outdir = py.path.local(sys.argv[1])
    include = outdir.join('hpy', 'devel', 'include')
    autogen_trampolines = include.join('universal', 'autogen_trampolines.h')
    autogen_ctx_def = outdir.join('hpy', 'universal', 'src', 'autogen_ctx_def.h')
    autogen_impl = include.join('common', 'autogen_impl.h')
    autogen_pypy = outdir.join('hpy', 'tools', 'autogen', 'autogen_pypy.txt')

    public_api_h = py.path.local(__file__).dirpath('public_api.h')
    api = HPyAPI.parse(public_api_h)
    for func in api.declarations:
        print(func)

    autogen_ctx_h(api).generate()


    func_trampolines = api.gen_func_trampolines()
    ctx_def = api.gen_ctx_def()
    impl = api.gen_func_implementations()
    pypy_decl = api.gen_pypy_decl()

    with autogen_trampolines.open('w') as f:
        print(DISCLAIMER, file=f)
        print(func_trampolines, file=f)

    with autogen_ctx_def.open('w') as f:
        print(DISCLAIMER, file=f)
        print(ctx_def, file=f)

    with autogen_impl.open('w') as f:
        print(DISCLAIMER, file=f)
        print(impl, file=f)

    with autogen_pypy.open('w') as f:
        print(pypy_decl, file=f)

if __name__ == '__main__':
    main()
