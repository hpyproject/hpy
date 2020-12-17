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
