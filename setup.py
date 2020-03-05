from setuptools import setup, Extension

setup(
    name="hpy.devel",
    packages = ['hpy.devel'],
    include_package_data=True,
    ext_modules = [
        Extension('hpy.universal',
                  ['hpy/universal/src/hpymodule.c',
                   'hpy/universal/src/handles.c',
                   'hpy/universal/src/api.c',
                   'hpy/devel/src/runtime/argparse.c',
                  ],
                  include_dirs=['hpy/devel/include'],
        )]

)
