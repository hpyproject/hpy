#ifndef HPy_H
#define HPy_H

/* XXX: should we:
 *    - enforce PY_SSIZE_T_CLEAN in hpy
 *    - make it optional
 *    - make it the default but give a way to disable it?
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef PyObject *HPy;
typedef long HPyContext;

#define HPY_MODINIT_FUNC(NAME)                                          \
    static HPy _init ## NAME(HPyContext);                               \
    PyMODINIT_FUNC                                                      \
    PyInit_ ## NAME (void)                                              \
    {                                                                   \
        HPyContext ctx = 42; /* ??? */                                  \
        return _init ## NAME(ctx);                                      \
    }                                                                   \
    static HPy _init ## NAME
#endif /* !HPy_H */
