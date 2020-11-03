from setuptools import setup, Extension

setup(
    name="hpy.microbench",
    setup_requires=['hpy.devel'],
    ext_modules = [
        Extension('cpy_simple', ['src/cpy_simple.c']),
    ],
    hpy_ext_modules = [
        Extension('hpy_simple', ['src/hpy_simple.c']),
    ],
)
