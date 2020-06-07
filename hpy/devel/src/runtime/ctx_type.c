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


_HPy_HIDDEN void*
ctx_Cast(HPyContext ctx, HPy h)
{
    // XXX: how do we implement ctx_Cast when has_pyobject_head==1?
    GenericHPyObject *o = (GenericHPyObject*)(_h2py(h));
    return (void*)o->payload;
}

static int
sig2flags(HPyMeth_Signature sig)
{
    switch(sig) {
        case HPyMeth_VARARGS:  return METH_VARARGS;
        case HPyMeth_KEYWORDS: return METH_VARARGS | METH_KEYWORDS;
        case HPyMeth_NOARGS:   return METH_NOARGS;
        case HPyMeth_O:        return METH_O;
        default:               return -1;
    }
}

/*
 * Create a PyMethodDef which contains:
 *     1. All the hpyspec->methods whose .slot == HPy_meth
 *     2. All the PyMethodDef contained inside legacy_methods
 *
 * Notes:
 *     - This function is also called from ctx_module.c.
 *     - This malloc()s a result which will never be freed. Too bad
 */
_HPy_HIDDEN PyMethodDef *
create_method_defs(HPyMeth *hpymethods[], PyMethodDef *legacy_methods)
{
    // count the methods
    Py_ssize_t hpy_count = 0;
    Py_ssize_t legacy_count = 0;
    Py_ssize_t total_count = 0;
    if (hpymethods != NULL) {
        for(int i=0; hpymethods[i] != NULL; i++)
            if (hpymethods[i]->slot == HPy_meth)
                hpy_count++;
    }
    if (legacy_methods != NULL) {
        while (legacy_methods[legacy_count].ml_name != NULL)
            legacy_count++;
    }
    total_count = hpy_count + legacy_count;

    // allocate&fill the result
    PyMethodDef *result = PyMem_Malloc(sizeof(PyMethodDef) * (total_count+1));
    if (result == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    // copy the HPy methods
    int dst_idx = 0;
    for(int i=0; hpymethods[i] != NULL; i++) {
        HPyMeth *src = hpymethods[i];
        if (src->slot != HPy_meth)
            continue;
        PyMethodDef *dst = &result[dst_idx++];
        dst->ml_name = src->name;
        dst->ml_meth = src->cpy_trampoline;
        dst->ml_flags = sig2flags(src->signature);
        if (dst->ml_flags == -1) {
            PyMem_Free(result);
            PyErr_SetString(PyExc_ValueError, "Unsupported HPyMeth signature");
            return NULL;
        }
        dst->ml_doc = src->doc;
    }
    // copy the legacy methods
    for(int i=0; i<legacy_count; i++) {
        PyMethodDef *src = &legacy_methods[i];
        PyMethodDef *dst = &result[dst_idx++];
        dst->ml_name = src->ml_name;
        dst->ml_meth = src->ml_meth;
        dst->ml_flags = src->ml_flags;
        dst->ml_doc = src->ml_doc;
    }
    result[dst_idx++] = (PyMethodDef){NULL, NULL, 0, NULL};
    return result;
}

static PyType_Slot *
create_slot_defs(HPyType_Spec *hpyspec)
{
    // count the slots != HPy_meth
    Py_ssize_t slots_count;
    if (hpyspec->methods == NULL) {
        slots_count = 0;
    }
    else {
        slots_count = 0;
        for(int i=0; hpyspec->methods[i] != NULL; i++)
            if (hpyspec->methods[i]->slot != HPy_meth)
                slots_count++;
    }
    // add a slot to hold Py_tp_methods
    slots_count++;

    // allocate the result PyType_Slot array
    PyType_Slot *result = PyMem_Malloc(sizeof(PyType_Slot) * (slots_count+1));
    if (result == NULL)
        return NULL;

    // fill the result with non-meth slots
    int dst_idx = 0;
    for(int i=0; hpyspec->methods[i] != NULL; i++) {
        HPyMeth *src = hpyspec->methods[i];
        if (src->slot == HPy_meth)
            continue;
        PyType_Slot *dst = &result[dst_idx++];
        if (!(src->slot & HPy_SLOT)) {
            PyErr_SetString(PyExc_ValueError, "Invalid slot value: did you use HPy_*?");
            return NULL;
        }
        dst->slot = src->slot & ~HPy_SLOT;
        dst->pfunc = src->cpy_trampoline;
    }

    // add the "real" methods
    PyMethodDef *m = create_method_defs(hpyspec->methods, NULL);
    if (m == NULL) {
        PyMem_Free(result);
        return NULL;
    }
    result[dst_idx++] = (PyType_Slot){Py_tp_methods, m};

    // add the NULL sentinel at the end
    result[dst_idx++] = (PyType_Slot){0, NULL};
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
    if (hpyspec->legacy_slots != NULL)
        abort(); // FIXME
    spec->slots = create_slot_defs(hpyspec);
    if (spec->slots == NULL) {
        PyMem_Free(spec);
        PyErr_NoMemory();
        return HPy_NULL;
    }
    PyObject *result = PyType_FromSpec(spec);
    /* note that we do NOT free the memory which was allocated by
       create_method_defs, because that one is referenced internally by
       CPython (which probably assumes it's statically allocated) */
    PyMem_Free(spec->slots);
    PyMem_Free(spec);
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
