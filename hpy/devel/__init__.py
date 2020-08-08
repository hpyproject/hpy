from pathlib import Path

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
        return self._extra_sources[:]

    def get_ctx_sources(self):
        return self._ctx_sources[:]
