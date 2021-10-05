from test.support import HPyDebugTest


class TestHandlesInvalid(HPyDebugTest):
    """
    Tests detection of invalid closed handles. We keep the handles leaks check,
    because they are not intended to happen in these tests.
    """

    def test_no_invalid_handle(self, hpy_debug_capture):
        # Basic sanity check that valid code does not trigger any error reports
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy x = HPyLong_FromLong(ctx, 42);
                HPy y = HPyLong_FromLong(ctx, 2);
                HPy arg_dup = HPy_Dup(ctx, arg);
                HPy_Close(ctx, y);
                HPy b = HPy_Dup(ctx, x);
                HPy_Close(ctx, x);
                HPy_Close(ctx, arg_dup);
                return b;
            }

            @EXPORT(f)
            @INIT
        """)
        assert mod.f("hello") == 42
        assert mod.f("world") == 42
        assert hpy_debug_capture.invalid_handles_count == 0

    def test_keeping_and_reusing_argument_handle(self, hpy_debug_capture):
        mod = self.make_module("""
            HPy keep;

            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                keep = arg;
                return HPy_Dup(ctx, ctx->h_None);
            }

            HPyDef_METH(g, "g", g_impl, HPyFunc_NOARGS)
            static HPy g_impl(HPyContext *ctx, HPy self)
            {
                return keep;
            }

            @EXPORT(f)
            @EXPORT(g)
            @INIT
        """)
        mod.f("hello leaks!")
        assert hpy_debug_capture.invalid_handles_count == 0
        mod.g()
        assert hpy_debug_capture.invalid_handles_count == 1

    def test_cant_use_closed_handle(self, hpy_debug_capture):
        mod = self.make_module("""
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