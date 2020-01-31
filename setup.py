from setuptools import setup, Extension

setup(
    name="hpy.universal",
    ext_modules = [
        Extension('hpy.universal',
                  ['hpy/universal/src/hpymodule.c',
                   'hpy/universal/src/handles.c',
                   'hpy/universal/src/api.c',
                  ],
                  include_dirs=['hpy-api/hpy_devel/include'],
        )]

)
