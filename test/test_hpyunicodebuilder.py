from .support import HPyTest

class TestString(HPyTest):
    def test_unicode_builder(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy h_self, HPy h_arg)
            {
                HPyUnicodeBuilder builder = HPyUnicodeBuilder_New(ctx, 0);
                if(HPy_IsNull(builder)) {
                    HPyErr_SetString(ctx, ctx->h_RuntimeError, "Could not create HPyUnicodeBuilder");
                    return HPy_NULL;
                }
                HPy h_s1 = HPyUnicode_FromString(ctx, "hello ");
                HPy h_s2 = HPyUnicode_FromString(ctx, "!");
                HPyUnicodeBuilder_Add(ctx, builder, h_s1);
                HPyUnicodeBuilder_Add(ctx, builder, h_arg);
                HPyUnicodeBuilder_Add(ctx, builder, h_s2);
                HPy h_string = HPyUnicodeBuilder_Build(ctx, builder);
                HPy_Close(ctx, h_s1);
                HPy_Close(ctx, h_s2);
                return h_string;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f("world") == "hello world!"

    def test_type_error(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy h_self, HPy h_arg)
            {
                HPyUnicodeBuilder builder = HPyUnicodeBuilder_New(ctx, 0);
                if(HPy_IsNull(builder)) {
                    HPyErr_SetString(ctx, ctx->h_RuntimeError, "Could not create HPyUnicodeBuilder");
                    return HPy_NULL;
                }
                HPy h_long = HPyLong_FromLong(ctx, 42);
                HPyUnicodeBuilder_Add(ctx, builder, h_long);
                HPy_Close(ctx, h_long);
                HPyUnicodeBuilder_Cancel(ctx, builder);
                return HPy_NULL;
            }
            @EXPORT(f)
            @INIT
        """)
        import pytest
        with pytest.raises(TypeError) as exc_info:
            mod.f("world")
        assert exc_info.match("Argument must be of type HPyUnicode")
