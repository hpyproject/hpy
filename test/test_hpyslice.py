from .support import HPyTest

class TestSlice(HPyTest):

    def test_unpack(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(sizeof_ssize_t, "sizeof_ssize_t", HPyFunc_NOARGS)
            static HPy sizeof_ssize_t_impl(HPyContext *ctx, HPy self)
            {
                return HPyLong_FromLong(ctx, sizeof(HPy_ssize_t));
            }

            HPyDef_METH(f, "f", HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy_ssize_t start, stop, step;
                HPy h_start, h_stop, h_step;
                if (HPySlice_Unpack(ctx, arg, &start, &stop, &step))
                    return HPy_NULL;

                h_start = HPyLong_FromSsize_t(ctx, start);
                h_stop = HPyLong_FromSsize_t(ctx, stop);
                h_step = HPyLong_FromSsize_t(ctx, step);
                HPy result = HPyTuple_Pack(ctx, 3, h_start, h_stop, h_step);
                HPy_Close(ctx, h_start);
                HPy_Close(ctx, h_stop);
                HPy_Close(ctx, h_step);
                return result;
            }
            @EXPORT(f)
            @EXPORT(sizeof_ssize_t)
            @INIT
        """)

        ssize_t_max = (1 << (mod.sizeof_ssize_t() * 8 - 1)) - 1
        ssize_t_min = -(1 << (mod.sizeof_ssize_t() * 8 - 1))
        assert mod.f(slice(2, 20, 2)) == (2, 20, 2)
        assert mod.f(slice(4, 20)) == (4, 20, 1)
        assert mod.f(slice(5, (1 << 129))) == (5, ssize_t_max, 1)
        assert mod.f(slice(13, -27, -1)) == (13, -27, -1)
        assert mod.f(slice(ssize_t_min - 1000, ssize_t_max + 1000, 1)) == (ssize_t_min, ssize_t_max, 1)
        assert mod.f(slice(ssize_t_max, 0, -ssize_t_max-1000)) == (ssize_t_max, 0, -ssize_t_max)
        assert mod.f(slice(None, 10, 1)) == (0, 10, 1)
        assert mod.f(slice(None, 10, -1)) == (ssize_t_max, 10, -1)
        assert mod.f(slice(None, None, -1)) == (ssize_t_max, ssize_t_min, -1)
        assert mod.f(slice(None, None, None)) == (0, ssize_t_max, 1)
        assert mod.f(slice(13, None, None)) == (13, ssize_t_max, 1)
        assert mod.f(slice(13, None, -1)) == (13, ssize_t_min, -1)
        with pytest.raises(ValueError):
            mod.f(slice(1, 10, 0))
        with pytest.raises(TypeError):
            mod.f(slice(1, 10, "1"))
        with pytest.raises(TypeError):
            mod.f(slice(1, "10", 1))
        with pytest.raises(TypeError):
            mod.f(slice("0", 10, 1))
