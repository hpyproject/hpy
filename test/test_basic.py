import sys
import types
from .support import HPyTest


class TestBasic(HPyTest):

    def test_empty_module(self):
        mod = self.make_module("""
            @INIT
        """)
        assert type(mod) is types.ModuleType

    def test_noop_function(self):
        mod = self.make_module("""
            HPy_FUNCTION(test_noop)
            HPy test_noop_impl(HPyContext ctx, HPy self, HPy args)
            {
                HPy_RETURN_NONE;
            }
            @EXPORT test_noop METH_NOARGS
            @INIT
        """)
        assert mod.test_noop() is None

    def test_identity_function(self):
        mod = self.make_module("""
            HPy_FUNCTION(test_f)
            HPy test_f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                return HPy_Dup(ctx, arg);
            }
            @EXPORT test_f METH_O
            @INIT
        """)
        x = object()
        assert mod.test_f(x) is x

    def test_int_add(self):
        mod = self.make_module("""
            HPy_FUNCTION(test_f)
            HPy test_f_impl(HPyContext ctx, HPy self, HPy args)
            {
                long a, b;
                if (!HPyArg_ParseTuple(ctx, args, "ll", &a, &b))
                    return HPy_NULL;
                return HPyLong_FromLong(ctx, a + b);
            }
            @EXPORT test_f METH_VARARGS
            @INIT
        """)
        assert mod.test_f(30, 12) == 42

    def test_close(self):
        mod = self.make_module("""
            HPy_FUNCTION(test_f)
            HPy test_f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy one = HPyLong_FromLong(ctx, 1);
                if (HPy_IsNull(one))
                    return HPy_NULL;
                HPy res = HPyNumber_Add(ctx, arg, one);
                HPy_Close(ctx, one);
                return res;
            }
            @EXPORT test_f METH_O
            @INIT
        """)
        assert mod.test_f(41.5) == 42.5

    def test_string(self):
        mod = self.make_module("""
            HPy_FUNCTION(test_f)
            HPy test_f_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPyUnicode_FromString(ctx, "foobar");
            }
            @EXPORT test_f METH_NOARGS
            @INIT
        """)
        assert mod.test_f() == "foobar"
