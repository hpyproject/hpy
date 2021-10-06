from test.support import HPyDebugTest
import pytest

class TestCharPtr(HPyDebugTest):
    """
    Tests detection of usage of char pointers associated with invalid already
    closed handles. For now, the debug mode does not provide any hook for this
    error, so we have to run the tests from subprocess and check the return code.
    """

    def test_charptr_use_after_implicit_arg_handle_close(self):
        if not self.supports_sys_executable():
            pytest.skip("no sys.executable")

        mod = self.compile_module("""
            const char *keep;
        
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy_ssize_t size;
                keep = HPyUnicode_AsUTF8AndSize(ctx, arg, &size);
                return HPy_Dup(ctx, ctx->h_None);
            }
            
            HPyDef_METH(g, "g", g_impl, HPyFunc_NOARGS)
            static HPy g_impl(HPyContext *ctx, HPy self)
            {
                return HPyUnicode_FromString(ctx, keep);
            }

            @EXPORT(f)
            @EXPORT(g)
            @INIT
        """)
        result = self.run_python_subprocess(mod, "mod.f('leak me!'); mod.g()")
        assert result.returncode != 0

    def test_charptr_use_after_handle_close(self):
        if not self.supports_sys_executable():
            pytest.skip("no sys.executable")

        mod = self.compile_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {                
                HPy arg_dup = HPy_Dup(ctx, arg);
                HPy_ssize_t size;
                const char *keep = HPyUnicode_AsUTF8AndSize(ctx, arg_dup, &size);
                HPy_Close(ctx, arg_dup);
                return HPyUnicode_FromString(ctx, keep);
            }

            @EXPORT(f)
            @INIT
        """)
        result = self.run_python_subprocess(mod, "mod.f('use after close me!');")
        assert result.returncode != 0

    def test_charptr_write_ptr(self):
        if not self.supports_sys_executable():
            pytest.skip("no sys.executable")

        mod = self.compile_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {                
                HPy_ssize_t size;
                char *keep = (char*) HPyUnicode_AsUTF8AndSize(ctx, arg, &size);
                keep[0] = 'a';                
                return HPy_Dup(ctx, ctx->h_None);
            }

            @EXPORT(f)
            @INIT
        """)
        result = self.run_python_subprocess(mod, "mod.f('try writing me!');")
        assert result.returncode != 0

    def test_charptr_correct_usage(self):
        if not self.supports_sys_executable():
            pytest.skip("no sys.executable")

        mod = self.compile_module("""
            #include <string.h>
            #include <stdio.h>
            
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {                
                HPy arg_dup = HPy_Dup(ctx, arg);
                HPy_ssize_t size;
                const char *keep = HPyUnicode_AsUTF8AndSize(ctx, arg_dup, &size);
                char *copy = (char*) malloc(size + 1);
                memcpy(copy, keep, size + 1);
                HPy_Close(ctx, arg_dup);
                HPy result = HPyUnicode_FromString(ctx, copy);
                free(copy);
                return result;
            }

            @EXPORT(f)
            @INIT
        """)
        result = self.run_python_subprocess(mod, "mod.f('I wont be leaked!');")
        assert result.returncode == 0
