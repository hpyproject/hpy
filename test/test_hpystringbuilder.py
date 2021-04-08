from .support import HPyTest

class TestString(HPyTest):
    def test_StringBuilder(self, hpy_abi):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy h_self, HPy h_arg)
            {
                HPyStringBuilder builder = HPyStringBuilder_New(ctx);
                HPy h_s1 = HPyUnicode_FromString(ctx, "hello ");
                HPy h_s2 = HPyUnicode_FromString(ctx, "!");
                HPyStringBuilder_Append(ctx, builder, h_s1);
                HPyStringBuilder_Append(ctx, builder, h_arg);
                HPyStringBuilder_Append(ctx, builder, h_s2);
                HPy h_string = HPyStringBuilder_Build(ctx, builder);
                HPy_Close(ctx, h_s1);
                HPy_Close(ctx, h_s2);
                return h_string;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f("world") == "hello world!"
