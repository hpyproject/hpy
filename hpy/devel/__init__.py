from pathlib import Path
from setuptools import Extension

_BASE_DIR = Path(__file__).parent

class HPyDevel:
    def __init__(self, base_dir=_BASE_DIR):
        self.base_dir = Path(base_dir)
        self.include_dir = self.base_dir.joinpath('include')
        self.src_dir = self.base_dir.joinpath('src', 'runtime')
        # extra_sources are needed both in CPython and Universal mode
        self._extra_sources = [
            self.src_dir.joinpath('argparse.c')
            ]
        # ctx_sources are needed only in Universal mode
        self._ctx_sources = list(self.src_dir.glob('ctx_*.c'))
        self._ctx_sources.append(self.src_dir.joinpath('listbuilder.c')) # XXX

    def get_extra_sources(self):
        return list(map(str, self._extra_sources))

    def get_ctx_sources(self):
        return list(map(str, self._ctx_sources))

    def get_extension(self, name, **kwargs):
        """
        Create a setuptools.Extension() to compile an HPy module.

        The arguments are the same as the ones passed to setuptools.Extension,
        with the addition of the following hpy-only arguments:

          hpy_abi: either 'cpython' (default) or 'universal'
        """
        hpy_abi = kwargs.pop('hpy_abi', 'cpython')
        assert hpy_abi in ('cpython', 'universal')
        ext = Extension(name, **kwargs)
        if hpy_abi == 'universal':
            ext.define_macros.append(('HPY_UNIVERSAL_ABI', None))
        ext.sources += self.get_extra_sources()
        if hpy_abi == 'cpython':
            ext.sources += self.get_ctx_sources()
        #
        ext.include_dirs.append(str(self.include_dir))
        return ext



def HPyExtension(name, **kwargs):
    return HPyDevel().get_extension(name, **kwargs)
