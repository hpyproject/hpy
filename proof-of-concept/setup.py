import sys, os
from setuptools import setup, Extension
import hpy.devel

setup(
    name="hpy-pof",
    ext_modules = [
        Extension('pof', ['pof.c'], include_dirs=[hpy.devel.get_include()]),
    ],
    setup_requires = ['hpy.devel'],
)
