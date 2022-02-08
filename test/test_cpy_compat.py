from .support import HPyTest


class TestCPythonCompatibility(HPyTest):

    # One note about the supports_refcounts() in the tests below: on
    # CPython, handles are actually implemented as INCREF/DECREF, so we can
    # check e.g. after an HPy_Dup the refcnt is += 1. However, on PyPy they
    # are implemented in a completely different way which is unrelated to the
    # refcnt (this is the whole point of HPy, after all :)). So in many of the
    # following ttests, checking the actual result of the function doesn't
    # really make sens on PyPy. We still run the functions to ensure they do
    # not crash, though.

    def test_frompyobject(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                PyObject *o = PyList_New(0);
                Py_ssize_t initial_refcount = o->ob_refcnt;
                HPy h = HPy_FromPyObject(ctx, o);
                Py_ssize_t final_refcount = o->ob_refcnt;

                PyList_Append(o, PyLong_FromLong(1234));
                PyList_Append(o, PyLong_FromSsize_t(final_refcount -
                                                    initial_refcount));
                Py_DECREF(o);
                return h;
            }
            @EXPORT(f)
            @INIT
        """)
        x = mod.f()
        assert x[0] == 1234
        assert len(x) == 2
        if self.supports_refcounts():
            assert x == [1234, +1]

    def test_frompyobject_null(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                HPy h = HPy_FromPyObject(ctx, NULL);
                if (HPy_IsNull(h)) {
                    return HPy_Dup(ctx, ctx->h_True);
                }
                else {
                    return HPy_Dup(ctx, ctx->h_False);
                }
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f()

    def test_aspyobject(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                PyObject *o = HPy_AsPyObject(ctx, arg);
                long val = PyLong_AsLong(o);
                Py_DecRef(o);
                return HPyLong_FromLong(ctx, val*2);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f(21) == 42

    def test_aspyobject_null(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                PyObject *o = HPy_AsPyObject(ctx, HPy_NULL);
                if (o == NULL) {
                    return HPy_Dup(ctx, ctx->h_True);
                }
                else {
                    return HPy_Dup(ctx, ctx->h_False);
                }
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f()

    def test_aspyobject_custom_class(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                PyObject *o = HPy_AsPyObject(ctx, arg);
                PyObject *o_res = PyObject_CallMethod(o, "foo", "");
                HPy h_res = HPy_FromPyObject(ctx, o_res);
                Py_DecRef(o);
                Py_DecRef(o_res);
                return h_res;
            }
            @EXPORT(f)
            @INIT
        """)
        class MyClass:
            def foo(self):
                return 1234
        obj = MyClass()
        assert mod.f(obj) == 1234

    def test_aspyobject_hpy_type(self):
        mod = self.make_module("""
            typedef struct {
                int magic;
                HPyField f;
            } MyHPyType;

            #include <Python.h>
            typedef struct {
                PyObject_HEAD
                int magic;
                PyObject *f;
            } MyLegacyType;

            HPyDef_SLOT(MyType_new, MyType_new_impl, HPy_tp_new)
            static HPy MyType_new_impl(HPyContext *ctx, HPy cls, HPy *args,
                                      HPy_ssize_t nargs, HPy kw)
            {
                MyHPyType *t;
                HPy h_obj = HPy_New(ctx, cls, &t);
                HPyField_Store(ctx, h_obj, &t->f, args[0]);
                t->magic = 42;
                return h_obj;
            }

            HPyDef_SLOT(MyType_traverse, MyType_traverse_impl, HPy_tp_traverse)
            static int MyType_traverse_impl(void *self, HPyFunc_visitproc visit, void *arg)
            {
                MyHPyType *p = (MyHPyType *)self;
                HPy_VISIT(&p->f);
                return 0;
            }

            static HPyDef *MyType_defines[] = { &MyType_new, &MyType_traverse, NULL };
            static HPyType_Spec MyType_spec = {
                .name = "mytest.MyType",
                .basicsize = sizeof(MyHPyType),
                .flags = HPy_TPFLAGS_DEFAULT | HPy_TPFLAGS_HAVE_GC,
                .defines = MyType_defines
            };

            HPyDef_METH(f, "f", f_impl, HPyFunc_O)
            static HPy f_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                // Note: omits error checking to keep the code brief
                PyObject *o = HPy_AsPyObject(ctx, arg);
                MyLegacyType* legacy_struct = (MyLegacyType*) o;

                PyObject *foo_res = PyObject_CallMethod(legacy_struct->f, "foo", "");
                HPy h_foo_res = HPy_FromPyObject(ctx, foo_res);
                HPy h_magic = HPyLong_FromLong(ctx, legacy_struct->magic);

                HPy h_res = HPyTuple_FromArray(ctx, (HPy[]) {h_foo_res, h_magic}, 2);
                Py_DecRef(o);
                Py_DecRef(foo_res);
                HPy_Close(ctx, h_foo_res);
                HPy_Close(ctx, h_magic);
                return h_res;
            }

            HPyDef_METH(add_type, "add_type", add_type_impl, HPyFunc_NOARGS)
            static HPy add_type_impl(HPyContext *ctx, HPy self)
            {
                if (!HPyHelpers_AddType(ctx, self, "MyType", &MyType_spec, NULL)) {
                    return HPy_NULL;
                }
                return HPy_Dup(ctx, ctx->h_None);
            }

            @EXPORT(f)
            @EXPORT(add_type)
            @INIT
        """)

        class MyClass:
            def foo(self):
                return 1234
        field_obj = MyClass()

        mod.add_type()
        obj = mod.MyType(field_obj)

        assert mod.f(obj) == (1234, 42)

    def test_hpy_close(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                PyObject *o = PyList_New(0);

                HPy h = HPy_FromPyObject(ctx, o);
                Py_ssize_t initial_refcount = o->ob_refcnt;
                HPy_Close(ctx, h);
                Py_ssize_t final_refcount = o->ob_refcnt;

                Py_DECREF(o);
                return HPyLong_FromLong(ctx, (long)(final_refcount -
                                                    initial_refcount));
            }
            @EXPORT(f)
            @INIT
        """)
        x = mod.f()
        if self.supports_refcounts():
            assert x == -1

    def test_hpy_dup(self):
        mod = self.make_module("""
            #include <Python.h>
            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                PyObject *o = PyList_New(0);

                HPy h = HPy_FromPyObject(ctx, o);
                Py_ssize_t initial_refcount = o->ob_refcnt;
                HPy h2 = HPy_Dup(ctx, h);
                Py_ssize_t final_refcount = o->ob_refcnt;

                HPy_Close(ctx, h);
                HPy_Close(ctx, h2);
                Py_DECREF(o);
                return HPyLong_FromLong(ctx, (long)(final_refcount -
                                                    initial_refcount));
            }
            @EXPORT(f)
            @INIT
        """)
        x = mod.f()
        if self.supports_refcounts():
            assert x == +1

    def test_many_handles(self):
        mod = self.make_module("""
            #include <Python.h>
            #define NUM_HANDLES  10000

            HPyDef_METH(f, "f", f_impl, HPyFunc_NOARGS)
            static HPy f_impl(HPyContext *ctx, HPy self)
            {
                PyObject *o = PyList_New(0);

                Py_ssize_t result = -42;
                HPy handles[NUM_HANDLES];
                int i;
                Py_ssize_t initial_refcount = o->ob_refcnt;
                for (i = 0; i < NUM_HANDLES; i++)
                    handles[i] = HPy_FromPyObject(ctx, o);
                for (i = 0; i < NUM_HANDLES; i++)
                    if (HPy_IsNull(handles[i]))
                        goto error;
                for (i = 0; i < NUM_HANDLES; i++)
                    HPy_Close(ctx, handles[i]);
                Py_ssize_t final_refcount = o->ob_refcnt;
                result = final_refcount - initial_refcount;

             error:
                return HPyLong_FromLong(ctx, (long)result);
            }
            @EXPORT(f)
            @INIT
        """)
        assert mod.f() == 0
