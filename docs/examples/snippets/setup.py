from setuptools import setup, Extension
from os import path

setup(
    name="hpy-snippets",
    hpy_ext_modules=[
        Extension('hpyvarargs', sources=[path.join(path.dirname(__file__), 'hpyvarargs.c')]),
        Extension('snippets', sources=[path.join(path.dirname(__file__), 'snippets.c')]),
    ],
    setup_requires=['hpy'],
)
