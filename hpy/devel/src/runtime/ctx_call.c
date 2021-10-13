#include <Python.h>
#include "hpy.h"
#if defined(_MSC_VER)
# include <malloc.h>   /* for alloca() */
#endif

#ifndef HPY_ABI_CPYTHON
   // for _h2py and _py2h
#  include "handles.h"
#endif

_HPy_HIDDEN HPy
ctx_CallTupleDict(HPyContext *ctx, HPy callable, HPy args, HPy kw)
{
    PyObject *obj;
    if (!HPy_IsNull(args) && !HPyTuple_Check(ctx, args)) {
       HPyErr_SetString(ctx, ctx->h_TypeError,
           "HPy_CallTupleDict requires args to be a tuple or null handle");
       return HPy_NULL;
    }
    if (!HPy_IsNull(kw) && !HPyDict_Check(ctx, kw)) {
       HPyErr_SetString(ctx, ctx->h_TypeError,
           "HPy_CallTupleDict requires kw to be a dict or null handle");
       return HPy_NULL;
    }
    if (HPy_IsNull(kw)) {
        obj = PyObject_CallObject(_h2py(callable), _h2py(args));
    }
    else if (!HPy_IsNull(args)){
        obj = PyObject_Call(_h2py(callable), _h2py(args), _h2py(kw));
    }
    else {
        // args is null, but kw is not, so we need to create an empty args tuple
        // for CPython's PyObject_Call
        HPy *items = NULL;
        HPy empty_tuple = HPyTuple_FromArray(ctx, items, 0);
        obj = PyObject_Call(_h2py(callable), _h2py(empty_tuple), _h2py(kw));
        HPy_Close(ctx, empty_tuple);
    }
    return _py2h(obj);
}

_HPy_UNUSED static inline void
_harr2pyarr(PyObject *dest[], const HPy_ssize_t dest_offset, HPy src[], HPy_ssize_t n)
{
    HPy_ssize_t i;
    for (i = 0; i < n; i++) {
        dest[i + dest_offset] = _h2py(src[i]);
    }
}

_HPy_HIDDEN HPy
ctx_CallVectorDict(HPyContext *ctx, HPy callable, HPy args[], HPy_ssize_t nargs, HPy kw)
{
    PyObject *result;

    if (!HPy_IsNull(kw) && !HPyDict_Check(ctx, kw)) {
       HPyErr_SetString(ctx, ctx->h_TypeError,
           "HPy_CallVectorDict requires kw to be a dict or null handle");
       return HPy_NULL;
    }

#if PY_VERSION_HEX >= 0x03080000
    PyObject **py_args = (PyObject **) alloca(nargs * sizeof(PyObject *));
    _harr2pyarr(py_args, 0, args, nargs);
#endif

#if PY_VERSION_HEX >= 0x03090000
    result = PyObject_VectorcallDict(_h2py(callable), py_args, nargs, _h2py(kw));
#elif PY_VERSION_HEX >= 0x03080000 
    result = _PyObject_FastCallDict(_h2py(callable), py_args, nargs, _h2py(kw));
#else
    /* Before Python 3.8, we need to convert to a tuple and use 'PyObject_Call'. */
    HPy args_tuple = HPyTuple_FromArray(ctx, args, nargs);
    if (HPy_IsNull(kw)) {
        result = PyObject_CallObject(_h2py(callable), _h2py(args_tuple));
    } else {
        result = PyObject_Call(_h2py(callable), _h2py(args_tuple), _h2py(kw));
    }
    HPy_Close(ctx, args_tuple);
#endif
    return _py2h(result);
}

_HPy_HIDDEN HPy
ctx_CallMethodVectorDict(HPyContext *ctx, HPy receiver, HPy name, HPy args[], HPy_ssize_t nargs, HPy kw)
{
    PyObject *result;

    if (!HPy_IsNull(kw) && !HPyDict_Check(ctx, kw)) {
       HPyErr_SetString(ctx, ctx->h_TypeError,
           "HPy_CallMethodVectorDict requires kw to be a dict or null handle");
       return HPy_NULL;
    }

#if PY_VERSION_HEX >= 0x03090000
    PyObject **py_args;
    if (HPy_IsNull(kw)) {
        /* If we don't have keywords, we can use PyObject_VectorcallMethod */
        py_args = (PyObject **) alloca((nargs + 1) * sizeof(PyObject *));
        py_args[0] = _h2py(receiver);
        _harr2pyarr(py_args, 1, args, nargs);
        result = PyObject_VectorcallMethod(_h2py(name), py_args, nargs + 1, NULL);
    } else {
        /* If we have keywords, we use PyObject_VectorcallDict and lookup the method manually */
        HPy callable = HPy_GetAttr(ctx, receiver, name);
        if (HPy_IsNull(callable)) {
            return HPy_NULL;
        }
        py_args = (PyObject **) alloca(nargs * sizeof(PyObject *));
        _harr2pyarr(py_args, 0, args, nargs);
        result = PyObject_VectorcallDict(_h2py(callable), py_args, nargs, _h2py(kw));
        HPy_Close(ctx, callable);
    }
#elif PY_VERSION_HEX >= 0x03080000 /* >= 3.8 */
    HPy callable = HPy_GetAttr(ctx, receiver, name);
    if (HPy_IsNull(callable)) {
        return HPy_NULL;
    }
    PyObject **py_args = (PyObject **) alloca(nargs * sizeof(PyObject *));
    _harr2pyarr(py_args, 0, args, nargs);
    if (HPy_IsNull(kw)) {
        result = _PyObject_Vectorcall(_h2py(callable), py_args, nargs, NULL);
    } else {
        result = _PyObject_FastCallDict(_h2py(callable), py_args, nargs, _h2py(kw));
    }
    HPy_Close(ctx, callable);
#else
    /* Before Python 3.8, we need to convert to a tuple and use 'PyObject_Call'. */
    HPy callable = HPy_GetAttr(ctx, receiver, name);
    if (HPy_IsNull(callable)) {
        return HPy_NULL;
    }
    HPy args_tuple = HPyTuple_FromArray(ctx, args, nargs);
    if (HPy_IsNull(kw)) {
        result = PyObject_CallObject(_h2py(callable), _h2py(args_tuple));
    } else {
        result = PyObject_Call(_h2py(callable), _h2py(args_tuple), _h2py(kw));
    }
    HPy_Close(ctx, args_tuple);
    HPy_Close(ctx, callable);
#endif /* >= 3.9 */
    return _py2h(result);
}
