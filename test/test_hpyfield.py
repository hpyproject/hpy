"""
NOTE: these tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest, DefaultExtensionTemplate

class TestHPyField(HPyTest):

    def test_store_load(self):
        import sys
        mod = self.make_module("""
            typedef struct {
                HPyField a;
                HPyField b;
            } PairObject;
        
            HPyType_HELPERS(PairObject);

            HPyDef_SLOT(Pair_new, Pair_new_impl, HPy_tp_new)
            static HPy Pair_new_impl(HPyContext *ctx, HPy cls, HPy *args,
                                      HPy_ssize_t nargs, HPy kw)
            {
                HPy a;
                HPy b;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "OO", &a, &b))
                    return HPy_NULL;
                PairObject *pair;
                HPy h_obj = HPy_New(ctx, cls, &pair);
                pair->a = HPyField_Store(ctx, a);
                pair->b = HPyField_Store(ctx, b);
                return h_obj;
            }

            HPyDef_METH(Pair_get_a, "get_a", Pair_get_a_impl, HPyFunc_NOARGS)
            static HPy Pair_get_a_impl(HPyContext *ctx, HPy self)
            {
                PairObject *pair = PairObject_AsStruct(ctx, self);
                return HPyField_Load(ctx, pair->a);
            }

            HPyDef_METH(Pair_get_b, "get_b", Pair_get_b_impl, HPyFunc_NOARGS)
            static HPy Pair_get_b_impl(HPyContext *ctx, HPy self)
            {
                PairObject *pair = PairObject_AsStruct(ctx, self);
                return HPyField_Load(ctx, pair->b);
            }

            static HPyDef *Pair_defines[] = {
                &Pair_new,
                &Pair_get_a,
                &Pair_get_b
            };
            static HPyType_Spec Pair_spec = {
                .name = "mytest.Pair",
                .basicsize = sizeof(PairObject),
                .defines = Pair_defines
            };

            @EXPORT_TYPE("Pair", Pair_spec)
            @INIT
        """)
        p = mod.Pair("hello", "world")
        assert p.get_a() == 'hello'
        assert p.get_b() == 'world'
        #
        # check the refcnt
        if self.supports_refcounts():
            a = object()
            a_refcnt = sys.getrefcount(a)
            p2 = mod.Pair(a, None)
            assert sys.getrefcount(a) == a_refcnt + 1
            assert p2.get_a() is a
            assert sys.getrefcount(a) == a_refcnt + 1
