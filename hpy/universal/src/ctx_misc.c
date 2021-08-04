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

HPyAPI_IMPL void
ctx_Field_Store(HPyContext *ctx, HPyField *target, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XDECREF(_hf2py(*target));
    Py_INCREF(obj);
    *target = _py2hf(obj);
}

HPyAPI_IMPL HPy
ctx_Field_Load(HPyContext *ctx, HPyField f)
{
    PyObject *obj = _hf2py(f);
    Py_INCREF(obj);
    return _py2h(obj);
}

HPyAPI_IMPL void
ctx_Field_Clear(HPyContext *ctx, HPyField *pf)
{
    PyObject *obj = _hf2py(*pf);
    Py_XDECREF(obj);
    *pf = HPyField_NULL;
}

HPyAPI_IMPL void
ctx_FatalError(HPyContext *ctx, const char *message)
{
    Py_FatalError(message);
}
