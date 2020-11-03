from setuptools import setup, Extension

setup(
    name="hpy.microbench",
    ext_modules = [
        Extension('hpy.microbench.cpy_simple', ['hpy/microbench/src/cpy_simple.c']),
    ],
    install_requires=['docopt']
)
