import os
import os.path
import re
import subprocess
import sys

import simple
import mixed
import hpyvarargs
import snippets


def test_simple_abs():
    assert simple.myabs(-42) == 42
    assert simple.myabs(42) == 42


def test_hpyvarargs():
    assert hpyvarargs.add_ints(40, 2) == 42


def test_mixed_add_ints():
    assert mixed.add_ints_legacy(40, 2) == 42
    assert mixed.add_ints(40, 2) == 42


def test_snippets():
    x = 2
    assert snippets.test_foo_and_is_same_object(x, x) == 1
    assert snippets.test_foo_and_is_same_object(x, 42) == 0


def test_leak_detector():
    from hpy.debug.pytest import LeakDetector
    with LeakDetector() as ld:
        # add_ints is an HPy C function. If it forgets to close a handle,
        # LeakDetector will complain
        assert mixed.add_ints(40, 2) == 42
# END: test_leak_detector

from hpy.debug.pytest import hpy_debug
def test_that_uses_leak_detector_fixture(hpy_debug):
    # Run some HPy extension code
    assert mixed.add_ints(40, 2) == 42


def test_leak_detector_with_traces():
    import hpy.debug
    hpy.debug.set_handle_stack_trace_limit(16)
    assert mixed.add_ints(40, 2) == 42
    hpy.debug.disable_handle_stack_traces()


def test_leak_detector_with_traces_output():
    # Update the debug documentation if anything here changes!
    env = os.environ.copy()
    env['HPY_DEBUG'] = '1'
    env['HPY_LOG'] = '1'
    script = os.path.join(os.path.dirname(__file__), 'debug-example.py')
    result = subprocess.run([sys.executable, script], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Rudimentary check that the output contains what we have in the documentation
    out = result.stdout.decode('latin-1')
    assert out == "Loading 'snippets' in HPy universal mode with a debug context" + os.linesep
    err = result.stderr.decode('latin-1')
    assert 'hpy.debug.leakdetector.HPyLeakError: 1 unclosed handle:' in err
    assert re.search('<DebugHandle 0x[\\da-h]* for 42>', err)
    assert 'Allocation stacktrace:' in err
    if sys.platform.startswith(("linux", "darwin")):
        assert 'snippets.hpy.so' in err  # Should be somewhere in the stack trace
    else:
        assert 'At the moment this is only supported on Linux with glibc' in err
