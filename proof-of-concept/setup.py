import sys, os
from setuptools import setup, Extension
import hpy_devel

setup(
    name="hpy-pof",
    ext_modules = [
        Extension('pof', ['pof.c'], include_dirs=[hpy_devel.get_include()]),
    ],
    setup_requires = ['hpy_devel'],
)
