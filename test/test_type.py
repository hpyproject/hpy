"""
NOTE: these tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
from .support import HPyTest


class TestType(HPyTest):
    def test_simple_type(self):
        mod = self.make_module("""
            typedef struct {
                HPyObject_HEAD
            } HPy_Dummy;

            static HPyType_Slot dummy_type_slots[] = {
                {0, NULL},
            };

            static HPyType_Spec dummy_type_spec = {
                .name = "mytest.Dummy",
                .basicsize = sizeof(HPy_Dummy),
                .itemsize = 0,
                .flags = HPy_TPFLAGS_DEFAULT | HPy_TPFLAGS_BASETYPE,
                .slots = dummy_type_slots,
            };

            static HPyMethodDef MyTestMethods[] = {
                {NULL, NULL, 0, NULL}
            };

            static HPyModuleDef moduledef = {
                HPyModuleDef_HEAD_INIT,
                .m_name = "mytest",
                .m_doc = "some test for hpy",
                .m_size = -1,
                .m_methods = MyTestMethods
            };


            HPy_MODINIT(mytest)
            static HPy init_mytest_impl(HPyContext ctx)
            {
                HPy m, h_dummy_type;
                m = HPyModule_Create(ctx, &moduledef);
                if (HPy_IsNull(m))
                    return HPy_NULL;
                h_dummy_type = HPyType_FromSpec(ctx, &dummy_type_spec);
                if (HPy_IsNull(h_dummy_type))
                return HPy_NULL;
                HPy_SetAttr_s(ctx, m, "Dummy", h_dummy_type);
                return m;
            }
        """)
        assert isinstance(mod.Dummy(), mod.Dummy)

        class Sub(mod.Dummy):
            pass
        assert isinstance(Sub(), mod.Dummy)
