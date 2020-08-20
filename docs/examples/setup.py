from setuptools import setup, Extension

setup(
    name="hpy-example",
    hpy_ext_modules=[
        Extension('simple', sources=['simple.c']),
    ],
    setup_requires=['hpy.devel'],
)
