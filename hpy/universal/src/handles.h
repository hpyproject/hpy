#ifndef HPY_HANDLES_H
#define HPY_HANDLES_H

#include <Python.h>
#include "hpy.h"

// represent handles as ~address. The main reason is to make sure that if
// people casts HPy to PyObject* directly, things explode.

static inline HPy _py2h(PyObject *obj) {
    if (obj == NULL)
        return HPy_NULL;
    return (HPy){~(HPy_ssize_t)obj};
}

static inline PyObject *_h2py(HPy h) {
    if HPy_IsNull(h)
        return NULL;
    return (PyObject *)~h._i;
}

#endif /* HPY_HANDLES_H */
