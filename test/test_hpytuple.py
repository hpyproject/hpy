from .support import HPyTest

class TestTuple(HPyTest):

    def test_FromArray(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy x = HPyLong_FromLong(ctx, 42);
                if (HPy_IsNull(x))
                     return HPy_NULL;
                HPy items[] = {self, arg, x};
                HPy res = HPyTuple_FromArray(ctx, items, 3);
                HPy_Close(ctx, x);
                return res;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f('hello') == (mod, 'hello', 42)

    def test_Pack(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy x = HPyLong_FromLong(ctx, 42);
                if (HPy_IsNull(x))
                     return HPy_NULL;
                return HPyTuple_Pack(ctx, 3, self, arg, x);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f('hello') == (mod, 'hello', 42)
