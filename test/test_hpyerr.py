from .support import HPyTest

class TestErr(HPyTest):

    def test_NoMemory(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext ctx, HPy self)
            {
                return HPyErr_NoMemory(ctx);
            }
            @EXPORT(f)
            @INIT
        """)
        with pytest.raises(MemoryError):
            mod.f()

    def test_FatalError(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext ctx, HPy self)
            {
                HPy_FatalError(ctx, "boom!");
                // note: no 'return' statement.  This also tests that
                // the call above is known to never return---otherwise,
                // we get a warning from the missing 'return' and it is
                // turned into an error.
            }
            @EXPORT(f)
            @INIT
        """)
        # Calling mod.f() gives a fatal error, ending in abort().
        # How to check that?  For now we just check that the above compiles

    def test_HPyErr_Occurred(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPyLong_AsLong(ctx, arg);
                if (HPyErr_Occurred(ctx)) {
                    HPyErr_SetString(ctx, ctx->h_ValueError, "hello world");
                    return HPy_NULL;
                }
                return HPyLong_FromLong(ctx, -1002);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(-10) == -1002
        with pytest.raises(ValueError) as exc:
            mod.f("not an integer")
        assert str(exc.value) == 'hello world'

    def test_HPyErr_Cleared(self):
        import sys
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext ctx, HPy self)
            {
                HPyErr_SetString(ctx, ctx->h_ValueError, "hello world");
                HPyErr_Clear(ctx);
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f() is None
        assert sys.exc_info() == (None, None, None)

    def test_h_exceptions(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy h_dict, h_err;
                h_dict = HPyDict_New(ctx);
                HPy_SetItem_s(ctx, h_dict, "Exception", ctx->h_Exception);
                HPy_SetItem_s(ctx, h_dict, "IndexError", ctx->h_IndexError);
                HPy_SetItem_s(ctx, h_dict, "OverflowError", ctx->h_OverflowError);
                HPy_SetItem_s(ctx, h_dict, "SystemError", ctx->h_SystemError);
                HPy_SetItem_s(ctx, h_dict, "TypeError", ctx->h_TypeError);
                HPy_SetItem_s(ctx, h_dict, "ValueError", ctx->h_ValueError);
                h_err = HPy_GetItem(ctx, h_dict, arg);
                HPyErr_SetString(ctx, h_err, "error message");
                HPy_Close(ctx, h_dict);
                HPy_Close(ctx, h_err);
                return HPy_NULL;
            }
            @EXPORT(f)
            @INIT
        """)

        def check_exception(cls):
            with pytest.raises(cls):
                mod.f(cls.__name__)

        check_exception(Exception)
        check_exception(IndexError)
        check_exception(OverflowError)
        check_exception(SystemError)
        check_exception(TypeError)
        check_exception(ValueError)
