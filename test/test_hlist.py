"""
NOTE: this tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestHList(HPyTest):
    def hlist_module(self, ops):
        return self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {{
                HList hl;
                hl = HList_New(ctx, 3);
                if (HList_IsNull(hl)) {{
                    return HPy_NULL;
                }}
                {ops}
                HList_Free(ctx, hl);
                return HPy_Dup(ctx, ctx->h_None);
            }}
            @EXPORT(f)
            @INIT
        """.format(ops=ops))

    def test_new_and_free(self):
        mod = self.hlist_module(ops="")
        mod.f(5)

    def test_track_without_closing(self):
        mod = self.hlist_module(ops="""
            HList_Track(ctx, hl, args[0]);
        """)
        mod.f(5)

    def test_track_and_closeall(self):
        mod = self.hlist_module(ops="""
            HList_Track(ctx, hl, args[0]);
            HList_CloseAll(ctx, hl);
        """)
        mod.f(5)

    def test_closeall_on_nothing(self):
        mod = self.hlist_module(ops="""
            HList_CloseAll(ctx, hl);
        """)
        mod.f(5)

    def test_squares_example(self):
        import pytest
        mod = self.make_module("""
            HPyDef_METH(squares, "squares", squares_impl, HPyFunc_VARARGS)
            static HPy squares_impl(HPyContext ctx, HPy self,
                              HPy *args, HPy_ssize_t nargs)
            {
                long i, n;
                long n_err = -1; // simulate an error at the given index
                int result;
                HPy key, value;
                HList hl;

                if (!HPyArg_Parse(ctx, args, nargs, "l|l", &n, &n_err))
                    return HPy_NULL;

                hl = HList_New(ctx, 2 * n);  // track n key-value pairs
                if (HList_IsNull(hl))
                    return HPy_NULL;

                HPy dict = HPyDict_New(ctx);
                if (HPy_IsNull(dict))
                    goto error;

                for (i=1; i<=n; i++) {
                    if (i==n_err)
                        goto error;
                    key = HPyLong_FromLong(ctx, i);
                    if (HPy_IsNull(key))
                        goto error;
                    HList_Track(ctx, hl, key);
                    value = HPyLong_FromLong(ctx, i * i);
                    if (HPy_IsNull(value))
                        goto error;
                    HList_Track(ctx, hl, value);
                    result = HPy_SetItem(ctx, dict, key, value);
                    if (result < 0)
                        goto error;
                }

                HList_CloseAll(ctx, hl);
                HList_Free(ctx, hl);
                return dict;

                error:
                    HList_CloseAll(ctx, hl);
                    HList_Free(ctx, hl);
                    HPy_Close(ctx, dict);
                    HPyErr_SetString(ctx, ctx->h_ValueError, "Failed!");
                    return HPy_NULL;
            }
            @EXPORT(squares)
            @INIT
        """)
        assert mod.squares(5) == {1: 1, 2: 4, 3: 9, 4: 16, 5: 25}
        assert mod.squares(100) == dict((i, i**2) for i in range(1, 101))
        with pytest.raises(ValueError) as err:
            mod.squares(5, 3)
        assert str(err.value) == "Failed!"
