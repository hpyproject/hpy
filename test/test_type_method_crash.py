"""
Test for method calls on custom types - reproduces crash in Universal ABI.

This test creates a simple custom type with a NOARGS method and verifies
that the method can be called on an instance.
"""
from .support import HPyTest


class TestTypeMethodCrash(HPyTest):
    """
    Reproducer for crash when calling methods on custom types in Universal ABI.

    The crash occurs when:
    1. A custom type is created via HPyType_FromSpec
    2. An instance is created via HPy_New
    3. A method (defined via HPyDef_METH) is called on the instance

    The crash happens before the method body is entered, suggesting an issue
    in the method dispatch mechanism for Universal ABI.
    """

    def test_simple_noargs_method(self):
        """
        Minimal reproducer: type with HPyType_HELPERS and a NOARGS method.
        """
        mod = self.make_module("""
            typedef struct {
                long value;
            } MyObject;

            HPyType_HELPERS(MyObject)

            HPyDef_SLOT(My_new, HPy_tp_new)
            static HPy My_new_impl(HPyContext *ctx, HPy cls, const HPy *args,
                                   HPy_ssize_t nargs, HPy kw)
            {
                MyObject *obj;
                HPy h = HPy_New(ctx, cls, &obj);
                if (HPy_IsNull(h))
                    return HPy_NULL;
                obj->value = 42;
                return h;
            }

            HPyDef_METH(My_get_value, "get_value", HPyFunc_NOARGS)
            static HPy My_get_value_impl(HPyContext *ctx, HPy self)
            {
                MyObject *obj = MyObject_AsStruct(ctx, self);
                return HPyLong_FromLong(ctx, obj->value);
            }

            static HPyDef *My_defines[] = {
                &My_new,
                &My_get_value,
                NULL
            };

            static HPyType_Spec My_spec = {
                .name = "mytest.MyType",
                .basicsize = sizeof(MyObject),
                .defines = My_defines,
            };

            @EXPORT_TYPE("MyType", My_spec)
            @INIT
        """)
        obj = mod.MyType()
        # This call crashes in Universal ABI before the method body is entered
        result = obj.get_value()
        assert result == 42

    def test_simple_noargs_method_no_struct_access(self):
        """
        Even simpler: method that doesn't access struct at all.
        """
        mod = self.make_module("""
            typedef struct {
                long value;
            } MyObject;

            HPyType_HELPERS(MyObject)

            HPyDef_SLOT(My_new, HPy_tp_new)
            static HPy My_new_impl(HPyContext *ctx, HPy cls, const HPy *args,
                                   HPy_ssize_t nargs, HPy kw)
            {
                MyObject *obj;
                HPy h = HPy_New(ctx, cls, &obj);
                if (HPy_IsNull(h))
                    return HPy_NULL;
                obj->value = 42;
                return h;
            }

            HPyDef_METH(My_constant, "constant", HPyFunc_NOARGS)
            static HPy My_constant_impl(HPyContext *ctx, HPy self)
            {
                // Doesn't access struct at all - just returns a constant
                return HPyLong_FromLong(ctx, 123);
            }

            static HPyDef *My_defines[] = {
                &My_new,
                &My_constant,
                NULL
            };

            static HPyType_Spec My_spec = {
                .name = "mytest.MyType",
                .basicsize = sizeof(MyObject),
                .defines = My_defines,
            };

            @EXPORT_TYPE("MyType", My_spec)
            @INIT
        """)
        obj = mod.MyType()
        # This also crashes - proving the issue is not in AsStruct
        result = obj.constant()
        assert result == 123

    def test_type_without_basicsize(self):
        """
        Test with basicsize=0 - no custom struct, just methods.
        """
        mod = self.make_module("""
            HPyDef_METH(My_constant, "constant", HPyFunc_NOARGS)
            static HPy My_constant_impl(HPyContext *ctx, HPy self)
            {
                return HPyLong_FromLong(ctx, 456);
            }

            static HPyDef *My_defines[] = {
                &My_constant,
                NULL
            };

            static HPyType_Spec My_spec = {
                .name = "mytest.MyType",
                .defines = My_defines,
            };

            @EXPORT_TYPE("MyType", My_spec)
            @INIT
        """)
        obj = mod.MyType()
        result = obj.constant()
        assert result == 456
