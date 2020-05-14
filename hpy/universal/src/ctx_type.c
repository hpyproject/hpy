#include <stddef.h>
#include <Python.h>
#include "hpy.h"
#include "handles.h"
#include "ctx_type.h"
#include "ctx_module.h"

/* by default, the C structs which bake an HPy custom type do NOT include
 * PyObject_HEAD.  So, HPy_New must allocate a memory region which is big
 * enough to contain PyObject_HEAD + any eventual extra padding + the actual
 * user struct. We use the union_alignment to ensure that the payload is
 * correctly aligned for every possible struct.
 */

typedef union {
    unsigned char m_char[1];
    unsigned short m_short;
    unsigned int m_int;
    unsigned long m_long;
    unsigned long long m_longlong;
    float m_float;
    double m_double;
    long double m_longdouble;
    void *m_pointer;
} union_alignment;

typedef struct {
    PyObject_HEAD
    union_alignment payload;
} GenericHPyObject;

#define PyObject_HEAD_SIZE (offsetof(GenericHPyObject, payload))


HPyAPI_STORAGE void*
ctx_Cast(HPyContext ctx, HPy h)
{
    // XXX: how do we implement ctx_Cast when has_pyobject_head==1?
    GenericHPyObject *o = (GenericHPyObject*)(_h2py(h));
    return (void*)o->payload.m_char;
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
    if (hpyspec->has_pyobject_head) {
        PyErr_SetString(PyExc_NotImplementedError, "has_pyobject_head not supported yet");
        return HPy_NULL;
    }
    spec->name = hpyspec->name;
    spec->basicsize = hpyspec->basicsize + PyObject_HEAD_SIZE;
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

HPyAPI_STORAGE HPy
ctx_New(HPyContext ctx, HPy h_type, void **data)
{
    PyObject *tp = _h2py(h_type);
    if (!PyType_Check(tp)) {
        PyErr_SetString(PyExc_TypeError, "HPy_New arg 1 must be a type");
        return HPy_NULL;
    }

    GenericHPyObject *result = PyObject_New(GenericHPyObject, (PyTypeObject*)tp);
    if (!result)
        return HPy_NULL;

    *data = (void*)result->payload.m_char;
    return _py2h((PyObject*)result);
}
