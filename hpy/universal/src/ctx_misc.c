#include <Python.h>
#include "hpy.h"
#include "handles.h"
#include "ctx_misc.h"

HPyAPI_STORAGE HPy
ctx_FromPyObject(HPyContext ctx, struct _object *obj)
{
    Py_INCREF(obj);
    return _py2h(obj);
}

HPyAPI_STORAGE struct _object *
ctx_AsPyObject(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return obj;
}

HPyAPI_STORAGE void
ctx_Close(HPyContext ctx, HPy h)
{
    _hclose(h);
}

HPyAPI_STORAGE HPy
ctx_Dup(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XINCREF(obj);
    return _py2h(obj);
}
