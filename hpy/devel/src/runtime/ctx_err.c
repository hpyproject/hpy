#include <Python.h>
#include "hpy.h"
#include "hpy/runtime/ctx_funcs.h"

#ifndef HPY_ABI_CPYTHON
   // for _h2py and _py2h
#  include "handles.h"
#endif

_HPy_HIDDEN int
ctx_Err_Occurred(HPyContext *ctx) {
    return PyErr_Occurred() ? 1 : 0;
}
