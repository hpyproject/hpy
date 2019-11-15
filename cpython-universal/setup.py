# This setup.py is incorrect because it relies on files which are outside the
# root directory. We need to think how to fix it, but for now it is enough to
# just run setup.py manually.

from setuptools import setup, Extension

setup(
    name="hpy_universal",
    ext_modules = [
        Extension('hpy_universal', [
                      'src/hpymodule.c',
                      'src/handles.c',
                      'src/api.c',
                      ],
                  include_dirs=['../hpy-api/include'])
    ]
)
