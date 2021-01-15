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

void debug_ctx_CallRealFunctionFromTrampoline(HPyContext ctx,
                                              HPyFunc_Signature sig,
                                              void *func, void *args)
{
    switch (sig) {
    case HPyFunc_NOARGS: {
        HPyFunc_noargs f = (HPyFunc_noargs)func;
        _HPyFunc_args_NOARGS *a = (_HPyFunc_args_NOARGS*)args;
        DHPy dh_self = DHPy_wrap(ctx, _py2h(a->self));
        DHPy dh_result = f(ctx, dh_self);
        a->result = _h2py(DHPy_unwrap(dh_result));
        return;
    }
    case HPyFunc_O: {
        abort();
    }
    case HPyFunc_VARARGS: {
        abort();
    }
    case HPyFunc_KEYWORDS: {
        abort();
    }
    case HPyFunc_INITPROC: {
        abort();
    }
    default:
        abort();  // XXX
    }
}
