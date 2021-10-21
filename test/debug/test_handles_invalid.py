import pytest
from hpy.debug.leakdetector import LeakDetector
from test.support import SUPPORTS_SYS_EXECUTABLE

@pytest.fixture
def hpy_abi():
    with LeakDetector():
        yield "debug"


def test_cant_use_closed_handle(compiler, hpy_debug_capture):
    mod = compiler.make_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O, .doc="double close")
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy h = HPy_Dup(ctx, arg);
            HPy_Close(ctx, h);
            HPy_Close(ctx, h); // double close
            return HPy_Dup(ctx, ctx->h_None);
        }

        HPyDef_METH(g, "g", g_impl, HPyFunc_O, .doc="use after close")
        static HPy g_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy h = HPy_Dup(ctx, arg);
            HPy_Close(ctx, h);
            return HPy_Repr(ctx, h);
        }

        @EXPORT(f)
        @EXPORT(g)
        @INIT
    """)
    mod.f('foo')   # double close
    assert hpy_debug_capture.invalid_handles_count == 1
    mod.g('bar')   # use-after-close
    assert hpy_debug_capture.invalid_handles_count == 2


def test_invalid_handle_crashes_python_if_no_hook(compiler, python_subprocess, fatal_exit_code):
    if not SUPPORTS_SYS_EXECUTABLE:
        pytest.skip("no sys.executable")

    mod = compiler.compile_module("""
        HPyDef_METH(f, "f", f_impl, HPyFunc_O, .doc="double close")
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            HPy h = HPy_Dup(ctx, arg);
            HPy_Close(ctx, h);
            HPy_Close(ctx, h); // double close
            return HPy_Dup(ctx, ctx->h_None);
        }

        @EXPORT(f)
        @INIT
    """)
    result = python_subprocess.run(mod, "mod.f(42);")
    assert result.returncode == fatal_exit_code