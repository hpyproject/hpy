/* =========== CPython-ONLY ===========
   In the following code, we use _py2h and _h2py and we assumed they are the
   ones defined by CPython's version of hpy.universal.

   DO NOT COMPILE THIS FILE UNLESS YOU ARE BUILDING CPython's hpy.universal.

   If you want to compile the debug mode into your own non-CPython version of
   hpy.universal, you should include debug_ctx_not_cpython.c.
   ====================================

   In theory, the debug mode is completely generic and can wrap a generic
   uctx. However, CPython is special because it does not have native support
   for HPy, so uctx contains the logic to call HPy functions from CPython, by
   using _HPy_CallRealFunctionFromTrampoline.

   uctx->ctx_CallRealFunctionFromTrampoline convers PyObject* into UHPy. So
   for the debug mode we need to:

       1. convert the PyObject* args into UHPys.
       2. wrap the UHPys into DHPys.
       3. unwrap the resulting DHPy and convert to PyObject*.
*/

#include <Python.h>
#include "debug_internal.h"
#include "handles.h" // for _py2h and _h2py

static inline DHPy _py2dh(HPyContext dctx, PyObject *obj)
{
    return DHPy_wrap(dctx, _py2h(obj));
}

static inline PyObject *_dh2py(DHPy dh)
{
    return _h2py(DHPy_unwrap(dh));
}

void debug_ctx_CallRealFunctionFromTrampoline(HPyContext dctx,
                                              HPyFunc_Signature sig,
                                              void *func, void *args)
{
    switch (sig) {
    case HPyFunc_NOARGS: {
        HPyFunc_noargs f = (HPyFunc_noargs)func;
        _HPyFunc_args_NOARGS *a = (_HPyFunc_args_NOARGS*)args;
        DHPy dh_self = _py2dh(dctx, a->self);
        DHPy dh_result = f(dctx, dh_self);
        a->result = _dh2py(dh_result);
        DHPy_close(dctx, dh_self);
        DHPy_close(dctx, dh_result);
        return;
    }
    case HPyFunc_O: {
        HPyFunc_o f = (HPyFunc_o)func;
        _HPyFunc_args_O *a = (_HPyFunc_args_O*)args;
        DHPy dh_self = _py2dh(dctx, a->self);
        DHPy dh_arg = _py2dh(dctx, a->arg);
        DHPy dh_result = f(dctx, dh_self, dh_arg);
        a->result = _dh2py(dh_result);
        DHPy_close(dctx, dh_self);
        DHPy_close(dctx, dh_arg);
        DHPy_close(dctx, dh_result);
        return;
    }
    case HPyFunc_VARARGS: {
        HPyFunc_varargs f = (HPyFunc_varargs)func;
        _HPyFunc_args_VARARGS *a = (_HPyFunc_args_VARARGS*)args;
        DHPy dh_self = _py2dh(dctx, a->self);
        Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
        DHPy dh_args[nargs * sizeof(DHPy)];
        for (Py_ssize_t i = 0; i < nargs; i++) {
            dh_args[i] = _py2dh(dctx, PyTuple_GET_ITEM(a->args, i));
        }
        a->result = _dh2py(f(dctx, dh_self, dh_args, nargs));
        DHPy_close(dctx, dh_self);
        for (Py_ssize_t i = 0; i < nargs; i++) {
            DHPy_close(dctx, dh_args[i]);
        }
        return;
    }
    case HPyFunc_KEYWORDS: {
        HPyFunc_keywords f = (HPyFunc_keywords)func;
        _HPyFunc_args_KEYWORDS *a = (_HPyFunc_args_KEYWORDS*)args;
        DHPy dh_self = _py2dh(dctx, a->self);
        Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
        DHPy dh_args[nargs * sizeof(DHPy)];
        for (Py_ssize_t i = 0; i < nargs; i++) {
            dh_args[i] = _py2dh(dctx, PyTuple_GET_ITEM(a->args, i));
        }
        DHPy dh_kw = _py2dh(dctx, a->kw);
        a->result = _dh2py(f(dctx, dh_self, dh_args, nargs, dh_kw));
        DHPy_close(dctx, dh_self);
        for (Py_ssize_t i = 0; i < nargs; i++) {
            DHPy_close(dctx, dh_args[i]);
        }
        DHPy_close(dctx, dh_kw);
        return;
    }
    case HPyFunc_INITPROC: {
        abort();
    }
    default:
        abort();  // XXX
    }
}
