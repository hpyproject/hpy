import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(__file__, '../../hpy-api')))
from setuptools import setup, Extension
import hpy_devel

setup(
    name="hpy-pof",
    ext_modules = [
        Extension('pof', ['pof.c'], include_dirs=[hpy_devel.get_include()]),
    ]
)
