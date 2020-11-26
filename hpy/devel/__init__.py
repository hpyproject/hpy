import os.path
from pathlib import Path
from distutils import log
from distutils.command.build import build
from distutils.command.install_lib import install_lib
from distutils.errors import DistutilsError
from setuptools.command.build_ext import build_ext

# NOTE: this file is also imported by PyPy tests, so it must be compatible
# with both Python 2.7 and Python 3.x


class HPyDevel:
    """ Extra sources for building HPy extensions with hpy.devel. """

    _BASE_DIR = Path(__file__).parent

    def __init__(self, base_dir=_BASE_DIR):
        self.base_dir = Path(base_dir)
        self.include_dir = self.base_dir.joinpath('include')
        self.src_dir = self.base_dir.joinpath('src', 'runtime')

    def get_extra_include_dirs(self):
        """ Extra include directories needed by extensions in both CPython and
            Universal modes.
        """
        return list(map(str, [
            self.include_dir,
        ]))

    def get_extra_sources(self):
        """ Extra sources needed by extensions in both CPython and Universal
            modes.
        """
        return list(map(str, [
            self.src_dir.joinpath('argparse.c'),
        ]))

    def get_ctx_sources(self):
        """ Extra sources needed only in Universal mode.
        """
        return list(map(str, self.src_dir.glob('ctx_*.c')))


def handle_hpy_ext_modules(dist, attr, hpy_ext_modules):
    """ Distuils hpy_ext_module setup(...) argument and --hpy-abi option.

        See hpy's setup.py where this function is registered as an entry
        point.
    """
    assert attr == 'hpy_ext_modules'

    base_build = dist.cmdclass.get("build", build)
    base_install_lib = dist.cmdclass.get("install_lib", install_lib)
    orig_has_ext_modules = base_build.has_ext_modules
    orig_get_outputs = base_install_lib.get_outputs
    orig_get_inputs = base_install_lib.get_inputs

    def has_hpy_ext_modules(self):
        return bool(self.distribution.hpy_ext_modules)

    def has_any_ext_modules(self):
        return has_hpy_ext_modules(self) or orig_has_ext_modules(self)

    def get_outputs(self):
        outputs = orig_get_outputs(self)
        hpy_ext_outputs = self._mutate_outputs(
            self.distribution.has_hpy_ext_modules(),
            'build_hpy_ext', 'build_lib',
            self.install_dir)
        outputs.extend(hpy_ext_outputs)
        return outputs

    def get_inputs(self):
        inputs = orig_get_inputs(self)
        if self.distribution.has_hpy_ext_modules():
            build_hpy_ext = self.get_finalized_command('build_hpy_ext')
            inputs.extend(build_hpy_ext.get_outputs())
        return inputs

    if not hasattr(dist.__class__, 'hpy_abi'):
        # add a global option --hpy-abi to setup.py
        dist.__class__.hpy_abi = 'cpython'
        dist.__class__.global_options += [
            ('hpy-abi=', None, 'Specify the HPy ABI mode (default: cpython)')
        ]
        # add build_hpy_ext subcommand
        base_build.sub_commands.append(("build_hpy_ext", has_hpy_ext_modules))
        base_build.has_ext_modules = has_any_ext_modules
        base_install_lib.get_outputs = get_outputs
        base_install_lib.get_inputs = get_inputs


class build_hpy_ext(build_ext):
    """ Distutils command for building HPy extensions.

        See hpy's setup.py where this class is registered as an entry point.
    """

    description = "build HPy C extensions"

    @property
    def extensions(self):
        return self.distribution.hpy_ext_modules

    @extensions.setter
    def extensions(self, value):
        pass  # ignore any attempts to change the list of extensions

    def initialize_options(self):
        super(self.__class__, self).initialize_options()
        self.hpydevel = HPyDevel()

    def get_outputs(self):
        outputs = super(self.__class__, self).get_outputs()
        print(outputs)
        return outputs

    def build_extension(self, ext):
        if not hasattr(ext, "hpy_abi"):
            ext.hpy_abi = self.distribution.hpy_abi
            ext.include_dirs += self.hpydevel.get_extra_include_dirs()
            ext.sources += self.hpydevel.get_extra_sources()
            if ext.hpy_abi == 'cpython':
                ext.sources += self.hpydevel.get_ctx_sources()
            if ext.hpy_abi == 'universal':
                ext.define_macros.append(('HPY_UNIVERSAL_ABI', None))
                ext._needs_stub = True
        return super(self.__class__, self).build_extension(ext)

    def get_ext_filename(self, ext_name):
        # this is needed to give the .hpy.so extension to universal extensions
        if self.distribution.hpy_abi == 'universal':
            ext_path = ext_name.split('.')
            ext_suffix = '.hpy.so'  # XXX Windows?
            ext_filename = os.path.join(*ext_path) + ext_suffix
            log.info("Building extension: %r" % ext_filename)
            return ext_filename
        return super(self.__class__, self).get_ext_filename(ext_name)

    def write_stub(self, output_dir, ext, compile=False):
        log.info("writing stub loader for %s to %s", ext._full_name,
                 output_dir)
        stub_file = (os.path.join(output_dir, *ext._full_name.split('.')) +
                     '.py')
        if compile and os.path.exists(stub_file):
            raise DistutilsError(stub_file + " already exists! Please delete.")
        ext_file = os.path.basename(ext._file_name)
        module_name = ext_file.split(".")[0]
        if not self.dry_run:
            f = open(stub_file, 'w')
            f.write("""
class Spec:
    def __init__(self, name, origin):
        self.name = name
        self.origin = origin


def __bootstrap__():
    global __loader__, __file__
    import sys, pkg_resources
    from hpy.universal import load_from_spec
    __file__ = pkg_resources.resource_filename(__name__, {ext_file!r})
    m = load_from_spec(Spec({module_name!r}, __file__))
    sys.modules[__name__] = m

__bootstrap__()
""".format(ext_file=ext_file, module_name=module_name))
            f.close()
