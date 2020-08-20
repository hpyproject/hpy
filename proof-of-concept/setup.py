from setuptools import setup, Extension

setup(
    name="hpy-pof",
    hpy_ext_modules=[
        Extension('pof', sources=['pof.c']),
    ],
    setup_requires=['hpy.devel'],
)
