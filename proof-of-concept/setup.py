from setuptools import setup, Extension

setup(
    name="hpy-pof",
    ## ext_modules=[
    ##     Extension('foo', sources=['foo.c']),
    ## ],
    hpy_ext_modules=[
        Extension('pof', sources=['pof.c']),
#        Extension('bar.foo', sources=['bar/foo.c']),
    ],
    setup_requires=['hpy.devel'],
)
