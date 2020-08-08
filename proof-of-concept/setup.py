from setuptools import setup, Extension
from hpy.devel import HPyExtension

# XXXX use HPyExtension
setup(
    name="hpy-pof",
    ext_modules=[
        HPyExtension('pof', sources=['pof.c']),
    ],
    setup_requires=['hpy.devel'],
)
