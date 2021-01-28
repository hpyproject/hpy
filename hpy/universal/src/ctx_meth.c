#include <Python.h>
#include "ctx_meth.h"
#include "handles.h"

HPyAPI_STORAGE void
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, HPyFunc_Signature sig,
                                   void *func, void *args)
{
    switch (sig) {
    case HPyFunc_NOARGS: {
        HPyFunc_noargs f = (HPyFunc_noargs)func;
        _HPyFunc_args_NOARGS *a = (_HPyFunc_args_NOARGS*)args;
        a->result = _h2py(f(ctx, _py2h(a->self)));
        return;
    }
    case HPyFunc_O: {
        HPyFunc_o f = (HPyFunc_o)func;
        _HPyFunc_args_O *a = (_HPyFunc_args_O*)args;
        a->result = _h2py(f(ctx, _py2h(a->self), _py2h(a->arg)));
        return;
    }
    case HPyFunc_VARARGS: {
        HPyFunc_varargs f = (HPyFunc_varargs)func;
        _HPyFunc_args_VARARGS *a = (_HPyFunc_args_VARARGS*)args;
        Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
        HPy h_args[nargs * sizeof(HPy)];
        for (Py_ssize_t i = 0; i < nargs; i++) {
            h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
        }
        a->result = _h2py(f(ctx, _py2h(a->self), h_args, nargs));
        return;
    }
    case HPyFunc_KEYWORDS: {
       HPyFunc_keywords f = (HPyFunc_keywords)func;
       _HPyFunc_args_KEYWORDS *a = (_HPyFunc_args_KEYWORDS*)args;
       Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
       HPy h_args[nargs * sizeof(HPy)];
       for (Py_ssize_t i = 0; i < nargs; i++) {
           h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
       }
       a->result = _h2py(f(ctx, _py2h(a->self), h_args, nargs, _py2h(a->kw)));
       return;
    }
    case HPyFunc_INITPROC: {
       HPyFunc_initproc f = (HPyFunc_initproc)func;
       _HPyFunc_args_INITPROC *a = (_HPyFunc_args_INITPROC*)args;
       Py_ssize_t nargs = PyTuple_GET_SIZE(a->args);
       HPy h_args[nargs * sizeof(HPy)];
       for (Py_ssize_t i = 0; i < nargs; i++) {
           h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
       }
       a->result = f(ctx, _py2h(a->self), h_args, nargs, _py2h(a->kw));
       return;
    }
#include "autogen_ctx_call.i"
    default:
        abort();  // XXX
    }
}


HPyAPI_STORAGE void
ctx_CallDestroyAndThenDealloc(HPyContext ctx, void *func, PyObject *self)
{
    /* It would be more consistent to call HPy_Cast or HPy_CastLegacy on
     * _py2h(self), but HPy_Cast calls _h2py(...) which checks whether
     * the reference count of the object passed is non-zero, which it isn't
     * at this point because the object is in the process of being destroyed.
     */
    void *obj = (void *)self;
    if (!(self->ob_type->tp_flags & HPy_TPFLAGS_LEGACY)) {
        obj = obj + 16; /* XXX */
    }
    HPyFunc_destroyfunc f = (HPyFunc_destroyfunc)func;
    f(obj);

    Py_TYPE(self)->tp_free(self);
}
