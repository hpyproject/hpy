import pytest
from .support import ExtensionCompiler

@pytest.fixture(params=['cpython', 'universal'])
def abimode(request):
    return request.param

@pytest.fixture
def compiler(tmpdir, abimode):
    return ExtensionCompiler(tmpdir, abimode)
