#include <Python.h>
#include "hpy.h"
#include "handles.h"
#include "ctx_misc.h"

HPyAPI_IMPL HPy
ctx_FromPyObject(HPyContext *ctx, cpy_PyObject *obj)
{
    Py_INCREF(obj);
    return _py2h(obj);
}

HPyAPI_IMPL cpy_PyObject *
ctx_AsPyObject(HPyContext *ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return obj;
}

HPyAPI_IMPL void
ctx_Close(HPyContext *ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XDECREF(obj);
}

HPyAPI_IMPL HPy
ctx_Dup(HPyContext *ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XINCREF(obj);
    return _py2h(obj);
}

HPyAPI_IMPL HPyField
ctx_Field_Store(HPyContext *ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return (HPyField){ ._i = h._i };
}

HPyAPI_IMPL HPy
ctx_Field_Load(HPyContext *ctx, HPyField f)
{
    HPy h = { ._i = f._i };
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return h;
}

HPyAPI_IMPL void
ctx_Field_Clear(HPyContext *ctx, HPyField *pf)
{
    HPy h = { ._i = pf->_i };
    Py_XDECREF(_h2py(h));
    *pf = (HPyField){ ._i = 0 };
}

HPyAPI_IMPL void
ctx_FatalError(HPyContext *ctx, const char *message)
{
    Py_FatalError(message);
}
