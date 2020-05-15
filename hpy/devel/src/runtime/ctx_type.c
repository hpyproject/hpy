#include <stddef.h>
#include <Python.h>
#include "hpy.h"
#include "common/runtime/ctx_type.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

/* by default, the C structs which bake an HPy custom type do NOT include
 * PyObject_HEAD.  So, HPy_New must allocate a memory region which is big
 * enough to contain PyObject_HEAD + any eventual extra padding + the actual
 * user struct. We use the union_alignment to ensure that the payload is
 * correctly aligned for every possible struct.
 */

typedef struct {
    PyObject_HEAD
    union {
        unsigned char payload[1];
        // these fields are never accessed: they are present just to ensure
        // the correct alignment of payload
        unsigned short _m_short;
        unsigned int _m_int;
        unsigned long _m_long;
        unsigned long long _m_longlong;
        float _m_float;
        double _m_double;
        long double _m_longdouble;
        void *_m_pointer;
    };
} GenericHPyObject;

#define PyObject_HEAD_SIZE (offsetof(GenericHPyObject, payload))


static void methoddef_copy_ml_meth_and_flags(HPyMethodDef *src, PyMethodDef *dst)
{
#ifdef HPY_UNIVERSAL_ABI
    // Universal ABI mode: ml_meth could be either a legacy function or a
    // new-style HPy method
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
#else
    // CPython ABI mode: ml_meth is already a PyCFunction function pointer
    dst->ml_meth = src->ml_meth;
    dst->ml_flags = src->ml_flags;
#endif
}

static void slot_copy_pfunc(HPyType_Slot *src, PyType_Slot *dst)
{
#ifdef HPY_UNIVERSAL_ABI
    // Universal ABI mode: for now we assume that it uses the HPy calling
    // convention, but we need a way to allow slots with the CPython
    // signature, similar to what we do in methoddef_copy_ml_meth_and_flags
    HPyMeth f = (HPyMeth)src->pfunc;
    void *impl_func;
    _HPy_CPyCFunction trampoline_func;
    f(&impl_func, &trampoline_func);
    dst->pfunc = trampoline_func;
#else
    // CPython ABI mode
    dst->pfunc = src->pfunc;
#endif
}


_HPy_HIDDEN void*
ctx_Cast(HPyContext ctx, HPy h)
{
    // XXX: how do we implement ctx_Cast when has_pyobject_head==1?
    GenericHPyObject *o = (GenericHPyObject*)(_h2py(h));
    return (void*)o->payload;
}

// note: this function is also called from ctx_module.c.
// This malloc a result which will never be freed. Too bad
_HPy_HIDDEN PyMethodDef *
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
        methoddef_copy_ml_meth_and_flags(src, dst);
    }
    result[count] = (PyMethodDef){NULL, NULL, 0, NULL};
    return result;
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
            slot_copy_pfunc(&hpyspec->slots[i], dst);
        }
    }
    result[count] = (PyType_Slot){0, NULL};
    return result;
}

_HPy_HIDDEN HPy
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
    // XXX: should we free() spec->slots?
    return _py2h(result);
}

_HPy_HIDDEN HPy
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

    *data = (void*)result->payload;
    return _py2h((PyObject*)result);
}
