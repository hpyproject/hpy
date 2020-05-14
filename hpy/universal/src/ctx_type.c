#include <Python.h>
#include "hpy.h"
#include "handles.h"
#include "ctx_type.h"
#include "ctx_module.h"


HPyAPI_STORAGE void*
ctx_Cast(HPyContext ctx, HPy h)
{
    return (void*)_h2py(h);
}

static PyType_Slot *
create_slot_defs(HPyType_Spec *hpyspec)
{
    // count the slots
    Py_ssize_t count;
    if (hpyspec->slots == NULL) {
        count = 0;
    }
    else {
        count = 0;
        while (hpyspec->slots[count].slot != 0)
            count++;
    }

    // allocate&fill the result
    PyType_Slot *result = PyMem_Malloc(sizeof(PyType_Slot) * (count+1));
    if (result == NULL)
        return NULL;
    for(int i=0; i<count; i++) {
        HPyType_Slot *src = &hpyspec->slots[i];
        PyType_Slot *dst = &result[i];
        dst->slot = src->slot;
        if (src->slot == Py_tp_methods) {
            // src->pfunc contains a HPyMethodDef*, which we convert to the
            // CPython's equivalent. The result of create_method_defs can't be
            // freed because CPython stores an internal reference to it, so
            // this creates a small leak :(
            HPyMethodDef *hpymethods = (HPyMethodDef*)src->pfunc;
            dst->pfunc = create_method_defs(hpymethods);
            if (dst->pfunc == NULL) {
                PyMem_Free(result);
                return NULL;
            }
        }
        else {
            // this must be a slot which contains a function pointer.  For now
            // we assume that it uses the HPy calling convention, but we need
            // a way to allow slots with the CPython signature, to make it
            // easier to do incremental porting.
            HPyMeth f = (HPyMeth)hpyspec->slots[i].pfunc;
            void *impl_func;
            _HPy_CPyCFunction trampoline_func;
            f(&impl_func, &trampoline_func);
            dst->pfunc = trampoline_func;
        }
    }
    result[count] = (PyType_Slot){0, NULL};
    return result;
}

HPyAPI_STORAGE HPy
ctx_Type_FromSpec(HPyContext ctx, HPyType_Spec *hpyspec)
{
    PyType_Spec *spec = PyMem_Malloc(sizeof(PyType_Spec));
    if (spec == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    spec->name = hpyspec->name;
    spec->basicsize = hpyspec->basicsize;
    spec->itemsize = hpyspec->itemsize;
    spec->flags = hpyspec->flags;
    spec->slots = create_slot_defs(hpyspec);
    if (spec->slots == NULL) {
        PyMem_Free(spec);
        PyErr_NoMemory();
        return HPy_NULL;
    }
    PyObject *result = PyType_FromSpec(spec);
    return _py2h(result);
}
