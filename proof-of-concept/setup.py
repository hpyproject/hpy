from setuptools import setup, Extension

setup(
    name="hpy-pof",
    hpy_ext_modules=[
        Extension('pof', sources=['pof.c']),
        Extension('pofpackage.foo', sources=['pofpackage/foo.c']),
        Extension('pofcpp', sources=['pofcpp.cpp']),
        Extension('pofpackage.bar', sources=['pofpackage/bar.cpp']),
    ],
    setup_requires=['hpy'],
)
