import pytest
from .support import ExtensionCompiler
from hpy.debug.pytest import hpy_debug # make it available to all tests

def pytest_addoption(parser):
    parser.addoption(
        "--compiler-v", action="store_true",
        help="Print to stdout the commands used to invoke the compiler")
    parser.addoption(
        "--subprocess-v", action="store_true",
        help="Print to stdout the stdout and stderr of Python subprocesses"
             "executed via run_python_subprocess")

@pytest.fixture(scope='session')
def hpy_devel(request):
    from hpy.devel import HPyDevel
    return HPyDevel()

@pytest.fixture(params=['cpython', 'universal', 'debug'])
def hpy_abi(request):
    return request.param

@pytest.fixture
def compiler(request, tmpdir, hpy_devel, hpy_abi):
    compiler_verbose = request.config.getoption('--compiler-v')
    return ExtensionCompiler(tmpdir, hpy_devel, hpy_abi,
                             compiler_verbose=compiler_verbose)

@pytest.fixture(scope="session")
def fatal_exit_code(request):
    import sys
    return {
        "linux": -6,  # SIGABRT
        # See https://bugs.python.org/issue36116#msg336782 -- the
        # return code from abort on Windows 8+ is a stack buffer overrun.
        # :|
        "win32": 0xC0000409,  # STATUS_STACK_BUFFER_OVERRUN
    }.get(sys.platform, -6)