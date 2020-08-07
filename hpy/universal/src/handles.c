#include <Python.h>
#include <stdlib.h>
#include "hpy.h"
#include "handles.h"


/* XXX we should turn the fast-path of _py2h and _h2py into macros in api.h */

static PyObject **all_handles;
static Py_ssize_t h_num_allocated = 0;
static Py_ssize_t h_free_list = -1;

static void
allocate_more_handles(void)
{
    Py_ssize_t base = (h_num_allocated < CONSTANT_H__TOTAL ?
                       CONSTANT_H__TOTAL : h_num_allocated);
    Py_ssize_t i, allocate = (base / 2) * 3 + 32;
    PyObject **new_handles = PyMem_Malloc(sizeof(PyObject *) * allocate);
    memcpy(new_handles, all_handles, sizeof(PyObject *) * h_num_allocated);

    for (i = allocate - 1; i >= base; i--) {
        new_handles[i] = (PyObject *)((h_free_list << 1) | 1);
        h_free_list = i;
    }

    if (h_num_allocated == 0) {
        new_handles[CONSTANT_H_NULL] = NULL;
        new_handles[CONSTANT_H_NONE] = Py_None;
        new_handles[CONSTANT_H_FALSE] = Py_False;
        new_handles[CONSTANT_H_TRUE] = Py_True;
        new_handles[CONSTANT_H_VALUEERROR] = PyExc_ValueError;
        new_handles[CONSTANT_H_TYPEERROR] = PyExc_TypeError;
        new_handles[CONSTANT_H_BASEOBJECTTYPE] = (PyObject *)&PyBaseObject_Type;
        new_handles[CONSTANT_H_TYPETYPE] = (PyObject *)&PyType_Type;
        new_handles[CONSTANT_H_LONGTYPE] = (PyObject *)&PyLong_Type;
        new_handles[CONSTANT_H_UNICODETYPE] = (PyObject *)&PyUnicode_Type;
        new_handles[CONSTANT_H_TUPLETYPE] = (PyObject *)&PyTuple_Type;
        new_handles[CONSTANT_H_LISTTYPE] = (PyObject *)&PyList_Type;
        if (CONSTANT_H__TOTAL != 12) {
            fprintf(stderr, "bad value for CONSTANT_H__TOTAL in handles.c\n");
            abort();
        }
    }

    PyMem_Free(all_handles);
    all_handles = new_handles;
    h_num_allocated = allocate;
}

HPy
_py2h(PyObject *obj)
{
    if (obj == NULL) {
        // Return the existing copy of HPy_NULL and don't create a new
        // handle.
        return HPy_NULL;
    }
    if (h_free_list < 0) {
        allocate_more_handles();
    }
    Py_ssize_t i = h_free_list;
    h_free_list = ((Py_ssize_t)all_handles[i]) >> 1;
    all_handles[i] = obj;
    return (HPy){i};
}

PyObject *
_h2py(HPy h)
{
    // If HPy_IsNull(h), the h._i = 0 and the line below returns the
    // pointer attached to the 0th handle, i.e. NULL.
    return all_handles[h._i];
}

void
_hclose(HPy h)
{
    Py_ssize_t i = h._i;
    Py_XDECREF(all_handles[i]);
    all_handles[i] = (PyObject *)((h_free_list << 1) | 1);
    h_free_list = i;
}
