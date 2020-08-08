from setuptools import setup
from hpy.devel import HPyExtension

setup(
    name="hpy-example",
    ext_modules=[
        HPyExtension('simple', sources=['simple.c']),
    ],
)
