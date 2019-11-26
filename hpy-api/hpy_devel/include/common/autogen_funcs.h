#ifndef HPY_COMMON_AUTOGEN_FUNCS_H
#define HPY_COMMON_AUTOGEN_FUNCS_H

/* eventually this file will be automatically generated, at least for all the
 * functions which can ben translated automatically */

HPyAPI_FUNC(HPy)
_HPy_API_NAME(Long_FromLong)(HPyContext ctx, long v)
{
    return _py2h(PyLong_FromLong(v));
}

HPyAPI_FUNC(long)
_HPy_API_NAME(Long_AsLong)(HPyContext ctx, HPy h)
{
    return PyLong_AsLong(_h2py(h));
}

HPyAPI_FUNC(HPy)
_HPy_API_NAME(Number_Add)(HPyContext ctx, HPy x, HPy y)
{
    return _py2h(PyNumber_Add(_h2py(x), _h2py(y)));
}

HPyAPI_FUNC(HPy)
_HPy_API_NAME(Unicode_FromString)(HPyContext ctx, const char *utf8)
{
    return _py2h(PyUnicode_FromString(utf8));
}

HPyAPI_FUNC(int)
_HPy_API_NAME(Unicode_Check)(HPyContext ctx, HPy o)
{
    return PyUnicode_Check(_h2py(o));
}

HPyAPI_FUNC(HPy)
_HPy_API_NAME(Unicode_AsUTF8String)(HPyContext ctx, HPy o)
{
    return _py2h(PyUnicode_AsUTF8String(_h2py(o)));
}

HPyAPI_FUNC(void)
_HPy_API_NAME(Err_SetString)(HPyContext ctx, HPy type, const char *message)
{
    PyErr_SetString(_h2py(type), message);
}

/* bytesobject.h */
HPyAPI_FUNC(int)
_HPy_API_NAME(Bytes_Check)(HPyContext ctx, HPy o)
{
    return PyBytes_Check(_h2py(o));
}

HPyAPI_FUNC(HPy_ssize_t)
_HPy_API_NAME(Bytes_Size)(HPyContext ctx, HPy o)
{
    return PyBytes_Size(_h2py(o));
}

HPyAPI_FUNC(HPy_ssize_t)
_HPy_API_NAME(Bytes_GET_SIZE)(HPyContext ctx, HPy o)
{
    return PyBytes_GET_SIZE(_h2py(o));
}

HPyAPI_FUNC(char*)
_HPy_API_NAME(Bytes_AsString)(HPyContext ctx, HPy o)
{
    return PyBytes_AsString(_h2py(o));
}

HPyAPI_FUNC(char*)
_HPy_API_NAME(Bytes_AS_STRING)(HPyContext ctx, HPy o)
{
    return PyBytes_AS_STRING(_h2py(o));
}


#endif // HPY_COMMON_AUTOGEN_FUNCS_H
