from setuptools import setup, Extension
import hpy.devel

setup(
    name="hpy-example",
    ext_modules=[
        Extension(
            'simple', ['simple.c'] + hpy.devel.get_sources(),
            include_dirs=[hpy.devel.get_include()],
        ),
    ],
)
