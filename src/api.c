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
    def->m_methods = NULL; // XXX
    PyObject *result = PyModule_Create(def);
    return _py2h(result);
}

static HPy
None_Get(HPyContext ctx)
{
    Py_INCREF(Py_None);
    return _py2h(Py_None);
}


struct _object *
CallRealFunctionFromTrampoline(HPyContext ctx, struct _object *self,
                               struct _object *args, HPyCFunction func)
{
    return _h2py(func(ctx, _py2h(self), _py2h(args)));
}


struct _HPyContext_s global_ctx = {
    .version = 1,
    .module_Create = &Module_Create,
    .none_Get = &None_Get,
    .callRealFunctionFromTrampoline = &CallRealFunctionFromTrampoline,
};
