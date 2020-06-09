#include "ctx_meth.h"
#include "handles.h"

HPyAPI_STORAGE cpy_PyObject *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, cpy_PyObject *self,
                                   cpy_PyObject *args, cpy_PyObject *kw,
                                   void *func, HPyFunc_Signature sig)
{
    switch (sig) {
    case HPyFunc_NOARGS: {
        HPyFunc_noargs f = (HPyFunc_noargs)func;
        return _h2py(f(ctx, _py2h(self)));
    }
    case HPyFunc_O: {
        HPyFunc_o f = (HPyFunc_o)func;
        return _h2py(f(ctx, _py2h(self), _py2h(args)));
    }
    case HPyFunc_VARARGS: {
        HPyFunc_varargs f = (HPyFunc_varargs)func;
        Py_ssize_t nargs = PyTuple_GET_SIZE(args);
        HPy *h_args = alloca(nargs * sizeof(HPy));
        for (Py_ssize_t i = 0; i < nargs; i++) {
            h_args[i] = _py2h(PyTuple_GET_ITEM(args, i));
        }
        return _h2py(f(ctx, _py2h(self), h_args, nargs));
    }
    case HPyFunc_KEYWORDS: {
       HPyFunc_keywords f = (HPyFunc_keywords)func;
       Py_ssize_t nargs = PyTuple_GET_SIZE(args);
       HPy *h_args = alloca(nargs * sizeof(HPy));
       for (Py_ssize_t i = 0; i < nargs; i++) {
           h_args[i] = _py2h(PyTuple_GET_ITEM(args, i));
       }
       return _h2py(f(ctx, _py2h(self), h_args, nargs, _py2h(kw)));
    }
    default:
        abort();  // XXX
    }
}
