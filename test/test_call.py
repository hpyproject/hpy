from .support import HPyTest


class TestCall(HPyTest):
    def test_hpy_call(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(call, "call", call_impl, HPyFunc_VARARGS)
            static HPy call_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {

                HPy f, f_args;
                HPy f_kw = HPy_NULL;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "OO|O", &f, &f_args,
                                  &f_kw))
                    return HPy_NULL;
                return HPy_Call(ctx, f, f_args, f_kw);
            }
            @EXPORT(call)
            @INIT
        """)

        def f(a, b):
            return a + b

        assert mod.call(f, (1, 2)) == 3
        assert mod.call(f, (), {"a": 2, "b": 3}) == 5
        assert mod.call(str, (2,)) == "2"
        assert mod.call(str, (2,), {}) == "2"
        with pytest.raises(TypeError):
            mod.call(f, (1,))
        with pytest.raises(TypeError):
            mod.call(f, (), {"b": 2})
        with pytest.raises(TypeError):
            mod.call(f, (), {})
        with pytest.raises(TypeError):
            mod.call("not callable", (2,))
        with pytest.raises(TypeError):
            mod.call("not callable", (2,), {})

    def test_hpy_callobject(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(call, "call", call_impl, HPyFunc_VARARGS)
            static HPy call_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {

                HPy f;
                HPy f_args = HPy_NULL;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "O|O", &f, &f_args))
                    return HPy_NULL;
                return HPy_CallObject(ctx, f, f_args);
            }
            @EXPORT(call)
            @INIT
        """)

        def f(a, b):
            return a + b

        def g():
            return "this is g"

        assert mod.call(f, (1, 2)) == 3
        assert mod.call(g) == "this is g"
        assert mod.call(str, (2,)) == "2"
        with pytest.raises(TypeError):
            mod.call(f, (1,))
        with pytest.raises(TypeError):
            mod.call("not callable", (2,))

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
