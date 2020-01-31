from setuptools import setup, Extension

setup(
    name="hpy.devel",
    ext_modules = [
        Extension('hpy.universal',
                  ['hpy/universal/src/hpymodule.c',
                   'hpy/universal/src/handles.c',
                   'hpy/universal/src/api.c',
                  ],
                  include_dirs=['hpy/devel/include'],
        )]

)
