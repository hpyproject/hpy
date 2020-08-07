import os
from setuptools import setup, Extension

if 'HPY_DEBUG' in os.environ:
    EXTRA_COMPILE_ARGS = ['-g', '-O0']
else:
    EXTRA_COMPILE_ARGS = []

setup(
    name="hpy.devel",
    packages = ['hpy.devel'],
    include_package_data=True,
    ext_modules = [
        Extension('hpy.universal',
                  ['hpy/universal/src/hpymodule.c',
                   'hpy/universal/src/handles.c',
                   'hpy/universal/src/ctx.c',
                   'hpy/universal/src/ctx_meth.c',
                   'hpy/universal/src/ctx_misc.c',
                   'hpy/devel/src/runtime/ctx_module.c',
                   'hpy/devel/src/runtime/ctx_type.c',
                   'hpy/devel/src/runtime/argparse.c',
                   'hpy/devel/src/runtime/listbuilder.c',
                  ],
                  include_dirs=[
                      'hpy/devel/include',
                      'hpy/universal/src',
                  ],
                  extra_compile_args=[
                      '-DHPY_UNIVERSAL_ABI',
                  ] + EXTRA_COMPILE_ARGS
        )]

)
