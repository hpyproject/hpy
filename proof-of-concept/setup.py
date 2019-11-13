from setuptools import setup, Extension

setup(
    name="hpy-pof",
    ext_modules = [Extension('pof', ['pof.c'], include_dirs=['../src/include'])],
)
