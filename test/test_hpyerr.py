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

    def check_exception_constant(self, cls):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext ctx, HPy self)
            {{
                HPyErr_SetString(ctx, ctx->h_{cls_name}, "error message");
                return HPy_NULL;
            }}
            @EXPORT(f)
            @INIT
        """.format(cls_name=cls.__name__))
        with pytest.raises(cls):
            mod.f()

    def test_h_Exception(self):
        self.check_exception_constant(Exception)

    def test_h_IndexError(self):
        self.check_exception_constant(IndexError)

    def test_h_OverflowError(self):
        self.check_exception_constant(OverflowError)

    def test_h_SystemError(self):
        self.check_exception_constant(SystemError)

    def test_h_TypeError(self):
        self.check_exception_constant(TypeError)

    def test_h_ValueError(self):
        self.check_exception_constant(ValueError)
