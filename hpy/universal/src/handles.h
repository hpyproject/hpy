#ifndef HPY_HANDLES_H
#define HPY_HANDLES_H

#include <Python.h>
#include "hpy.h"

// The main reason for +1/-1 is to make sure that if people casts HPy to
// PyObject* directly, things explode. Moreover, with this we can easily
// distinguish normal and debug handles in gdb, by only looking at the last
// bit.

static inline HPy _py2h(PyObject *obj) {
    if (obj == NULL)
        return HPy_NULL;
    return (HPy){(HPy_ssize_t)obj + 1};
}

static inline PyObject *_h2py(HPy h) {
    if HPy_IsNull(h)
        return NULL;
    return (PyObject *)(h._i - 1);
}

static inline HPyField _py2hf(PyObject *obj)
{
    return (HPyField){(intptr_t) obj};
}

static inline PyObject * _hf2py(HPyField hf)
{
    return (PyObject*)hf._i;
}

#endif /* HPY_HANDLES_H */
