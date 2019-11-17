import pytest
import sys
import types
from .support import HPyTest


class TestBasic(HPyTest):

    def test_empty_module(self):
        mod = self.make_module("""
            @INIT
        """)
        assert type(mod) is types.ModuleType
        assert mod.__loader__.name == 'mytest'
        assert mod.__spec__.loader is mod.__loader__
        assert mod.__file__

    def test_different_name(self):
        mod = self.make_module("""
            @INIT
        """, name="foo")
        assert mod.__name__ == "foo"

    def test_noop_function(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPyNone_Get(ctx);
            }
            @EXPORT f METH_NOARGS
            @INIT
        """)
        assert mod.f() is None

    def test_self_is_module(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPy_Dup(ctx, self);
            }
            @EXPORT f METH_NOARGS
            @INIT
        """)
        assert mod.f() is mod

    def test_identity_function(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                return HPy_Dup(ctx, arg);
            }
            @EXPORT f METH_O
            @INIT
        """)
        x = object()
        assert mod.f(x) is x

    def test_wrong_number_of_arguments(self):
        mod = self.make_module("""
            HPy_FUNCTION(f_noargs)
            static HPy f_noargs_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPyNone_Get(ctx);
            }
            HPy_FUNCTION(f_o)
            static HPy f_o_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPyNone_Get(ctx);
            }
            @EXPORT f_noargs METH_NOARGS
            @EXPORT f_o METH_O
            @INIT
        """)
        with pytest.raises(TypeError):
            mod.f_noargs(1)
        with pytest.raises(TypeError):
            mod.f_o()
        with pytest.raises(TypeError):
            mod.f_o(1, 2)

    def test_int_add(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
            {
                long a, b;
                if (!HPyArg_ParseTuple(ctx, args, "ll", &a, &b))
                    return HPy_NULL;
                return HPyLong_FromLong(ctx, a + b);
            }
            @EXPORT f METH_VARARGS
            @INIT
        """)
        assert mod.f(30, 12) == 42

    def test_close(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy one = HPyLong_FromLong(ctx, 1);
                if (HPy_IsNull(one))
                    return HPy_NULL;
                HPy res = HPyNumber_Add(ctx, arg, one);
                HPy_Close(ctx, one);
                return res;
            }
            @EXPORT f METH_O
            @INIT
        """)
        assert mod.f(41.5) == 42.5

    def test_string(self):
        mod = self.make_module("""
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
            {
                return HPyUnicode_FromString(ctx, "foobar");
            }
            @EXPORT f METH_NOARGS
            @INIT
        """)
        assert mod.f() == "foobar"
