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
            } DummyObject;

            static HPyType_Slot dummy_type_slots[] = {
                {0, NULL},
            };

            static HPyType_Spec dummy_type_spec = {
                .name = "mytest.Dummy",
                .basicsize = sizeof(DummyObject),
                .itemsize = 0,
                .flags = HPy_TPFLAGS_DEFAULT | HPy_TPFLAGS_BASETYPE,
                .slots = dummy_type_slots,
            };

            @EXPORT_TYPE("Dummy", dummy_type_spec)
            @INIT
        """)
        assert isinstance(mod.Dummy, type)
        assert mod.Dummy.__name__ == 'Dummy'
        assert mod.Dummy.__module__ == 'mytest'
        assert isinstance(mod.Dummy(), mod.Dummy)

        class Sub(mod.Dummy):
            pass
        assert isinstance(Sub(), mod.Dummy)
