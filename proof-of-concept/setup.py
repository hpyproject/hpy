from setuptools import setup, Extension

setup(
    name="hpy-pof",
    ext_modules=[
        Extension('pofpackage.point_capi', sources=['pofpackage/point_capi.c'])
    ],
    hpy_ext_modules=[
        Extension('pof', sources=['pof.c']),
        Extension('pofpackage.foo', sources=['pofpackage/foo.c']),
        Extension('pofpackage.point_hpy_legacy_1',
                  sources=['pofpackage/point_hpy_legacy_1.c'])
    ],
    setup_requires=['hpy.devel'],
)
