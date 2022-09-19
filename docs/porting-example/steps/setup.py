# -*- coding: utf-8 -*-

from setuptools import setup, Extension

from conftest import PORTING_STEPS


setup(
    name="hpy-porting-example",
    ext_modules=[
        Extension("step_00_c_api", sources=["step_00_c_api.c"])
    ],
    hpy_ext_modules=[
        Extension(name, sources=[f"{name}.c"])
        for name in
        [
            "step_01_hpy_legacy",
            "step_02_hpy_legacy",
            "step_03_hpy_final",
        ]
    ],
    setup_requires=['hpy'],
)
