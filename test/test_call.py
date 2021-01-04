from .support import HPyTest


class TestCall(HPyTest):
    def test_hpy_calltupledict(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(call, "call", call_impl, HPyFunc_KEYWORDS)
            static HPy call_impl(HPyContext ctx, HPy self,
                                 HPy *args, HPy_ssize_t nargs, HPy kw)
            {

                HPy f, result;
                HPy f_args = HPy_NULL;
                HPy f_kw = HPy_NULL;
                HPyTracker ht;
                static const char *kwlist[] = { "f", "args", "kw", NULL };
                if (!HPyArg_ParseKeywords(ctx, &ht, args, nargs, kw, "O|OO",
                                          kwlist, &f, &f_args, &f_kw)) {
                    return HPy_NULL;
                }
                result = HPy_CallTupleDict(ctx, f, f_args, f_kw);
                HPyTracker_Close(ctx, ht);
                return result;
            }
            @EXPORT(call)
            @INIT
        """)

        def f(a, b):
            return a + b

        def g():
            return "this is g"

        # test calls with args (both with and without keywords)
        assert mod.call(f, args=(1, 2)) == 3
        assert mod.call(f, args=(), kw={"a": 2, "b": 3}) == 5
        assert mod.call(g, (), {}) == "this is g"
        assert mod.call(str, args=(2,)) == "2"
        assert mod.call(str, args=(2,), kw={}) == "2"
        with pytest.raises(TypeError):
            mod.call(f, args=(1,))
        with pytest.raises(TypeError):
            mod.call(f, args=(), kw={"b": 2})
        with pytest.raises(TypeError):
            mod.call(f, args=(), kw={})
        with pytest.raises(TypeError):
            mod.call("not callable", args=(2,))
        with pytest.raises(TypeError):
            mod.call("not callable", args=(2,), kw={})

        # test calls without args (both and without keywords)
        assert mod.call(g) == "this is g"
        assert mod.call(g, kw={}) == "this is g"

    def test_hpycallable_check(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                if (HPyCallable_Check(ctx, arg))
                    return HPy_Dup(ctx, ctx->h_True);
                return HPy_Dup(ctx, ctx->h_False);
            }
            @EXPORT(f)
            @INIT
        """)

        def f():
            return "this is f"

        assert mod.f(f) is True
        assert mod.f(str) is True
        assert mod.f("a") is False
        assert mod.f(3) is False
