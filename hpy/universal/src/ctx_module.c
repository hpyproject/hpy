#include <Python.h>
#include "hpy.h"
#include "api.h"
#include "handles.h"

static PyModuleDef empty_moduledef = {
    PyModuleDef_HEAD_INIT
};

// this malloc a result which will never be freed. Too bad
HPyAPI_STORAGE PyMethodDef *
create_method_defs(HPyMethodDef *hpymethods)
{
    // count the methods
    Py_ssize_t count;
    if (hpymethods == NULL) {
        count = 0;
    }
    else {
        count = 0;
        while (hpymethods[count].ml_name != NULL)
            count++;
    }

    // allocate&fill the result
    PyMethodDef *result = PyMem_Malloc(sizeof(PyMethodDef) * (count+1));
    if (result == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    for(int i=0; i<count; i++) {
        HPyMethodDef *src = &hpymethods[i];
        PyMethodDef *dst = &result[i];
        dst->ml_name = src->ml_name;
        dst->ml_doc = src->ml_doc;

        if (src->ml_flags & _HPy_METH) {
            // HPy function: cal ml_meth to get pointers to the impl_func and
            // the cpy trampoline
            void *impl_func;
            PyCFunction trampoline_func;
            src->ml_meth(&impl_func, &trampoline_func);
            dst->ml_meth = (PyCFunction)trampoline_func;
            dst->ml_flags = src->ml_flags & ~_HPy_METH;
        }
        else {
            // legacy function: ml_meth already contains a function pointer
            // with the correct CPython signature
            dst->ml_meth = (PyCFunction)src->ml_meth;
            dst->ml_flags = src->ml_flags;
        }
    }
    result[count] = (PyMethodDef){NULL, NULL, 0, NULL};
    return result;
}

HPyAPI_STORAGE HPy
ctx_Module_Create(HPyContext ctx, HPyModuleDef *hpydef)
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
    def->m_methods = create_method_defs(hpydef->m_methods);
    if (def->m_methods == NULL)
        return HPy_NULL;
    PyObject *result = PyModule_Create(def);
    return _py2h(result);
}
