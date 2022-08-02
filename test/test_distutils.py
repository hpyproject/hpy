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
import shutil
import venv
import py
import pytest

HPY_ROOT = Path(__file__).parent.parent

# this is only for development: if we set it to true, we don't have to
# recreate the venv_template between runs, it's much faster
REUSE_VENV_TEMPLATE = True

@pytest.fixture(scope='session')
def venv_template(tmpdir_factory):
    if REUSE_VENV_TEMPLATE:
        d = py.path.local('/tmp/venv-for-hpytest')
        if d.check(dir=True):
            # if it exists, we assume it's correct. If you want to recreate,
            # just manually delete /tmp/venv-for-hpytest
            return d
    else:
        d = tmpdir_factory.mktemp('venv')

    venv.create(d, with_pip=True)
    pip = d.join('bin', 'pip')
    subprocess.run([str(pip), 'install', '-U', 'pip'])
    subprocess.run([str(pip), 'install', '-e', str(HPY_ROOT)], check=True)
    return d


@pytest.mark.usefixtures('initargs')
class TestDistutils:

    @pytest.fixture()
    def initargs(self, tmpdir, venv_template):
        self.tmpdir = tmpdir
        # create a fresh venv by copying the template
        self.venv = tmpdir.join('venv')
        shutil.copytree(venv_template, self.venv)
        # create the files for our test project
        self.hpy_test_project = tmpdir.join('hpy_test_project').ensure(dir=True)
        self.gen_project()
        self.hpy_test_project.chdir()

    def run(self, exe, *args, capture=False):
        """
        Run a command inside the venv; if capture==True, return stdout
        """
        full_exe = self.venv.join('bin', exe)
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
            #ifdef HPY_UNIVERSAL_ABI
                .doc = "hpymod Universal ABI",
            #else
                .doc = "hpymod CPython ABI",
            #endif
            };

            HPy_MODINIT(hpymod)
            static HPy init_hpymod_impl(HPyContext *ctx)
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

    def test_cpymod_only_setup_install(self):
        # CPython-only project, no hpy at all. This is a baseline to check
        # that everything works even without hpy.
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  ext_modules = [cpymod],
            )
        """)
        self.run('python', 'setup.py', 'install')
        out = self.run('python', '-c', 'import cpymod; print(cpymod.__doc__)',
                       capture=True)
        assert out == 'cpymod docstring'

    def test_cpymod_with_empty_hpy_ext_modules_setup_install(self):
        # if we have hpy_ext_modules=[] we trigger the hpy.devel monkey
        # patch. This checks that we don't ext_modules still works after that.
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

    def test_hpy_ext_modules_setup_install(self):
        # first basic HPy test
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  hpy_ext_modules = [hpymod],
            )
        """)
        self.run('python', 'setup.py', 'install')
        out = self.run('python', '-c', 'import hpymod; print(hpymod.__doc__)',
                       capture=True)
        assert out == 'hpymod CPython ABI'

    def test_hpy_ext_modules_build_platlib(self):
        # check that if we have only hpy_ext_modules, the distribution is
        # detected as "platform-specific" and not "platform-neutral". In
        # particular, we want the end result to be in
        # e.g. build/lib.linux-x86_64-3.8 and NOT in build/lib.
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  hpy_ext_modules = [hpymod],
                  install_requires = [],
            )
        """)
        self.run('python', 'setup.py', 'build')
        build = self.hpy_test_project.join('build')
        libs = build.listdir('lib*')
        assert len(libs) == 1
        libdir = libs[0]
        # this is something like lib.linux-x86_64-cpython-38
        assert libdir.basename != 'lib'

    @pytest.mark.xfail
    def test_dont_mix_cpython_and_universal_abis(self):
        # make sure that the build dirs for cpython and universal ABIs are
        # distinct
        self.gen_setup_py("""
            setup(name = "hpy_test_project",
                  hpy_ext_modules = [hpymod],
                  install_requires = [],
            )
        """)
        self.run('python', 'setup.py', 'install')
        out = self.run('python', '-c', 'import hpymod; print(hpymod.__doc__)',
                       capture=True)
        assert out == 'hpymod CPython ABI'
        #
        # now recompile with universal *without* cleaning the build
        self.run('python', 'setup.py', '--hpy-abi=universal', 'install')
        out = self.run('python', '-c', 'import hpymod; print(hpymod.__doc__)',
                       capture=True)
        assert out == 'hpymod Universal ABI'
