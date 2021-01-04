#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

_HPy_HIDDEN HPy
ctx_CallTupleDict(HPyContext ctx, HPy callable, HPy args, HPy kw)
{
    PyObject *obj = PyObject_Call(_h2py(callable), _h2py(args), _h2py(kw));
    return _py2h(obj);
}
