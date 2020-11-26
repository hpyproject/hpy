import os.path
from pathlib import Path
from distutils import log
from distutils.command.build import build
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

    def fix_distribution(self, dist):
        """ Override build_ext to support hpy modules.

            Used from both setup.py and hpy/test.
        """
        dist.hpydevel = self

        base_build = dist.cmdclass.get("build", build)
        base_build_ext = dist.cmdclass.get("build_ext", build_ext)

        class build_hpy_ext(build_hpy_ext_mixin, base_build_ext):
            _base_build_ext = base_build_ext

        def dist_has_ext_modules(self):
            if self.ext_modules or self.hpy_ext_modules:
                return True
            return False

        def build_has_ext_modules(self):
            return self.distribution.has_ext_modules()

        # replace build_ext subcommand
        dist.cmdclass['build_ext'] = build_hpy_ext
        dist.__class__.has_ext_modules = dist_has_ext_modules
        base_build.has_ext_modules = build_has_ext_modules
        idx = [sub[0] for sub in base_build.sub_commands].index("build_ext")
        base_build.sub_commands[idx] = ("build_ext", build_has_ext_modules)


def handle_hpy_ext_modules(dist, attr, hpy_ext_modules):
    """ Distuils hpy_ext_module setup(...) argument and --hpy-abi option.

        See hpy's setup.py where this function is registered as an entry
        point.
    """
    assert attr == 'hpy_ext_modules'

    # add a global option --hpy-abi to setup.py
    dist.__class__.hpy_abi = 'cpython'
    dist.__class__.global_options += [
        ('hpy-abi=', None, 'Specify the HPy ABI mode (default: cpython)')
    ]
    hpydevel = HPyDevel()
    hpydevel.fix_distribution(dist)


_HPY_UNIVERSAL_MODULE_STUB_TEMPLATE = """
class Spec:
    def __init__(self, name, origin):
        self.name = name
        self.origin = origin


def __bootstrap__():
    import os, sys
    from hpy.universal import load_from_spec
    ext_filepath = os.path.join(os.path.dirname(__file__), {ext_file!r})
    m = load_from_spec(Spec({module_name!r}, ext_filepath))
    m.__file__ = ext_filepath
    sys.modules[__name__] = m

__bootstrap__()
"""


class HPyExtensionName(str):
    def split(self, *args, **kw):
        result = str.split(self, *args, **kw)
        return [self.__class__(s) for s in result]


class build_hpy_ext_mixin:
    """ A mixin class for setuptools build_ext to add support for buidling
        HPy extensions.
    """

    _extensions = None

    @property
    def extensions(self):
        return self._extensions

    @extensions.setter
    def extensions(self, value):
        pass  # ignore any attempts to change the list of extensions directly

    def initialize_options(self):
        self._base_build_ext.initialize_options(self)
        self.hpydevel = self.distribution.hpydevel

    def _finalize_hpy_ext(self, ext):
        if hasattr(ext, "hpy_abi"):
            return
        ext.name = HPyExtensionName(ext.name)
        ext.hpy_abi = self.distribution.hpy_abi
        ext.include_dirs += self.hpydevel.get_extra_include_dirs()
        ext.sources += self.hpydevel.get_extra_sources()
        if ext.hpy_abi == 'cpython':
            ext.sources += self.hpydevel.get_ctx_sources()
            ext._hpy_needs_stub = False
        if ext.hpy_abi == 'universal':
            ext.define_macros.append(('HPY_UNIVERSAL_ABI', None))
            ext._hpy_needs_stub = True

    def finalize_options(self):
        self._extensions = self.distribution.ext_modules or []
        hpy_ext_modules = self.distribution.hpy_ext_modules or []
        for ext in hpy_ext_modules:
            self._finalize_hpy_ext(ext)
        self._extensions.extend(hpy_ext_modules)
        self._base_build_ext.finalize_options(self)
        for ext in hpy_ext_modules:
            ext._needs_stub = ext._hpy_needs_stub

    def get_ext_fullname(self, ext_name):
        fullname = self._base_build_ext.get_ext_fullname(self, ext_name)
        if isinstance(ext_name, HPyExtensionName):
            fullname = HPyExtensionName(fullname)
        return fullname

    def get_ext_fullpath(self, ext_name):
        fullpath = self._base_build_ext.get_ext_fullpath(self, ext_name)
        if isinstance(ext_name, HPyExtensionName):
            fullpath = HPyExtensionName(fullpath)
        return fullpath

    def get_ext_filename(self, ext_name):
        if not isinstance(ext_name, HPyExtensionName):
            return self._base_build_ext.get_ext_filename(self, ext_name)
        if self.distribution.hpy_abi == 'universal':
            ext_path = ext_name.split('.')
            ext_suffix = '.hpy.so'  # XXX Windows?
            ext_filename = os.path.join(*ext_path) + ext_suffix
        else:
            ext_filename = self._base_build_ext.get_ext_filename(
                self, ext_name)
        return HPyExtensionName(ext_filename)

    def write_stub(self, output_dir, ext, compile=False):
        if (not hasattr(ext, "hpy_abi") or
                self.distribution.hpy_abi != 'universal'):
            return self._base_build_ext.write_stub(
                self, output_dir, ext, compile=compile)
        log.info(
            "writing hpy universal stub loader for %s to %s",
            ext._full_name, output_dir)
        # ignore output_dir which points to completely the wrong place
        output_dir = self.build_lib
        stub_file = (os.path.join(output_dir, *ext._full_name.split('.')) +
                     '.py')
        if compile and os.path.exists(stub_file):
            raise DistutilsError(stub_file + " already exists! Please delete.")
        ext_file = os.path.basename(ext._file_name)
        module_name = ext_file.split(".")[0]
        if not self.dry_run:
            with open(stub_file, 'w') as f:
                f.write(_HPY_UNIVERSAL_MODULE_STUB_TEMPLATE.format(
                    ext_file=ext_file, module_name=module_name)
                )
