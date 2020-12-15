import os
from setuptools import setup, Extension

if 'HPY_DEBUG' in os.environ:
    EXTRA_COMPILE_ARGS = ['-g', '-O0']
else:
    EXTRA_COMPILE_ARGS = []

setup(
    name="hpy.debug",
    hpy_ext_modules=[
        Extension('hpy.debug._ctx',
                  extra_compile_args=EXTRA_COMPILE_ARGS,
                  sources=[
                      'hpy/debug/src/_ctx.c',
                  ]),
    ],
    setup_requires=['hpy.devel'],
)
