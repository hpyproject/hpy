from setuptools import setup, Extension

setup(
    name="hpy.microbench",
    setup_requires=['hpy.devel'],
    install_requires=['docopt'],
    ext_modules = [
        Extension('hpy.microbench.cpy_simple', ['hpy/microbench/src/cpy_simple.c']),
    ],
    hpy_ext_modules = [
        Extension('hpy.microbench.hpy_simple', ['hpy/microbench/src/hpy_simple.c']),
    ],
)
