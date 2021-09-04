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
ctx_Field_Store(HPyContext *ctx, HPy target_object, HPyField *target_field, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XDECREF(_hf2py(*target_field));
    Py_XINCREF(obj);
    *target_field = _py2hf(obj);
}

HPyAPI_IMPL HPy
ctx_Field_Load(HPyContext *ctx, HPy source_object, HPyField source_field)
{
    PyObject *obj = _hf2py(source_field);
    Py_INCREF(obj);
    return _py2h(obj);
}

HPyAPI_IMPL void
ctx_FatalError(HPyContext *ctx, const char *message)
{
    Py_FatalError(message);
}
