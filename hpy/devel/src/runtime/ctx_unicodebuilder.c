#include <stddef.h>
#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI
   // for _h2py and _py2h
#  include "handles.h"
#endif

#include <stdio.h>

_HPy_HIDDEN HPyUnicodeBuilder
ctx_UnicodeBuilder_New(HPyContext *ctx)
{
    PyObject *lst = PyList_New(0);
    if (lst == NULL)
        PyErr_Clear();   /* delay the MemoryError */
    return (HPyUnicodeBuilder){(HPy_ssize_t)lst };
}

_HPy_HIDDEN void
ctx_UnicodeBuilder_Append(HPyContext *ctx, HPyUnicodeBuilder builder, HPy h_item)
{
    PyObject *lst = (PyObject *)builder._lst;
    if (lst != NULL) {
        PyObject *item = _h2py(h_item);
        PyList_Append(lst, item);
    }
}

_HPy_HIDDEN HPy
ctx_UnicodeBuilder_Build(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    PyObject *lst = (PyObject *)builder._lst;
    if (lst == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    builder._lst = 0;
    PyObject *sep = PyUnicode_FromString("");
    PyObject *str = PyUnicode_Join(sep, lst);
    Py_XDECREF(sep);
    if(str == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    Py_XDECREF(lst);
    return _py2h(str);
}

_HPy_HIDDEN void
ctx_UnicodeBuilder_Cancel(HPyContext *ctx, HPyUnicodeBuilder builder)
{
    PyObject *lst = (PyObject *)builder._lst;
    if (lst == NULL) {
        // we don't report the memory error here: the builder
        // is being cancelled (so the result of the builder is not being used)
        // and likely it's being cancelled during the handling of another error
        return;
    }
    builder._lst = 0;
    Py_XDECREF(lst);
}
