from setuptools import setup, Extension

setup(
    name="hpy.devel",
    packages = ['hpy.devel'],
    include_package_data=True,
    ext_modules = [
        Extension('hpy.universal',
                  ['hpy/universal/src/hpymodule.c',
                   'hpy/universal/src/handles.c',
                   'hpy/universal/src/ctx.c',
                   'hpy/universal/src/ctx_module.c',
                   'hpy/universal/src/ctx_meth.c',
                   'hpy/universal/src/ctx_misc.c',
                   'hpy/universal/src/debugmode.c',
                   'hpy/devel/src/runtime/ctx_type.c',
                   'hpy/devel/src/runtime/argparse.c',
                  ],
                  include_dirs=[
                      'hpy/devel/include',
                      'hpy/universal/src',
                  ],
                  extra_compile_args=[
                      '-DHPY_UNIVERSAL_ABI',
                      #'-g', 'O0'
                  ],
        )]

)
