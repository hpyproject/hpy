"""
Test the hpy+distutils integration. Most of the relevant code is in
hpy/devel/__init__.py.

Note that this is a different kind of test than the majority of the other
files in this directory, which all inherit from HPyTest and test the API
itself.
"""

import sys
import textwrap
import subprocess
from pathlib import Path
import pytest

HPY_ROOT = Path(__file__).parent.parent

@pytest.fixture(scope='session')
def hpy_wheel(tmpdir_factory):
    wheelhouse = tmpdir_factory.mktemp('wheelhouse')
    proc = subprocess.run([
        sys.executable, '-m', 'pip', 'wheel', str(HPY_ROOT), '-w', str(wheelhouse)
        ])
    wheels = wheelhouse.listdir('hpy*.whl')
    assert len(wheels) == 1
    return wheels[0]


@pytest.mark.usefixtures('initargs')
class TestDistutils:

    @pytest.fixture()
    def initargs(self, tmpdir, venv, hpy_wheel):
        self.tmpdir = tmpdir
        self.venv = venv
        self.run('pip', 'install', str(hpy_wheel))
        self.hpy_test_project = tmpdir.join('hpy_test_project').ensure(dir=True)
        self.gen_project()
        self.hpy_test_project.chdir()

    def run(self, exe, *args, capture=False):
        """
        Run a command inside the venv; if capture==True, return stdout
        """
        full_exe = Path(self.venv.bin).joinpath(exe)
        cmd = [str(full_exe)] + list(args)
        print('[RUN]', ' '.join(cmd))
        if capture:
            proc = subprocess.run(cmd, stdout=subprocess.PIPE)
            out = proc.stdout.decode('latin-1').strip()
        else:
            proc = subprocess.run(cmd)
            out = None
        if proc.returncode != 0:
            raise Exception(f"Command {exe} failed")
        return out


    def writefile(self, fname, content):
        """
        Write a file inside hpy_test_project
        """
        f = self.hpy_test_project.join(fname)
        content = textwrap.dedent(content)
        f.write(content)

    def gen_project(self):
        """
        Generate the files needed to build the project, except setup.py
        """
        self.writefile('cpymod.c', """
            // the simplest possible Python/C module
            #include <Python.h>
            static PyModuleDef moduledef = {
                PyModuleDef_HEAD_INIT,
                "cpymod",
                "cpymod docstring"
            };

            PyMODINIT_FUNC
            PyInit_cpymod(void)
            {
                return PyModule_Create(&moduledef);
            }
        """)

        self.writefile('hpymod.c', """
            // the simplest possible HPy module
            #include <hpy.h>
            static HPyModuleDef moduledef = {
                .name = "hpymod",
                .doc = "hpymod docstring",
            };

            HPy_MODINIT(hfoo)
            static HPy init_hfoo_impl(HPyContext *ctx)
            {
                return HPyModule_Create(ctx, &moduledef);
            }
        """)

    def gen_setup_py(self, src):
        preamble = textwrap.dedent("""
            from setuptools import setup, Extension
            cpymod = Extension("cpymod", ["cpymod.c"])
            hpymod = Extension("hpymod", ["hpymod.c"])
        """)
        src = preamble + textwrap.dedent(src)
        f = self.hpy_test_project.join('setup.py')
        f.write(src)

    def test_cpymod_only(self):
        # CPython-only project, no hpy at all
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  ext_modules = [cpymod],
            )
        """)
        self.run('python', 'setup.py', 'install')
        out = self.run('python', '-c', 'import cpymod; print(cpymod.__doc__)',
                       capture=True)
        assert out == 'cpymod docstring'

    def test_cpymod_with_empty_hpy_ext_modules(self):
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  ext_modules = [cpymod],
                  hpy_ext_modules = []
            )
        """)
        self.run('python', 'setup.py', 'install')
        out = self.run('python', '-c', 'import cpymod; print(cpymod.__doc__)',
                       capture=True)
        assert out == 'cpymod docstring'
