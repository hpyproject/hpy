from .support import HPyTest

class TestString(HPyTest):
    def test_unicode_builder(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy h_self)
            {
                HPyUnicodeBuilder builder = HPyUnicodeBuilder_New(ctx, 0);
                if(HPy_IsNull(builder)) {
                    HPyErr_SetString(ctx, ctx->h_RuntimeError, "Could not create HPyUnicodeBuilder");
                    return HPy_NULL;
                }
                HPyUnicodeBuilder_Add(ctx, builder, "hello ");
                HPyUnicodeBuilder_Add(ctx, builder, "world");
                HPyUnicodeBuilder_Add(ctx, builder, "!");
                HPy h_string = HPyUnicodeBuilder_Build(ctx, builder);
                return h_string;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f() == "hello world!"
