"""
NOTE: these tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest, DefaultExtensionTemplate

class PairTemplate(DefaultExtensionTemplate):

    def DEFINE_PairObject(self):
        return """
            typedef struct {
                HPyField a;
                HPyField b;
            } PairObject;
        
            HPyType_HELPERS(PairObject);
        """

    def DEFINE_Pair_new(self):
        return """
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
        """

    def DEFINE_Pair_get_ab(self):
        return """
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
        """

    def EXPORT_PAIR_TYPE(self, *defines):
        defines += ('NULL',)
        defines = ', '.join(defines)
        self.EXPORT_TYPE('"Pair"', "Pair_spec")
        return """
            static HPyDef *Pair_defines[] = { %s };
            static HPyType_Spec Pair_spec = {
                .name = "mytest.Pair",
                .basicsize = sizeof(PairObject),
                .defines = Pair_defines
            };
        """ % defines


class TestHPyField(HPyTest):

    ExtensionTemplate = PairTemplate

    def test_store_load(self):
        import sys
        mod = self.make_module("""
            @DEFINE_PairObject
            @DEFINE_Pair_new
            @DEFINE_Pair_get_ab

            @EXPORT_PAIR_TYPE(&Pair_new, &Pair_get_a, &Pair_get_b)
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

    def test_clear(self):
        import sys
        mod = self.make_module("""
            #include <assert.h>
            @DEFINE_PairObject
            @DEFINE_Pair_new
            @DEFINE_Pair_get_ab

            HPyDef_METH(Pair_clear_a, "clear_a", Pair_clear_a_impl, HPyFunc_NOARGS)
            static HPy Pair_clear_a_impl(HPyContext *ctx, HPy self)
            {
                PairObject *pair = PairObject_AsStruct(ctx, self);
                assert(!HPy_IsNull(pair->a));
                HPyField_Clear(ctx, &pair->a);
                pair->a = HPyField_Store(ctx, ctx->h_None);
                return HPy_Dup(ctx, ctx->h_None);
            }

            @EXPORT_PAIR_TYPE(&Pair_new, &Pair_get_a, &Pair_get_b, &Pair_clear_a)
            @INIT
        """)
        p = mod.Pair("hello", "world")
        assert p.get_a() == 'hello'
        assert p.get_b() == 'world'
        p.clear_a()
        assert p.get_a() is None
        #
        # check the refcnt
        if self.supports_refcounts():
            a = object()
            a_refcnt = sys.getrefcount(a)
            p2 = mod.Pair(a, None)
            assert sys.getrefcount(a) == a_refcnt + 1
            assert p2.get_a() is a
            p2.clear_a()
            assert sys.getrefcount(a) == a_refcnt

    def test_tp_traverse(self):
        import gc
        mod = self.make_module("""
            #include <assert.h>
            @DEFINE_PairObject
            @DEFINE_Pair_new
            @DEFINE_Pair_get_ab

            HPyDef_SLOT(Pair_traverse, Pair_traverse_impl, HPy_tp_traverse)
            static int Pair_traverse_impl(HPyContext *ctx, HPy self,
                                          HPyFunc_visitproc visit, void *arg)
            {
                PairObject *p = PairObject_AsStruct(ctx, self);
                visit(ctx, &p->a, arg); // XXX use HPy_VISIT
                visit(ctx, &p->b, arg);
                return 0;
            }

            @EXPORT_PAIR_TYPE(&Pair_new, &Pair_get_a, &Pair_get_b, &Pair_traverse)
            @INIT
        """)
        p = mod.Pair("hello", "world")
        referents = gc.get_referents(p)
        referents.sort()
        assert referents == ['hello', 'world']
