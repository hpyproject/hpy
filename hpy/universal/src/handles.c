#include <Python.h>
#include <stdlib.h>
#include "hpy.h"
#include "handles.h"
#include "debugmode.h"


/* XXX we should turn the fast-path of _py2h and _h2py into macros in api.h */

static PyObject **all_handles;
static Py_ssize_t h_num_allocated = 0;
static Py_ssize_t h_free_list = -1;
static Py_ssize_t h_free_list_2 = -1;
/* note: h_free_list_2 is only here for debugging.  We can push freed
   handles directly into h_free_list.  But using two free lists makes
   reuses occur later and in a less deterministic order. */

static void
allocate_more_handles(void)
{
    if (h_free_list_2 >= 0) {
        h_free_list = h_free_list_2;
        h_free_list_2 = -1;
        return;
    }

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
        HPY_ASSERT(CONSTANT_H__TOTAL == 6,
            ("update handles.c with the list of constants"));
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
    HPY_ASSERT(h._i >= 0 && h._i < h_num_allocated,
        ("using an HPy containing garbage: _i = %zd", h._i));
    // If HPy_IsNull(h), the h._i = 0 and the line below returns the
    // pointer attached to the 0th handle, i.e. NULL.
    PyObject *result = all_handles[h._i];
    if (h._i == 0) {
        HPY_ASSERT(result == NULL, ("handle number 0 doesn't contain NULL"));
        return NULL;
    }
    HPY_ASSERT((((Py_ssize_t)result) & 1) == 0,
        ("using an HPy that was freed already (or never allocated): _i = %zd",
         h._i));
    HPY_ASSERT(result != NULL,
        ("NULL PyObject unexpected in handle _i = %zd", h._i));
    HPY_ASSERT(Py_REFCNT(result) > 0,
        ("bogus (freed?) PyObject found in handle _i = %zd", h._i));
    return result;
}

void
_hclose(HPy h)
{
    Py_ssize_t i = h._i;
    HPY_ASSERT(i >= 0 && i < h_num_allocated,
        ("freeing an HPy containing garbage: _i = %zd", i));
    HPY_ASSERT(i != 0, ("freeing HPy_NULL is not allowed"));
    PyObject *old = all_handles[i];
    HPY_ASSERT((((Py_ssize_t)old) & 1) == 0,
        ("freeing an HPy that was freed already (or never allocated): _i = %zd",
         i));
    HPY_ASSERT(Py_REFCNT(old) > 0,
        ("bogus PyObject found while freeing handle _i = %zd", h._i));
    all_handles[i] = (PyObject *)((h_free_list_2 << 1) | 1);
    h_free_list_2 = i;
    Py_DECREF(old);
}
