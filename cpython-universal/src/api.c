#include <Python.h>
#include <stdlib.h>
#include "universal/hpy.h"

static PyModuleDef empty_moduledef = {
    PyModuleDef_HEAD_INIT
};

// XXX: we should properly allocate it dynamically&growing
static PyObject *objects[100];
static Py_ssize_t last_handle = 0;
HPy
_py2h(PyObject *obj)
{
    Py_ssize_t i = last_handle++;
    objects[i] = obj;
    return (HPy){(void*)i};
}

PyObject *
_h2py(HPy h)
{
    Py_ssize_t i = (Py_ssize_t)h._o;
    return objects[i];
}

void
_hclose(HPy h)
{
    Py_ssize_t i = (Py_ssize_t)h._o;
    Py_XDECREF(objects[i]);
    objects[i] = NULL;
}

// this malloc a result which will never be freed. Too bad
static PyMethodDef *
create_method_defs(HPyModuleDef *hpydef)
{
    // count the methods
    Py_ssize_t count;
    if (hpydef->m_methods == NULL) {
        count = 0;
    }
    else {
        count = 0;
        while (hpydef->m_methods[count].ml_name != NULL)
            count++;
    }

    // allocate&fill the result
    PyMethodDef *result = PyMem_Malloc(sizeof(PyMethodDef) * (count+1));
    if (result == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    for(int i=0; i<count; i++) {
        HPyMethodDef *src = &hpydef->m_methods[i];
        PyMethodDef *dst = &result[i];
        dst->ml_name = src->ml_name;
        dst->ml_flags = src->ml_flags;
        dst->ml_doc = src->ml_doc;

        HPyCFunction impl_func;
        PyCFunction trampoline_func;
        src->ml_meth(&impl_func, &trampoline_func);
        dst->ml_meth = trampoline_func;
    }
    result[count] = (PyMethodDef){NULL, NULL, 0, NULL};
    return result;
}


static HPy
Module_Create(HPyContext ctx, HPyModuleDef *hpydef)
{
    // create a new PyModuleDef

    // we can't free this memory because it is stitched into moduleobject. We
    // just make it immortal for now, eventually we should think whether or
    // not to free it if/when we unload the module
    PyModuleDef *def = PyMem_Malloc(sizeof(PyModuleDef));
    if (def == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    memcpy(def, &empty_moduledef, sizeof(PyModuleDef));
    def->m_name = hpydef->m_name;
    def->m_doc = hpydef->m_doc;
    def->m_size = hpydef->m_size;
    def->m_methods = create_method_defs(hpydef);
    if (def->m_methods == NULL)
        return HPy_NULL;
    PyObject *result = PyModule_Create(def);
    return _py2h(result);
}

static HPy
None_Get(HPyContext ctx)
{
    Py_INCREF(Py_None);
    return _py2h(Py_None);
}


static struct _object *
CallRealFunctionFromTrampoline(HPyContext ctx, struct _object *self,
                               struct _object *args, HPyCFunction func)
{
    return _h2py(func(ctx, _py2h(self), _py2h(args)));
}


static HPy
FromPyObject(HPyContext ctx, struct _object *obj)
{
    Py_INCREF(obj);
    return _py2h(obj);
}

static struct _object *
AsPyObject(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return obj;
}

static void
Close(HPyContext ctx, HPy h)
{
    _hclose(h);
}

static HPy
Dup(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XINCREF(obj);
    return _py2h(obj);
}

static HPy
Long_FromLong(HPyContext ctx, long value)
{
    return _py2h(PyLong_FromLong(value));
}

static int
Arg_VaParse(HPyContext ctx, HPy args, const char *fmt, va_list vl)
{
    /* XXX EXPLODES IF THERE ARE SOME 'PyObject *' RETURNED */
    return PyArg_VaParse(_h2py(args), fmt, vl);
}


#include "autogen_ctx_def.h"
