import pytest
from test.support import SUPPORTS_SYS_EXECUTABLE

@pytest.fixture
def hpy_abi():
    return "debug"


@pytest.mark.skipif(not SUPPORTS_SYS_EXECUTABLE, reason="needs subprocess")
def test_use_invalid_as_struct(compiler, python_subprocess):
    mod = compiler.compile_module("""
        typedef struct {
            int value;
        } DummyObject;
        HPyType_HELPERS(DummyObject, HPyType_BuiltinShape_Object)
        
        static HPyType_Spec Dummy_spec = {
            .name = "mytest.Dummy",
            .basicsize = sizeof(DummyObject),
            .builtin_shape = HPyType_BuiltinShape_Type
        };
                

        HPyDef_METH(f, "f", HPyFunc_O)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
        {
            DummyObject *data = DummyObject_AsStruct(ctx, arg);
            return HPyLong_FromLong(ctx, data->value);
        }

        @EXPORT_TYPE("Dummy", Dummy_spec)
        @EXPORT(f)
        @INIT
    """)
    code = "assert mod.f(mod.Dummy()) == 0"
    result = python_subprocess.run(mod, code)
    assert result.returncode != 0
    assert "Invalid usage of _HPy_AsStruct_Object" in result.stderr.decode("utf-8")


@pytest.mark.skipif(not SUPPORTS_SYS_EXECUTABLE, reason="needs subprocess")
def test_typecheck(compiler, python_subprocess):
    mod = compiler.compile_module("""
        HPyDef_METH(f, "f", HPyFunc_VARARGS)
        static HPy f_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
        {
            if (nargs != 2) {
                HPyErr_SetString(ctx, ctx->h_TypeError, "expected exactly 2 arguments");
                return HPy_NULL;
            }
            int res = HPy_TypeCheck(ctx, args[0], args[1]);
            return HPyBool_FromLong(ctx, res);
        }
        @EXPORT(f)
        @INIT
    """)
    code = "assert mod.f(mod.f('hello', 2)) == 0"
    result = python_subprocess.run(mod, code)
    assert result.returncode != 0
    assert "HPy_TypeCheck arg 2 must be a type" in result.stderr.decode("utf-8")

