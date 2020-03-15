from .support import HPyTest


class TestNumber(HPyTest):

    def test_unary(self):
        import pytest
        import operator
        for c_name, op in [
                ('Negative', operator.neg),
                ('Positive', operator.pos),
                ('Absolute', abs),
                ('Invert', operator.invert),
                ('Index', operator.index),
                ('Long', int),
                ('Float', float),
                ]:
            mod = self.make_module("""
                HPy_DEF_METH_O(f)
                static HPy f_impl(HPyContext ctx, HPy self, HPy arg)
                {
                    return HPyNumber_%s(ctx, arg);
                }
                @EXPORT f HPy_METH_O
                @INIT
            """ % (c_name,), name='number_'+c_name)
            assert mod.f(-5) == op(-5)
            assert mod.f(6) == op(6)
            try:
                res = op(4.75)
            except Exception as e:
                with pytest.raises(e.__class__):
                    mod.f(4.75)
            else:
                assert mod.f(4.75) == res

    def test_binary(self):
        import operator
        for c_name, op in [
                ('Add', operator.add),
                ('Subtract', operator.sub),
                ('Multiply', operator.mul),
                ('FloorDivide', operator.floordiv),
                ('TrueDivide', operator.truediv),
                ('Remainder', operator.mod),
                ('Divmod', divmod),
                ('Lshift', operator.lshift),
                ('Rshift', operator.rshift),
                ('And', operator.and_),
                ('Xor', operator.xor),
                ('Or', operator.or_),
                ]:
            mod = self.make_module("""
                HPy_DEF_METH_VARARGS(f)
                static HPy f_impl(HPyContext ctx, HPy self,
                                  HPy *args, HPy_ssize_t nargs)
                {
                    HPy a, b;
                    if (!HPyArg_Parse(ctx, args, nargs, "OO", &a, &b))
                        return HPy_NULL;
                    return HPyNumber_%s(ctx, a, b);
                }
                @EXPORT f HPy_METH_VARARGS
                @INIT
            """ % (c_name,), name='number_'+c_name)
            assert mod.f(5, 4) == op(5, 4)
            assert mod.f(6, 3) == op(6, 3)

    def test_power(self):
        mod = self.make_module("""
            HPy_DEF_METH_VARARGS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                HPy a, b, c;
                if (!HPyArg_Parse(ctx, args, nargs, "OOO", &a, &b, &c))
                    return HPy_NULL;
                return HPyNumber_Power(ctx, a, b, c);
            }
            @EXPORT f HPy_METH_VARARGS
            @INIT
        """)
        assert mod.f(4, 5, None) == 4 ** 5
        assert mod.f(4, 5, 7) == pow(4, 5, 7)

    def test_matmul(self):
        class Mat:
            def __matmul__(self, other):
                return ('matmul', self, other)
        m1 = Mat()
        m2 = Mat()
        mod = self.make_module("""
            HPy_DEF_METH_VARARGS(f)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                HPy a, b;
                if (!HPyArg_Parse(ctx, args, nargs, "OO", &a, &b))
                    return HPy_NULL;
                return HPyNumber_MatrixMultiply(ctx, a, b);
            }
            @EXPORT f HPy_METH_VARARGS
            @INIT
        """)
        assert mod.f(m1, m2) == m1.__matmul__(m2)
