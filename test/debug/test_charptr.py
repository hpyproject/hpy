from ..support import SUPPORTS_SYS_EXECUTABLE
import os
import sys
import pytest

# Tests detection of usage of char pointers associated with invalid already
# closed handles. For now, the debug mode does not provide any hook for this
# error, so we have to run the tests from subprocess and check the return code.

SUPPORTS_MEM_PROTECTION = os.name == 'posix' and \
                          '_HPY_DEBUG_FORCE_DEFAULT_MEM_PROTECT' not in os.environ

@pytest.fixture
def hpy_abi():
    from hpy.debug import LeakDetector
    with LeakDetector():
        yield "debug"


@pytest.mark.skipif(not SUPPORTS_SYS_EXECUTABLE, reason="needs subprocess")
def test_charptr_use_after_implicit_arg_handle_close(compiler, python_subprocess):
    mod = compiler.compile_module("""
        const char *keep;

        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy_ssize_t size;
            keep = HPyUnicode_AsUTF8AndSize(ctx, arg, &size);
            return HPy_Dup(ctx, ctx->h_None);
        }

        HPyDef_METH(g, "g", g_impl, HPyFunc_NOARGS)
        static HPy g_impl(HPyContext *ctx, HPy self)
        {
            return HPyUnicode_FromString(ctx, keep);
        }

        @EXPORT(f)
        @EXPORT(g)
        @INIT
    """)
    if SUPPORTS_MEM_PROTECTION:
        code = "mod.f('leak me!'); mod.g()"
    else:
        code = "mod.f('leak me!'); assert mod.g() == 'leak me!'"
    result = python_subprocess.run(mod, code)
    assert result.returncode != 0
    assert result.stdout == b""
    if sys.implementation.name == 'pypy':
        assert b"RPython" in result.stderr
    elif SUPPORTS_MEM_PROTECTION:
        assert result.stderr == b""
    else:
        # The garbage we override the data with will cause this error
        assert b"UnicodeDecodeError" in result.stderr


@pytest.mark.skipif(not SUPPORTS_SYS_EXECUTABLE, reason="needs subprocess")
def test_charptr_use_after_handle_close(compiler, python_subprocess):
    mod = compiler.compile_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy arg_dup = HPy_Dup(ctx, arg);
            HPy_ssize_t size;
            const char *keep = HPyUnicode_AsUTF8AndSize(ctx, arg_dup, &size);
            HPy_Close(ctx, arg_dup);
            return HPyUnicode_FromString(ctx, keep);
        }

        @EXPORT(f)
        @INIT
    """)
    if SUPPORTS_MEM_PROTECTION:
        code = "mod.f('use after close me!')"
    else:
        code = "assert mod.f('use after close me!') == 'use after close me!'"
    result = python_subprocess.run(mod, code)
    assert result.returncode != 0
    assert result.stdout == b""
    if sys.implementation.name == 'pypy':
        assert b"RPython" in result.stderr
    elif SUPPORTS_MEM_PROTECTION:
        assert result.stderr == b""
    else:
        # The garbage we override the data with will cause this error
        assert b"UnicodeDecodeError" in result.stderr


@pytest.mark.skipif(not SUPPORTS_MEM_PROTECTION, reason=
                    "Could be implemented by checking the contents on "
                    "close, but long term it would be better to provide"
                    "proper protection on Windows in the future")
@pytest.mark.skipif(not SUPPORTS_SYS_EXECUTABLE, reason="needs subprocess")
def test_charptr_write_ptr(compiler, python_subprocess):
    mod = compiler.compile_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy_ssize_t size;
            char *keep = (char*) HPyUnicode_AsUTF8AndSize(ctx, arg, &size);
            keep[0] = 'a';
            return HPy_Dup(ctx, ctx->h_None);
        }

        @EXPORT(f)
        @INIT
    """)
    result = python_subprocess.run(mod, "mod.f('try writing me!');")
    if sys.implementation.name == 'pypy':
        assert result.returncode == 0
    else:
        assert result.returncode != 0
    assert result.stdout == b""
    assert result.stderr == b""


def test_charptr_correct_usage(compiler):
    mod = compiler.make_module("""
        #include <string.h>
        #include <stdio.h>

        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy arg_dup = HPy_Dup(ctx, arg);
            HPy_ssize_t size;
            const char *keep = HPyUnicode_AsUTF8AndSize(ctx, arg_dup, &size);
            char *copy = (char*) malloc(size + 1);
            memcpy(copy, keep, size + 1);
            HPy_Close(ctx, arg_dup);
            HPy result = HPyUnicode_FromString(ctx, copy);
            free(copy);
            return result;
        }

        @EXPORT(f)
        @INIT
    """)
    assert mod.f('I wont be leaked!') == 'I wont be leaked!';


def test_charptr_limit_stress_test(compiler):
    from hpy.universal import _debug
    mod = compiler.make_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy_ssize_t size;
            HPyUnicode_AsUTF8AndSize(ctx, arg, &size);
            if (size == 0) {
                return HPy_NULL;
            }
            return HPy_Dup(ctx, ctx->h_None);
        }

        // Dummy function just to force handle creation, but should not create
        // any raw data attached to those handles
        HPyDef_METH(g, "g", g_impl, HPyFunc_O)
        static HPy g_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            int len = HPyLong_AsLong(ctx, arg);
            for (int i = 0; i < len; ++i) {
                HPy h = HPyLong_FromLong(ctx, i);
                HPy_Close(ctx, h);
            }
            return HPy_Dup(ctx, ctx->h_None);
        }

        @EXPORT(f)
        @EXPORT(g)
        @INIT
    """)

    def get_raw_data_sizes(handles):
        return list(map(lambda h: h.raw_data_size, handles))

    def clear_raw_data_in_closed_handles():
        closed_size = len(_debug.get_closed_handles())
        old_limit = _debug.get_closed_handles_queue_max_size()
        try:
            # make sure that the closed_handles queue is considered full: this
            # will force the reuse of existing closed handles.
            _debug.set_closed_handles_queue_max_size(closed_size)
            # Dummy call to force the reuse of the existing closed handles
            # -2 because 'self' and the 'arg' should already reuse 2 handles
            mod.g(closed_size - 2)
        finally:
            _debug.set_closed_handles_queue_max_size(old_limit)
        return closed_size

    old_raw_data_max_size = _debug.get_protected_raw_data_max_size()
    old_closed_handles_max_size = _debug.get_closed_handles_queue_max_size()
    _debug.set_protected_raw_data_max_size(100)
    try:
        # Reset the state as much as possible:
        closed_size = clear_raw_data_in_closed_handles()
        # Make enough room for the handles created by this test
        _debug.set_closed_handles_queue_max_size(closed_size + 100)
        # Sanity check: no raw data is now held by closed handles
        initial = get_raw_data_sizes(_debug.get_closed_handles())
        assert all(map(lambda x: x == -1, initial))

        # Large string that shouldn't be retained at all
        gen = _debug.new_generation()
        mod.f('abc' * 50)
        closed1 = get_raw_data_sizes(_debug.get_closed_handles(gen))
        assert closed1 == [-1, -1, -1]  # -1 for 'self' and the return value

        # Two small strings, should be kept, one large that should not fit in
        gen = _debug.new_generation()
        mod.f('a' * 31)
        mod.f('b' * 32)
        mod.f('c' * 50)
        closed2 = get_raw_data_sizes(_debug.get_closed_handles(gen))
        # -1 for 'self'/return value for each call, and the long string
        # note: C strings' size is +1 for the terminating '\0'
        assert sorted(closed2) == [-1] * 7 + [32, 33]

        # Another small string should still fit
        gen = _debug.new_generation()
        mod.f('a' * 13)
        closed3 = get_raw_data_sizes(_debug.get_closed_handles(gen))
        # 'self'/return, and the expected small string
        assert sorted(closed3) == [-1, -1, 14]

        # But another small-ish string not anymore
        gen = _debug.new_generation()
        mod.f('a' * 27)
        closed4 = get_raw_data_sizes(_debug.get_closed_handles(gen))
        # 'self'/return, and the string whose raw data didn't fit in
        assert sorted(closed4) == [-1, -1, -1]

        # Check that raw data of closed handles is freed when the handle
        # is reused
        closed_size = clear_raw_data_in_closed_handles()

        # None of the closed handles should now have any raw data attached
        all_closed = _debug.get_closed_handles()
        assert len(all_closed) == closed_size  # Sanity check
        for h in all_closed:
            assert h.raw_data_size == -1
    finally:
        _debug.set_protected_raw_data_max_size(old_raw_data_max_size)
        _debug.set_closed_handles_queue_max_size(old_closed_handles_max_size)
