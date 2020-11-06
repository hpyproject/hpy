from setuptools import setup, Extension

setup(
    name="hpy.microbench",
    setup_requires=['hpy.devel', 'cffi'],
    ext_modules = [
        Extension('cpy_simple', ['src/cpy_simple.c']),
    ],
    hpy_ext_modules = [
        Extension('hpy_simple', ['src/hpy_simple.c']),
    ],
    cffi_modules=["_valgrind_build.py:ffibuilder"],
)
