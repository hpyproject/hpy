from .support import HPyTest
import pytest


class TestBuildValue(HPyTest):

    def make_build_item(self, fmt, values=None):
        comma_and_values = "" if values is None else ", " + values
        mod = self.make_module("""
            #include <limits.h>
        
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {{
                return HPy_BuildValue(ctx, "{fmt}" {values});
            }}
            @EXPORT(f)
            @INIT
        """.format(fmt=fmt, values=comma_and_values))
        return mod

    @pytest.mark.parametrize("fmt, c_values, expected", [
        ("i", "42", 42),
        ("i", "0", 0),
        ("i", "-1", -1),
        ("I", "33", 33),
        ("k", "1", 1),
        ("K", "6543", 6543),
        ("l", "345L", 345),
        ("l", "-876L", -876),
        ("L", "545LL", 545),
        ("L", "-344LL", -344),
        ("f", "0.25f", 0.25),
        ("d", "0.25", 0.25),
        ("ii", "-1, 1", (-1, 1)),
        ("(i)", "-1", (-1,)),
        ("(ii)", "-1, 1", (-1, 1)),
        ("s", '"test string"', "test string"),
        ("[ii]", "4, 2", [4, 2]),
        ("[is]", '4, "2"', [4, "2"]),
        ("[]", None, []),
        ("[(is)((f)[kk])i]", '4, "str", 0.25, 4, 2, 14267', [(4, "str"), ((0.25,), [4, 2]), 14267]),
    ])
    def test_formats(self, fmt, c_values, expected):
        mod = self.make_build_item(fmt, c_values)
        assert mod.f(None) == expected

    @pytest.mark.parametrize("fmt", ["O", "N", "S"])
    def test_O_and_aliases(self, fmt):
        class Dummy:
            pass
        obj = Dummy()
        mod = self.make_build_item(fmt, "arg")
        assert mod.f(obj) == obj

    def test_OO_pars_with_new_objects(self):
        mod = self.make_module("""
            #include <stdio.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {{
                HPy o1 = HPyLong_FromLong(ctx, 1);
                HPy o2 = HPyLong_FromLong(ctx, 2);
                HPy result = HPy_BuildValue(ctx, "(OO)", o1, o2);
                HPy_Close(ctx, o1);
                HPy_Close(ctx, o2);
                return result;
            }}
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(None) == (1, 2)

    def test_int_limits(self):
        mod = self.make_build_item("(ii)", "INT_MIN, INT_MAX")
        result = mod.f(None)
        assert result[0] < 0
        assert result[1] > 0

    def test_long_limit(self):
        mod = self.make_build_item("(ll)", "LONG_MIN, LONG_MAX")
        result = mod.f(None)
        assert result[0] < 0
        assert result[1] > 0

    def test_longlong_limit(self):
        mod = self.make_build_item("(LL)", "LLONG_MIN, LLONG_MAX")
        result = mod.f(None)
        assert result[0] < 0
        assert result[1] > 0

    def test_ulong_limit(self):
        mod = self.make_build_item("k", "ULONG_MAX")
        assert mod.f(None) > 0

    def test_ulonglong_limit(self):
        mod = self.make_build_item("k", "ULLONG_MAX")
        assert mod.f(None) > 0

    def test_uint_limit(self):
        mod = self.make_build_item("k", "UINT_MAX")
        assert mod.f(None) > 0

    @pytest.mark.parametrize("fmt, msg", [
        ("(q)", "bad format char 'q' in the format string passed HPy_BuildValue"),
        ("(i", "unmatched '(' in the format string passed HPy_BuildValue"),
        ("[i", "unmatched '[' in the format string passed HPy_BuildValue"),
        ("([(i)k", "unmatched '(' in the format string passed HPy_BuildValue"),
        ("(i]", "unmatched '(' in the format string passed HPy_BuildValue"),
        ("[i)", "unmatched '[' in the format string passed HPy_BuildValue"),
    ])
    def test_bad_format(self, fmt, msg):
        import pytest
        mod = self.make_build_item(fmt, "42")
        with pytest.raises(SystemError) as e:
            mod.f(None)
        assert msg in str(e)
