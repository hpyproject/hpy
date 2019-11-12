#ifndef HPy_H
#define HPy_H

/* XXX: it would be nice if we could include hpy.h WITHOUT bringing in all the
   stuff from Python.h, to make sure that people don't use the CPython API by
   mistake. How to achieve it, though? */

/* XXX: should we:
 *    - enforce PY_SSIZE_T_CLEAN in hpy
 *    - make it optional
 *    - make it the default but give a way to disable it?
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef PyObject *HPy;
typedef long HPyContext;

static inline HPyContext _HPyGetContext(void) {
    return 42;
}

static inline HPy _HPyFromPy(HPyContext ctx, PyObject *obj) {
    return obj;
}

#define HPy_RETURN_NONE Py_RETURN_NONE

/* moduleobject.h */
typedef PyModuleDef HPyModuleDef;
#define HPyModuleDef_HEAD_INIT PyModuleDef_HEAD_INIT

static inline HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *mdef) {
    return _HPyFromPy(ctx, PyModule_Create(mdef));
}

#define HPy_DECLARE_MODINIT(MODNAME, FUNC)          \
    PyMODINIT_FUNC                                  \
    PyInit_##MODNAME(void)                          \
    {                                               \
        return FUNC(_HPyGetContext());              \
    }

/* methodobject.h */
typedef PyMethodDef HPyMethodDef;


/* function declaration */

/* XXX: this is a bit undesirable because it introduces an indirection and an
   extra function call. Unsure how to solve it, though. */
#define HPy_FUNCTION(NAME)                                              \
    static HPy NAME##_impl(HPyContext, HPy, HPy);                       \
    static PyObject* NAME(PyObject *self, PyObject *args)               \
    {                                                                   \
        return NAME##_impl(_HPyGetContext(), self, args);               \
    }

#endif /* !HPy_H */
