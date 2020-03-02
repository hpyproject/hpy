"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestArgParse(HPyTest):
    def make_two_arg_add(self, fmt="OO"):
        mod = self.make_module("""
            HPy_DEF_METH_VARARGS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {{
                HPy a, b;
                if (!HPyArg_Parse(ctx, args, nargs, "{fmt}", &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }}
            @EXPORT f HPy_METH_VARARGS
            @INIT
        """.format(fmt=fmt))
        return mod

    def test_many_int_arguments(self):
        mod = self.make_module("""
            HPy_DEF_METH_VARARGS(f)
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
            @EXPORT f HPy_METH_VARARGS
            @INIT
        """)
        assert mod.f(4, 5, 6, 7, 8) == 45678

    def test_many_handle_arguments(self):
        mod = self.make_module("""
            HPy_DEF_METH_VARARGS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                HPy a, b;
                if (!HPyArg_Parse(ctx, args, nargs, "OO", &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }
            @EXPORT f HPy_METH_VARARGS
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
        assert str(exc.value) == "XXX: Too few arguments passed"

    def test_too_many_args(self):
        import pytest
        mod = self.make_two_arg_add()
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2, 3)
        assert str(exc.value) == "XXX: Too many arguments passed"


class TestArgParseKeywords(HPyTest):
    def make_two_arg_add(self, fmt="OO"):
        mod = self.make_module("""
            HPy_DEF_METH_KEYWORDS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {{
                HPy a, b;
                static const char *kwlist[] = {{ "a", "b", 0 }};
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "{fmt}", kwlist, &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }}
            @EXPORT f HPy_METH_VARARGS
            @INIT
        """.format(fmt=fmt))
        return mod

    def test_handle_two_arguments(self):
        mod = self.make_module("""
            HPy_DEF_METH_KEYWORDS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a, b;
                static const char *kwlist[] = {"a", "b", 0};
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "OO", kwlist, &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }
            @EXPORT f HPy_METH_KEYWORDS
            @INIT
        """)
        assert mod.f("x", b="y") == "xy"

    def test_handle_reordered_arguments(self):
        mod = self.make_module("""
            HPy_DEF_METH_KEYWORDS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a, b;
                static const char *kwlist[] = {"a", "b", 0};
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "OO", kwlist, &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }
            @EXPORT f HPy_METH_KEYWORDS
            @INIT
        """)
        assert mod.f(b="y", a="x") == "xy"

    def test_handle_optional_arguments(self):
        mod = self.make_module("""
            HPy_DEF_METH_KEYWORDS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs, HPy kw)
            {
                HPy a, b;
                b = HPyLong_FromLong(ctx, 5);
                static const char *kwlist[] = {"a", "b", 0};
                if (!HPyArg_ParseKeywords(ctx, args, nargs, kw, "O|O", kwlist, &a, &b))
                    return HPy_NULL;
                return HPyNumber_Add(ctx, a, b);
            }
            @EXPORT f HPy_METH_KEYWORDS
            @INIT
        """)
        assert mod.f(a=3, b=2) == 5
        assert mod.f(a=3) == 8

    def test_unsupported_fmt(self):
        import pytest
        mod = self.make_two_arg_add(fmt="ZZ")
        with pytest.raises(ValueError) as exc:
            mod.f("a")
        assert str(exc.value) == "XXX: Unknown arg format code"

    def test_missing_required_argument(self):
        import pytest
        mod = self.make_two_arg_add(fmt="OO")
        with pytest.raises(TypeError) as exc:
            mod.f(1)
        assert str(exc.value) == "XXX:"

    def test_mismatched_args_too_few_keywords(self):
        import pytest
        mod = self.make_two_arg_add(fmt="OOO")
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == "XXX: mismatched args (too few keywords for fmt)"

    def test_mismatched_args_too_many_keywords(self):
        import pytest
        mod = self.make_two_arg_add(fmt="O")
        with pytest.raises(TypeError) as exc:
            mod.f(1, 2)
        assert str(exc.value) == "XXX: mismatched args (too many keywords for fmt)"
