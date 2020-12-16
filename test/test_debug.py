from .support import HPyTest


class TestDebug(HPyTest):

    def test_ctx_name(self, hpy_abi):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext ctx, HPy self)
            {
                return HPyUnicode_FromString(ctx, ctx->name);
            }

            @EXPORT(f)
            @INIT
        """)
        ctx_name = mod.f()
        if hpy_abi == 'cpython':
            assert ctx_name == 'HPy CPython ABI'
        elif hpy_abi == 'universal':
            # this can be "HPy Universal ABI (CPython backend)" or
            # "... (PyPy backend)", etc.
            assert ctx_name.startswith('HPy Universal ABI')
        elif hpy_abi == 'debug':
            assert ctx_name.startswith('HPy Debug Mode ABI')
        else:
            assert False, 'unexpected hpy_abi: %s' % hpy_abi


    def test_temp(self, hpy_abi):
        import pytest
        if hpy_abi != 'universal':
            pytest.skip()
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPyContext ctx2 = (HPyContext)HPyLong_AsLong(ctx, arg);
                HPy a = HPyLong_FromLong(ctx, 4);
                HPy b = HPyLong_FromLong(ctx, 5);
                HPy res = HPy_Add(ctx2, a, b); // note, we are using ctx2!
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return res;
            }
            @EXPORT(f)
            @INIT
        """)
        from hpy.debug._ctx import get_debug_ctx
        ctx2 = get_debug_ctx()
        res = mod.f(ctx2)
        assert res == 9
