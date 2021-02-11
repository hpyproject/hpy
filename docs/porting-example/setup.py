# -*- coding: utf-8 -*-

from setuptools import setup, Extension

from conftest import PORTING_STEPS


setup(
    name="porting-example",
    hpy_ext_modules=[
        Extension(step.name, sources=[step.src])
        for step in PORTING_STEPS
    ],
    setup_requires=['hpy.devel'],
)
