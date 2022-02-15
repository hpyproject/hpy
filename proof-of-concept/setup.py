from setuptools import setup, Extension
import platform

compile_extra_args = []

if platform.system() == "Windows":
    compile_extra_args = [
        "/std:c++latest",  # MSVC C7555
    ]

setup(
    name="hpy-pof",
    hpy_ext_modules=[
        Extension('pof', 
                    sources=['pof.c']),
        Extension('pofpackage.foo', 
                    sources=['pofpackage/foo.c']),
        Extension('pofcpp', 
                    sources=['pofcpp.cpp'], 
                    language='C++', 
                    extra_compile_args=compile_extra_args),
        Extension('pofpackage.bar', 
                    sources=['pofpackage/bar.cpp'], 
                    language='C++', 
                    extra_compile_args=compile_extra_args),
    ],
    setup_requires=['hpy'],
)
