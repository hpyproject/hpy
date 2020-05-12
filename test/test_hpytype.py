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

            static HPyType_Slot Dummy_slots[] = {
                {0, NULL},
            };

            static HPyType_Spec Dummy_spec = {
                .name = "mytest.Dummy",
                .basicsize = sizeof(DummyObject),
                .itemsize = 0,
                .flags = HPy_TPFLAGS_DEFAULT | HPy_TPFLAGS_BASETYPE,
                .slots = Dummy_slots,
            };

            @EXPORT_TYPE("Dummy", Dummy_spec)
            @INIT
        """)
        assert isinstance(mod.Dummy, type)
        assert mod.Dummy.__name__ == 'Dummy'
        assert mod.Dummy.__module__ == 'mytest'
        assert isinstance(mod.Dummy(), mod.Dummy)

        class Sub(mod.Dummy):
            pass
        assert isinstance(Sub(), mod.Dummy)

    def test_slots(self):
        mod = self.make_module("""
            typedef struct {
                HPyObject_HEAD
            } DummyObject;

            HPy_DEF_METH_NOARGS(Dummy_repr)
            static HPy Dummy_repr_impl(HPyContext ctx, HPy self)
            {
                return HPyUnicode_FromString(ctx, "<Dummy>");
            }

            HPy_DEF_METH_NOARGS(Dummy_abs)
            static HPy Dummy_abs_impl(HPyContext ctx, HPy self)
            {
                return HPyLong_FromLong(ctx, 1234);
            }

            static HPyType_Slot Dummy_slots[] = {
                {Py_tp_repr, Dummy_repr},
                {Py_nb_absolute, Dummy_abs},
                {0, NULL},
            };

            static HPyType_Spec Dummy_spec = {
                .name = "mytest.Dummy",
                .basicsize = sizeof(DummyObject),
                .slots = Dummy_slots
            };

            @EXPORT_TYPE("Dummy", Dummy_spec)
            @INIT
        """)
        d = mod.Dummy()
        assert repr(d) == '<Dummy>'
        assert abs(d) == 1234

    def test_tp_methods(self):
        mod = self.make_module("""
            typedef struct {
                HPyObject_HEAD
            } DummyObject;

            HPy_DEF_METH_O(Dummy_foo)
            static HPy Dummy_foo_impl(HPyContext ctx, HPy self, HPy arg)
            {
                return HPy_Add(ctx, arg, arg);
            }

            HPy_DEF_METH_NOARGS(Dummy_bar)
            static HPy Dummy_bar_impl(HPyContext ctx, HPy self)
            {
                return HPyLong_FromLong(ctx, 1234);
            }

            static HPyMethodDef Dummy_methods[] = {
                {"foo", Dummy_foo, HPy_METH_O},
                {"bar", Dummy_bar, HPy_METH_NOARGS},
                {NULL, NULL, NULL}
            };

            static HPyType_Slot dummy_type_slots[] = {
                {Py_tp_methods, Dummy_methods},
                {0, NULL},
            };

            static HPyType_Spec dummy_type_spec = {
                .name = "mytest.Dummy",
                .basicsize = sizeof(DummyObject),
                .slots = dummy_type_slots,
            };

            @EXPORT_TYPE("Dummy", dummy_type_spec)
            @INIT
        """)
        d = mod.Dummy()
        assert d.foo(21) == 42
        assert d.bar() == 1234
