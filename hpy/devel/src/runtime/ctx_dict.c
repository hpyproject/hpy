#include <Python.h>
#include "hpy.h"

#ifndef HPY_ABI_CPYTHON
   // for _h2py and _py2h
#  include "handles.h"
#endif


_HPy_HIDDEN HPy
ctx_Dict_GetItem(HPyContext *ctx, HPy op, HPy key)
{
    PyObject *res = PyDict_GetItem(_h2py(op), _h2py(key));
    Py_XINCREF(res);
    return _py2h(res);
}
