from .support import HPyTest

class TestModule(HPyTest):
    def test_HPyModule_Create(self):
        mod = self.make_module("""
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                HPyModuleDef def = {
                    .name = "foo",
                    .doc = "Some doc",
                    .size = -1,
                };
                return HPyModule_Create(ctx, &def);
            }
            @EXPORT(f)
            @INIT
        """)
        m = mod.f()
        assert m.__name__ == "foo"
        assert m.__doc__ == "Some doc"
        assert m.__package__ is None
        assert m.__loader__ is None
        assert m.__spec__ is None
        assert set(vars(m).keys()) == {
            '__name__', '__doc__', '__package__', '__loader__', '__spec__'}

    def test_HPyModule_GetState(self):
        mod = self.make_module("""
            #include<string.h>

            typedef struct {
                HPyField field;
                char *rawData;
            } foo_data_t;

            HPyDef_SLOT(foo_traverse, foo_traverse_impl, HPy_m_traverse)
            static int foo_traverse_impl(void *self, HPyFunc_visitproc visit, void *arg)
            {
                foo_data_t *data = (foo_data_t *)self;
                HPy_VISIT(&data->field);
                return 0;
            }

            HPyDef_SLOT(foo_destroy, foo_destroy_impl, HPy_m_destroy)
            static void foo_destroy_impl(void *self)
            {
                foo_data_t *data = (foo_data_t *)self;
                free(data->rawData);
            }

            HPyDef_METH(bar, "bar", bar_impl, HPyFunc_NOARGS)
            static HPy bar_impl(HPyContext *ctx, HPy self)
            {
                foo_data_t *data = (foo_data_t *)HPyModule_GetState(ctx, self);
                return HPyUnicode_FromString(ctx, data->rawData);
            }

            HPyDef_METH(getf, "getf", getf_impl, HPyFunc_NOARGS)
            static HPy getf_impl(HPyContext *ctx, HPy self)
            {
                foo_data_t *data = (foo_data_t *)HPyModule_GetState(ctx, self);
                return HPyField_Load(ctx, self, data->field);
            }

            static HPyDef *foo_defines[] = {
                &bar,
                &getf,
                NULL,
                NULL,
                NULL
            };

            HPyDef_METH(f, "f", f_impl, HPyFunc_VARARGS)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
            {
                HPy field_value = args[0];
                bool implement_traverse = HPyLong_AsLong(ctx, args[1]);
                bool implement_destroy = HPyLong_AsLong(ctx, args[2]);

                // Dynamically add traverse and/or destroy to foo_defines
                foo_defines[2] = NULL;
                foo_defines[3] = NULL;
                size_t next_defines_idx = 2;
                if (implement_traverse)
                    foo_defines[next_defines_idx++] = &foo_traverse;
                if (implement_destroy)
                    foo_defines[next_defines_idx++] = &foo_destroy;

                HPyModuleDef def = {
                    .name = "foo",
                    .size = sizeof(foo_data_t),
                    .defines = foo_defines,
                };
                HPy module = HPyModule_Create(ctx, &def);
                foo_data_t *data = (foo_data_t *)HPyModule_GetState(ctx, module);

                // Example of dynamically allocated native memory
                if (implement_destroy) {
                    data->rawData = malloc(4);
                    memcpy(data->rawData, "ABQ", 4);
                }

                // Example of HPyField
                if (implement_traverse) {
                    data->field = HPyField_NULL;
                    HPyField_Store(ctx, self, &data->field, field_value);
                }

                return module;
            }

            @EXPORT(f)
            @INIT
        """)

        def test_with(traverse, destroy):
            obj = {'some': 'object'}
            m = mod.f(obj, traverse, destroy)
            if destroy:
                assert m.bar() == "ABQ"
            if traverse:
                assert m.getf() is obj

            # Check that tp_traverse visits the object
            if traverse and self.supports_gc_module():
                import gc
                referents = gc.get_referents(m)
                assert obj in referents

            del m
            # Note: it seems that CPython cannot really remove the module.
            # According to gc.get_referrers is referenced by the builtin functions
            # located in it, and it seems that this cycle is not detected?
            # if self.supports_refcounts():
            #    from sys import getrefcount
            #    obj_refcnt = getrefcount(obj)
            #    del m
            #    gc.collect()
            #    assert getrefcount(obj) == obj_refcnt - 1

        test_with(traverse=True, destroy=True)
        test_with(traverse=False, destroy=True)
        test_with(traverse=True, destroy=False)
        test_with(traverse=False, destroy=False)