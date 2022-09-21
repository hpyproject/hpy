from test.support import SUPPORTS_SYS_EXECUTABLE
import os
import pytest

# Tests detection of usage of char pointers associated with invalid already
# closed handles. For now, the debug mode does not provide any hook for this
# error, so we have to run the tests from subprocess and check the return code.

SUPPORTS_MEM_PROTECTION = os.name == 'posix' and \
                          '_HPY_DEBUG_FORCE_DEFAULT_MEM_PROTECT' not in os.environ

@pytest.fixture
def hpy_abi():
    from hpy.debug import LeakDetector
    with LeakDetector():
        yield "debug"


def test_reuse_context_from_global_variable(compiler, python_subprocess):
    mod = compiler.compile_module("""
        #include <stdio.h>

        HPyContext *keep;

        HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
        static HPy f_impl(HPyContext *ctx, HPy self)
        {
            keep = ctx;
            return HPy_Dup(ctx, ctx->h_None);
        }

        HPyDef_METH(g, "g", g_impl, HPyFunc_NOARGS)
        static HPy g_impl(HPyContext *ctx, HPy self)
        {
            HPy t = HPy_Dup(ctx, ctx->h_True);
            // just checking if the correct context works
            if (!HPy_TypeCheck(ctx, t, ctx->h_BoolType)) { 
                // this should make the test fail
                HPy_Close(ctx, t);
                return HPy_Dup(ctx, ctx->h_None);
            }
            HPy_Close(ctx, t);
            printf("Heavy Marmelade\\n");
            return HPy_Dup(keep, ctx->h_None);
        }

        @EXPORT(f)
        @EXPORT(g)
        @INIT
    """)
    code = "mod.f(); mod.g()"
    result = python_subprocess.run(mod, code)
    assert result.returncode != 0
    print(result.stdout.decode("latin-1"))
    print(result.stderr.decode("latin-1"))
    assert result.stdout == b"Heavy Marmelade\n"
    assert result.stderr == b"Error: Wrong HPy Context!"
    