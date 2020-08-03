#include "ctx_meth.h"
#include "handles.h"

HPyAPI_STORAGE cpy_PyObject *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, HPyFunc_Signature sig,
                                   void *func, void *args)
{
    switch (sig) {
    case HPyFunc_NOARGS: {
        HPyFunc_noargs f = (HPyFunc_noargs)func;
        _HPyFunc_args_NOARGS *a = (_HPyFunc_args_NOARGS*)args;
        return _h2py(f(ctx, _py2h(a->self)));
    }
    case HPyFunc_O: {
        HPyFunc_o f = (HPyFunc_o)func;
        _HPyFunc_args_O *a = (_HPyFunc_args_O*)args;
        return _h2py(f(ctx, _py2h(a->self), _py2h(a->arg)));
    }
    case HPyFunc_VARARGS: {
        HPyFunc_varargs f = (HPyFunc_varargs)func;
        _HPyFunc_args_VARARGS *a = (_HPyFunc_args_VARARGS*)args;
        Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
        HPy *h_args = alloca(nargs * sizeof(HPy));
        for (Py_ssize_t i = 0; i < nargs; i++) {
            h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
        }
        return _h2py(f(ctx, _py2h(a->self), h_args, nargs));
    }
    case HPyFunc_KEYWORDS: {
       HPyFunc_keywords f = (HPyFunc_keywords)func;
       _HPyFunc_args_KEYWORDS *a = (_HPyFunc_args_KEYWORDS*)args;
       Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
       HPy *h_args = alloca(nargs * sizeof(HPy));
       for (Py_ssize_t i = 0; i < nargs; i++) {
           h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
       }
       return _h2py(f(ctx, _py2h(a->self), h_args, nargs, _py2h(a->kw)));
    }
    default:
        abort();  // XXX
    }
}
