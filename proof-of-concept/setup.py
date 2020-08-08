from setuptools import setup, Extension
from hpy.devel import HPyExtension

# XXXX use HPyExtension
setup(
    name="hpy-pof",
    ext_modules=[
        HPyExtension('pof', sources=['pof.c']),
        #HPyExtension('pof', sources=['pof.c'], hpy_abi='universal'),
    ],
    setup_requires=['hpy.devel'],
)
