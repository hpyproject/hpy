from .support import HPyTest

class TestIter(HPyTest):

    def test_check(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                if (HPyIter_Check(ctx, arg))
                    return HPy_Dup(ctx, ctx->h_True);
                return HPy_Dup(ctx, ctx->h_False);
            }
            @EXPORT(f)
            @INIT
        """)

        class MyList(list):
            pass

        assert mod.f(iter((1,))) is True
        assert mod.f(iter([])) is True
        assert mod.f(iter('hello')) is True
        assert mod.f(iter(MyList())) is True
        assert mod.f(object()) is False
        assert mod.f(10) is False
