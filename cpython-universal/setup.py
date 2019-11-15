from setuptools import setup, Extension

setup(
    name="hpy_universal",
    ext_modules = [
        Extension('hpy_universal', ['src/hpymodule.c', 'src/api.c'],
                  include_dirs=['../hpy-api/include'])
    ]
)
