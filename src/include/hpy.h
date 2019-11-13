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

typedef struct { PyObject *_o; } HPy;
typedef long HPyContext;

static inline HPyContext _HPyGetContext(void) {
    return 42;
}

/* For internal usage only. These should be #undef at the end of this header.
   If you need to convert HPy to PyObject* and vice-versa, you should use the
   official way to do it (not implemented yet :)
*/
#define _h2py(x) (x._o)
#define _py2h(o) ((HPy){o})


#define HPy_NULL ((HPy){NULL})
#define HPy_IsNull(x) ((x)._o == NULL)
#define HPy_RETURN_NONE return Py_INCREF(Py_None), _py2h(Py_None)

/* moduleobject.h */
typedef PyModuleDef HPyModuleDef;
#define HPyModuleDef_HEAD_INIT PyModuleDef_HEAD_INIT

static inline HPy HPyModule_Create(HPyContext ctx, HPyModuleDef *mdef) {
    return _py2h(PyModule_Create(mdef));
}

#define HPy_MODINIT(modname)                               \
    static HPy HPyInit_##modname(HPyContext ctx);          \
    PyMODINIT_FUNC                                         \
    PyInit_##modname(void)                                 \
    {                                                      \
        return _h2py(HPyInit_##modname(_HPyGetContext())); \
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
        return _h2py(NAME##_impl(_HPyGetContext(), _py2h(self), _py2h(args)));\
    }


static int HPyArg_ParseTuple(HPyContext ctx, HPy args, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int res = PyArg_VaParse(_h2py(args), fmt, vl);
    va_end(vl);
    /* XXX incref all returned 'PyObject*' */
    return res;
}


static HPy HPyLong_FromLong(HPyContext ctx, long v)
{
    return _py2h(PyLong_FromLong(v));
}


#endif /* !HPy_H */
