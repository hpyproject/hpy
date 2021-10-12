from test.support import HPyDebugTest
import os
import pytest

SUPPORTS_MEM_PROTECTION = os.name == 'posix' and \
                          '_HPY_DEBUG_FORCE_DEFAULT_MEM_PROTECT' not in os.environ


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
        if SUPPORTS_MEM_PROTECTION:
            code = "mod.f('leak me!'); mod.g()"
        else:
            code = "mod.f('leak me!'); assert mod.g() == 'leak me!'"
        result = self.run_python_subprocess(mod, code)
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
        if SUPPORTS_MEM_PROTECTION:
            code = "mod.f('use after close me!')"
        else:
            code = "assert mod.f('use after close me!') == 'use after close me!'"
        result = self.run_python_subprocess(mod, code)
        assert result.returncode != 0

    @pytest.mark.skipif(not SUPPORTS_MEM_PROTECTION, reason=
                        "Could be implemented by checking the contents on "
                        "close, but long term it would be better to provide"
                        "proper protection on Windows in the future")
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

        mod = self.make_module("""
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
        assert mod.f('I wont be leaked!') == 'I wont be leaked!';

    def test_charptr_stress_test(self):
        # Stress test of the limit on the size of the "leaked" strings, i.e.,
        # associated with already closed handles
        mod = self.make_module("""
            #include <string.h>
            #include <stdio.h>
            #define N 100
            #define STR_SIZE (1024 * 1024)
            // define log(...) printf(__VA_ARGS__)
            #define log(...)

            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                HPy *handles = (HPy*)malloc(sizeof(HPy) * N);
                for (size_t i = 0; i < N; ++i) {
                    handles[i] = HPy_NULL;
                }

                HPy result;
                for (HPy_ssize_t i = 0; i < N; ++i) {
                    // create some random-ish string
                    HPy_ssize_t str_len = STR_SIZE + (i % 23);
                    log("\\nIteration %zd, creating string of size %zd\\n", i, str_len);
                    char *string = (char*) malloc(str_len + 1);
                    string[str_len] = '\\0';
                    for (HPy_ssize_t str_idx = 0; str_idx < str_len; ++str_idx) {
                        string[str_idx] = 'a' + (i + str_idx) % 5;
                    }

                    // create a Python string from it
                    handles[i] = HPyUnicode_FromString(ctx, string);
                    if (HPy_IsNull(handles[i])) {
                        result = HPy_NULL;
                        goto cleanup;
                    }

                    // get the data back and, while we are at it, check it too
                    HPy_ssize_t py_str_size;
                    const char* py_str_data = HPyUnicode_AsUTF8AndSize(ctx, handles[i], &py_str_size);
                    log("Checking data of the Python string...\\n");
                    if (py_str_size != str_len) {
                        log("Wrong size: %zd != %zd\\n", py_str_size, str_len);
                        HPyErr_SetString(ctx, ctx->h_SystemError, "The string size does not match");
                        result = HPy_NULL;
                        goto cleanup;
                    }
                    if (strcmp(string, py_str_data)) {
                        HPyErr_SetString(ctx, ctx->h_SystemError, "The string contents do not match");
                        result = HPy_NULL;
                        goto cleanup;
                    }

                    // cleanup and check that it didn't do any harm to the
                    // data returned by HPyUnicode_AsUTF8AndSize
                    string[0] = '\\0';
                    free(string);
                    if (strlen(py_str_data) != (size_t) py_str_size) {
                        HPyErr_SetString(ctx, ctx->h_SystemError,
                            "Freeing the user string corrupted the Python string data");
                        result = HPy_NULL;
                        goto cleanup;
                    }

                    // close some random-ish handle that we've opened in some previous iteration
                    if (i > 10) {
                        size_t to_close_idx = (str_len * 7) % (i - 1);
                        log("Closing handle at index %zu\\n", to_close_idx);
                        HPy_Close(ctx, handles[to_close_idx]);
                        handles[to_close_idx] = HPy_NULL;
                    }
                }

                result = HPy_Dup(ctx, ctx->h_None);
            cleanup:
                for (size_t i = 0; i < N; ++i) {
                    log("Cleanup: closing handle at index %zu\\n", i);
                    HPy_Close(ctx, handles[i]);
                }
                return result;
            }

            @EXPORT(f)
            @INIT
        """)
        assert mod.f() is None
