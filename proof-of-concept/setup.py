from setuptools import setup, Extension

setup(
    name="hpy-pof",
    ext_modules=[
        Extension('pofpackage.point_capi', sources=['pofpackage/point_capi.c'])
    ],
    hpy_ext_modules=[
        Extension('pof', sources=['pof.c']),
        Extension('pofpackage.foo', sources=['pofpackage/foo.c']),
    ],
    setup_requires=['hpy.devel'],
)
