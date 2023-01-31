from setuptools import setup, Extension
from os import path

setup(
    name="hpy-snippets",
    hpy_ext_modules=[
        Extension('hpyvarargs', sources=[path.join(path.dirname(__file__), 'hpyvarargs.c')]),
        Extension('snippets', sources=[path.join(path.dirname(__file__), 'snippets.c')]),
        Extension('hpyinit', sources=[path.join(path.dirname(__file__), 'hpyinit.c')]),
    ],
    ext_modules=[
        Extension('legacyinit', sources=[path.join(path.dirname(__file__), 'legacyinit.c')]),
    ],
    setup_requires=['hpy'],
)
