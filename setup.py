from setuptools import setup, Extension
import hpy_devel

setup(
    name="hpy_universal",
    ext_modules = [
        Extension('hpy_universal', [
                      'cpython-universal/src/hpymodule.c',
                      'cpython-universal/src/handles.c',
                      'cpython-universal/src/api.c',
                      ],
                  include_dirs=[hpy_devel.get_include()])
    ]
)
