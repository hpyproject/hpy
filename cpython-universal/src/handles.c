#include <Python.h>
#include <stdlib.h>
#include "api.h"


/* XXX we should turn the fast-path of _py2h and _h2py into macros in api.h */

static PyObject **all_handles;
static Py_ssize_t h_num_allocated = 0;
static Py_ssize_t h_free_list = -1;

static void
allocate_more_handles(void)
{
    Py_ssize_t i, allocate = (h_num_allocated / 2) * 3 + 32;
    PyObject **new_handles = PyMem_Malloc(sizeof(PyObject *) * allocate);
    memcpy(new_handles, all_handles, sizeof(PyObject *) * h_num_allocated);

    for (i = allocate - 1; i >= h_num_allocated && i > 0; i--) {
        new_handles[i] = (PyObject *)((h_free_list << 1) | 1);
        h_free_list = i;
    }

    PyMem_Free(all_handles);
    all_handles = new_handles;
    h_num_allocated = allocate;
}

HPy
_py2h(PyObject *obj)
{
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
