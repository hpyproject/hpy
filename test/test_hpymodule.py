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

    def test_HPyModule_ModuleInit_with_exec(self):
        mod = self.make_module("""
        static HPyModuleDef moduledef = {
            .name = "mytest",
            .doc = "some test for hpy"
        };

        HPy_MODINIT(mytest, moduledef, exec_foo)
        static int exec_foo(HPyContext *ctx, HPy mod)
        {
            HPy h_test = HPyUnicode_FromString(ctx, "value to check");
            HPy_SetAttr_s(ctx, mod, "test", h_test);
            HPy_Close(ctx, h_test);
            return 0;
        }
        """)
        assert hasattr(mod, "test")
        assert mod.test == "value to check"

    def test_HPyModule_ModuleInit_with_create_and_exec(self):
        mod = self.make_module("""
        static HPyDef *moduledefs[] = {
            NULL
        };

        static HPyModuleDef moduledef = {
            .name = "mytest",
            .doc = "some test for hpy",
            .defines = moduledefs,
        };

        HPy_MODINIT(mytest, moduledef, create_foo, exec_foo)

        static HPy create_foo(HPyContext *ctx, HPy spec, HPyModuleDef *def)
        {
            HPy mod = HPyModule_Create(ctx, def);
            HPy_SetAttr_s(ctx, mod, "create_spec", spec);
            return mod;
        }

        static int exec_foo(HPyContext *ctx, HPy mod)
        {
            if (HPy_HasAttr_s(ctx, mod, "create_spec")) {
                HPy spec = HPy_GetAttr_s(ctx, mod, "create_spec");
                HPy_SetAttr_s(ctx, mod, "exec_spec", spec);
                HPy_Close(ctx, spec);
                return 0;
            } else {
                return -1;
            }
        }
        """)
        assert hasattr(mod, "create_spec")
        assert mod.create_spec is getattr(mod, "exec_spec", None)
