"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest
from .test_hpylong import unsigned_long_bits


class TestParseItem(HPyTest):

    def make_build_item(self, fmt, values):
        mod = self.make_module("""
            #include <limits.h>
        
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {{
                return HPy_BuildValue(ctx, "{fmt}", {values});
            }}
            @EXPORT(f)
            @INIT
        """.format(fmt=fmt, values=values))
        return mod

    def test_f(self):
        mod = self.make_build_item("f", "HPyFloat_AsDouble(ctx, arg)")
        assert mod.f(0.25) == 0.25

    def test_i(self):
        mod = self.make_build_item("i", "HPyLong_AsLong(ctx, arg)")
        assert mod.f(42) == 42
        assert mod.f(0) == 0
        assert mod.f(-1) == -1

    def test_ii(self):
        mod = self.make_build_item("ii", "-1, 1")
        assert mod.f(None) == (-1, 1)

    def test_i_pars(self):
        mod = self.make_build_item("(i)", "-1")
        assert mod.f(None) == (-1,)

    def test_limits(self):
        mod = self.make_build_item("(ii)", "INT_MIN, INT_MAX")
        result = mod.f(None)
        assert result[0] < 0
        assert result[1] > 0
