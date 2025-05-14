from setuptools import setup, Extension

setup(
    # Workaround: HPy adds files to the sources list and uses absolute paths.
    # Newer setuptools complain about that if package data should be included.
    # Therefore, we explicitly disable this here.
    include_package_data=False,
    ext_modules = [
        Extension('cpy_simple',
                  ['src/cpy_simple.c'],
                  extra_compile_args=['-g'])
    ],
    hpy_ext_modules = [
        Extension('hpy_simple',
                  ['src/hpy_simple.c'],
                  extra_compile_args=['-g']),
    ],
    cffi_modules=["_valgrind_build.py:ffibuilder"],
)
