#include "ctx_meth.h"
#include "handles.h"

/* should these typedefs officialized as part of the official API? */
typedef HPy (*HPyMeth_NoArgs)(HPyContext, HPy self);
typedef HPy (*HPyMeth_O)(HPyContext, HPy self, HPy arg);
typedef HPy (*HPyMeth_VarArgs)(HPyContext, HPy self, HPy *args, HPy_ssize_t);
typedef HPy (*HPyMeth_Keywords)(HPyContext, HPy self, HPy *args, HPy_ssize_t,
                                HPy kw);

HPyAPI_STORAGE struct _object *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, struct _object *self,
                                   struct _object *args, struct _object *kw,
                                   void *func, int ml_flags)
{
    switch (ml_flags)
    {
    case HPy_METH_NOARGS: {
        HPyMeth_NoArgs f = (HPyMeth_NoArgs)func;
        return _h2py(f(ctx, _py2h(self)));
    }
    case HPy_METH_O: {
        HPyMeth_O f = (HPyMeth_O)func;
        return _h2py(f(ctx, _py2h(self), _py2h(args)));
    }
    case HPy_METH_VARARGS: {
        HPyMeth_VarArgs f = (HPyMeth_VarArgs)func;
        Py_ssize_t nargs = PyTuple_GET_SIZE(args);
        HPy *h_args = alloca(nargs * sizeof(HPy));
        for (Py_ssize_t i = 0; i < nargs; i++) {
            h_args[i] = _py2h(PyTuple_GET_ITEM(args, i));
        }
        return _h2py(f(ctx, _py2h(self), h_args, nargs));
    }
    case HPy_METH_KEYWORDS: {
       HPyMeth_Keywords f = (HPyMeth_Keywords)func;
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
