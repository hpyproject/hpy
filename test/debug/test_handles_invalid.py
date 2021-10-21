import pytest
from hpy.debug.leakdetector import LeakDetector

@pytest.fixture
def hpy_abi():
    with LeakDetector():
        yield "debug"


def test_cant_use_closed_handle(compiler):
    from hpy.universal import _debug
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
    n = 0
    def callback():
        nonlocal n
        n += 1
    _debug.set_on_invalid_handle(callback)
    mod.f('foo')   # double close
    assert n == 1
    mod.g('bar')   # use-after-close
    assert n == 2

