"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestObject(HPyTest):
    def test_getitem(self):
        import pytest
        mod = self.make_module("""
            HPy_DEF_METH_O(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy key, result;
                key = HPyLong_FromLong(ctx, 0);
                if (HPy_IsNull(key))
                    return HPy_NULL;
                result = HPy_GetItem(ctx, arg, key);
                HPy_Close(ctx, key);
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT f HPy_METH_O
            @INIT
        """)
        assert mod.f({0: "hello"}) == "hello"
        with pytest.raises(KeyError) as exc:
            mod.f({1: "bad"})
        assert exc.value.args == (0,)

        assert mod.f(["hello"]) == "hello"
        with pytest.raises(IndexError):
            mod.f([])

    def test_getitem_i(self):
        import pytest
        mod = self.make_module("""
            HPy_DEF_METH_O(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
            {
                HPy result;
                result = HPy_GetItem_i(ctx, arg, 0);
                if (HPy_IsNull(result))
                    return HPy_NULL;
                return result;
            }
            @EXPORT f HPy_METH_O
            @INIT
        """)
        assert mod.f({0: "hello"}) == "hello"
        with pytest.raises(KeyError) as exc:
            mod.f({1: "bad"})
        assert exc.value.args == (0,)

        assert mod.f(["hello"]) == "hello"
        with pytest.raises(IndexError):
            mod.f([])
