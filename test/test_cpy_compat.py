from .support import HPyTest


class TestCPythonCompatibility(HPyTest):

    def test_frompyobject(self):
        mod = self.make_module("""
            #include <Python.h>
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
            {
                PyObject *o = PyList_New(0);

                Py_ssize_t initial_refcount = o->ob_refcnt;
                HPy h = HPy_FromPyObject(ctx, o);
                Py_ssize_t final_refcount = o->ob_refcnt;

                PyList_Append(o, PyLong_FromSsize_t(final_refcount -
                                                    initial_refcount));
                Py_DECREF(o);
                return h;
            }
            @EXPORT f METH_NOARGS
            @INIT
        """)
        x = mod.f()
        assert x == [+1]

    def test_hpy_close(self):
        mod = self.make_module("""
            #include <Python.h>
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
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
            @EXPORT f METH_NOARGS
            @INIT
        """)
        assert mod.f() == -1

    def test_hpy_dup(self):
        mod = self.make_module("""
            #include <Python.h>
            HPy_FUNCTION(f)
            static HPy f_impl(HPyContext ctx, HPy self, HPy args)
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
            @EXPORT f METH_NOARGS
            @INIT
        """)
        assert mod.f() == +1
