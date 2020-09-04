"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestParseItem(HPyTest):
    def make_parse_item(self, fmt, type, hpy_converter):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {{
                {type} a;
                if (!HPyArg_Parse(ctx, args, nargs, "{fmt}", &a))
                    return HPy_NULL;
                return {hpy_converter}(ctx, a);
            }}
            @EXPORT(f)
            @INIT
        """.format(fmt=fmt, type=type, hpy_converter=hpy_converter))
        return mod

    def test_i(self):
        mod = self.make_parse_item("i", "int", "HPyLong_FromLong")
        assert mod.f(1) == 1
        assert mod.f(-2) == -2

    def test_l(self):
        mod = self.make_parse_item("l", "long", "HPyLong_FromLong")
        assert mod.f(1) == 1
        assert mod.f(-2) == -2

    def test_d(self):
        import pytest
        mod = self.make_parse_item("d", "double", "HPyFloat_FromDouble")
        assert mod.f(1.) == 1.
        assert mod.f(-2) == -2.
        with pytest.raises(TypeError):
            mod.f("x")

    def test_O(self):
        mod = self.make_parse_item("O", "HPy", "HPy_Dup")
        assert mod.f("a") == "a"
        assert mod.f(5) == 5


class TestArgParse(HPyTest):
    def make_two_arg_add(self, fmt="OO"):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {{
                HPy a;
                HPy b = HPy_NULL;
                HPy res;
                if (!HPyArg_Parse(ctx, args, nargs, "{fmt}", &a, &b))
                    return HPy_NULL;
                if (HPy_IsNull(b)) {{
                    b = HPyLong_FromLong(ctx, 5);
                }} else {{
                    b = HPy_Dup(ctx, b);
                }}
                res = HPy_Add(ctx, a, b);
                HPy_Close(ctx, b);
                return res;
            }}
            @EXPORT(f)
            @INIT
        """.format(fmt=fmt))
        return mod

    def test_many_int_arguments(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                long a, b, c, d, e;
                if (!HPyArg_Parse(ctx, args, nargs, "lllll",
                                  &a, &b, &c, &d, &e))
                    return HPy_NULL;
                return HPyLong_FromLong(ctx,
                    10000*a + 1000*b + 100*c + 10*d + e);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(4, 5, 6, 7, 8) == 45678

    def test_many_handle_arguments(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                HPy a, b;
                if (!HPyArg_Parse(ctx, args, nargs, "OO", &a, &b))
                    return HPy_NULL;
                return HPy_Add(ctx, a, b);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f("a", "b") == "ab"

    def test_unsupported_fmt(self):
        import pytest
        mod = self.make_two_arg_add(fmt="ZZ")
        with pytest.raises(ValueError) as exc:
            mod.f("a")
        assert str(exc.value) == "XXX: Unknown arg format code"

    def test_too_few_args(self):
        import pytest
        mod = self.make_two_arg_add()
        with pytest.raises(TypeError) as exc:
            mod.f()
        assert str(exc.value) == "XXX: required positional argument missing"

    def test_too_many_args(self):
        import pytest
        mod = self.make_two_arg_add()
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2, 3)
        assert str(exc.value) == "XXX: mismatched args (too many arguments for fmt)"

    def test_optional_args(self):
        mod = self.make_two_arg_add(fmt="O|O")
        assert mod.f(1) == 6
        assert mod.f(3, 4) == 7

    def test_keyword_only_args_fails(self):
        import pytest
        mod = self.make_two_arg_add(fmt="O$O")
        with pytest.raises(ValueError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == "XXX: Unknown arg format code"


class TestArgParseKeywords(HPyTest):
    def make_two_arg_add(self, fmt="OO"):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {{
                HPy a, b;
                static const char *kwlist[] = {{ "a", "b", NULL }};
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "{fmt}",
                                          kwlist, &a, &b))
                    return HPy_NULL;
                HPy c = HPy_Add(ctx, a, b);
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return c;
            }}
            @EXPORT(f)
            @INIT
        """.format(fmt=fmt))
        return mod

    def test_handle_two_arguments(self):
        mod = self.make_two_arg_add("NN")
        assert mod.f("x", b="y") == "xy"

    def test_handle_reordered_arguments(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a, b;
                static const char *kwlist[] = { "a", "b", NULL };
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "NN", kwlist, &a, &b))
                    return HPy_NULL;
                HPy c = HPy_Add(ctx, a, b);
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return c;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(b="y", a="x") == "xy"

    def test_handle_optional_arguments(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a;
                HPy b = HPy_NULL;
                HPy res;
                static const char *kwlist[] = { "a", "b", NULL };
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "N|N", kwlist, &a, &b))
                    return HPy_NULL;
                if (HPy_IsNull(b)) {
                    b = HPyLong_FromLong(ctx, 5);
                }
                res = HPy_Add(ctx, a, b);
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return res;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(a=3, b=2) == 5
        assert mod.f(3, 2) == 5
        assert mod.f(a=3) == 8
        assert mod.f(3) == 8

    def test_unsupported_fmt(self):
        import pytest
        mod = self.make_two_arg_add(fmt="ZZ")
        with pytest.raises(ValueError) as exc:
            mod.f("a")
        assert str(exc.value) == "XXX: Unknown arg format code"

    def test_missing_required_argument(self):
        import pytest
        mod = self.make_two_arg_add(fmt="NN")
        with pytest.raises(TypeError) as exc:
            mod.f(1)
        assert str(exc.value) == "XXX: no value for required argument"

    def test_mismatched_args_too_few_keywords(self):
        import pytest
        mod = self.make_two_arg_add(fmt="NNN")
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == "XXX: mismatched args (too few keywords for fmt)"

    def test_mismatched_args_too_many_keywords(self):
        import pytest
        mod = self.make_two_arg_add(fmt="N")
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == "XXX: mismatched args (too many keywords for fmt)"

    def test_blank_keyword_argument_exception(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a, b, c;
                static const char *kwlist[] = { "", "b", "", NULL };
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "NNN", kwlist,
                                          &a, &b, &c))
                    return HPy_NULL;
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                HPy_Close(ctx, c);
                return HPy_Dup(ctx, ctx->h_None);
            }
            @EXPORT(f)
            @INIT
        """)
        with pytest.raises(TypeError) as exc:
            mod.f()
        assert str(exc.value) == "XXX: Empty keyword parameter name"

    def test_positional_only_argument(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a;
                HPy b = HPy_NULL;
                HPy res;
                static const char *kwlist[] = { "", "b", NULL };
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "N|N", kwlist, &a, &b))
                    return HPy_NULL;
                if (HPy_IsNull(b)) {
                    b = HPyLong_FromLong(ctx, 5);
                }
                res = HPy_Add(ctx, a, b);
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return res;
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(1, b=2) == 3
        assert mod.f(1, 2) == 3
        assert mod.f(1) == 6
        with pytest.raises(TypeError) as exc:
            mod.f(a=1, b=2)
        assert str(exc.value) == "XXX: no value for required argument"

    def test_keyword_only_argument(self):
        import pytest
        mod = self.make_two_arg_add(fmt="N$N")
        assert mod.f(1, b=2) == 3
        assert mod.f(a=1, b=2) == 3
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == (
            "XXX: keyword only argument passed as positional argument")

    def test_keyword_only_argument_convert_failure(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_KEYWORDS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a;
                HPy b = HPy_NULL;
                int c = 1000;
                static const char *kwlist[] = { "", "b", "c", NULL };
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "N$Ni", kwlist, &a, &b, &c))
                    return HPy_NULL;
                HPy_Close(ctx, a);
                HPy_Close(ctx, b);
                return HPyLong_FromLong(ctx, c);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(1, b=2, c=30) == 30
        with pytest.raises(TypeError) as exc:
            mod.f(1, b=2, c="foo")
