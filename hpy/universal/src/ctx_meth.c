#include <Python.h>
#include "ctx_meth.h"
#include "hpy/runtime/ctx_type.h"
#include "handles.h"

static void _buffer_h2py(HPyContext *ctx, const HPy_buffer *src, Py_buffer *dest)
{
    dest->buf = src->buf;
    dest->obj = HPy_AsPyObject(ctx, src->obj);
    dest->len = src->len;
    dest->itemsize = src->itemsize;
    dest->readonly = src->readonly;
    dest->ndim = src->ndim;
    dest->format = src->format;
    dest->shape = src->shape;
    dest->strides = src->strides;
    dest->suboffsets = src->suboffsets;
    dest->internal = src->internal;
}

static void _buffer_py2h(HPyContext *ctx, const Py_buffer *src, HPy_buffer *dest)
{
    dest->buf = src->buf;
    dest->obj = HPy_FromPyObject(ctx, src->obj);
    dest->len = src->len;
    dest->itemsize = src->itemsize;
    dest->readonly = src->readonly;
    dest->ndim = src->ndim;
    dest->format = src->format;
    dest->shape = src->shape;
    dest->strides = src->strides;
    dest->suboffsets = src->suboffsets;
    dest->internal = src->internal;
}

/* Some explanation about what's going on with tp_traverse.

   From the HPy user point of view, the visitor function passed to tp_traverse
   takes a ctx and an HPyField*:

       typedef int (*HPyFunc_visitproc)(HPyContext *, HPyField *, void *);
       ...
       static int my_traverse_impl(HPyContext *ctx, HPy self,
                                   HPyFunc_visitproc hpy_visit, void *arg)
       {
           MyCustomObject *obj = ...;
           hpy_visit(&obj->a, arg);  // obj->a is an HPyField
           hpy_visit(&obj->b, arg);
           ...
       }

   However, from the CPython point of view the visitor function passed to
   tp_traverse takes a PyObject*:

       typedef int (*visitproc)(cpy_PyObject *, void *);
       static int my_traverse(MyCustomObject *self, visitproc cpy_visit, void *arg)
       {
           cpy_visit(self->a, arg);  // self->a is a PyObject*
           cpy_visit(self->b, arg);
           ...
       }

   This is what happens:

   1. CPython calls the trampoline created by _HPyFunc_TRAMPOLINE_TRAVERSEPROC
      passing a visitor function. Let's call it cpy_visit, and remember that
      it takes a PyObject*.

   2. The trampoline invokes _HPy_CallRealFunctionFromTrampoline, which
      ultimately calls call_traverseproc_from_trampoline.

   3. call_traverseproc_from_trampoline invokes the tp_traverse_impl written
      by the HPy user, passing hpy2cpy_visit as the visitor function.

   4. tp_traverse_impl calls hpy2cpy_visit multiple times, once for every HPyField

   5. hpy2cpy_visit takes an HPyField*, converts it to a PyObject* and invokes
      the cpy_visit of point (1)
*/

typedef struct {
    cpy_visitproc cpy_visit;
    void *cpy_arg;
} hpy2cpy_visit_args_t;

static int hpy2cpy_visit(HPyContext *ctx, HPyField *f, void *v_args)
{
    hpy2cpy_visit_args_t *args = (hpy2cpy_visit_args_t *)v_args;
    cpy_visitproc cpy_visit = args->cpy_visit;
    void *cpy_arg = args->cpy_arg;
    PyObject *cpy_obj = _hf2py(*f);
    return cpy_visit(cpy_obj, cpy_arg);
}

static int call_traverseproc_from_trampoline(HPyContext *ctx,
                                             HPyFunc_traverseproc tp_traverse,
                                             HPy h_self,
                                             cpy_visitproc cpy_visit,
                                             void *cpy_arg)
{
    hpy2cpy_visit_args_t args = { cpy_visit, cpy_arg };
    return tp_traverse(ctx, h_self, hpy2cpy_visit, &args);
}


HPyAPI_IMPL void
ctx_CallRealFunctionFromTrampoline(HPyContext *ctx, HPyFunc_Signature sig,
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
        HPy *h_args = (HPy *)alloca(nargs * sizeof(HPy));
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
        HPy *h_args = (HPy *)alloca(nargs * sizeof(HPy));
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
        HPy *h_args = (HPy *)alloca(nargs * sizeof(HPy));
        for (Py_ssize_t i = 0; i < nargs; i++) {
            h_args[i] = _py2h(PyTuple_GET_ITEM(a->args, i));
        }
        a->result = f(ctx, _py2h(a->self), h_args, nargs, _py2h(a->kw));
        return;
    }
    case HPyFunc_GETBUFFERPROC: {
        HPyFunc_getbufferproc f = (HPyFunc_getbufferproc)func;
        _HPyFunc_args_GETBUFFERPROC *a = (_HPyFunc_args_GETBUFFERPROC*)args;
        HPy_buffer hbuf;
        a->result = f(ctx, _py2h(a->self), &hbuf, a->flags);
        if (a->result < 0) {
            a->view->obj = NULL;
            return;
        }
        _buffer_h2py(ctx, &hbuf, a->view);
        HPy_Close(ctx, hbuf.obj);
        return;
    }
    case HPyFunc_RELEASEBUFFERPROC: {
        HPyFunc_releasebufferproc f = (HPyFunc_releasebufferproc)func;
        _HPyFunc_args_RELEASEBUFFERPROC *a = (_HPyFunc_args_RELEASEBUFFERPROC*)args;
        HPy_buffer hbuf;
        _buffer_py2h(ctx, a->view, &hbuf);
        f(ctx, _py2h(a->self), &hbuf);
        // XXX: copy back from hbuf?
        HPy_Close(ctx, hbuf.obj);
        return;
    }
    case HPyFunc_TRAVERSEPROC: {
        HPyFunc_traverseproc f = (HPyFunc_traverseproc)func;
        _HPyFunc_args_TRAVERSEPROC *a = (_HPyFunc_args_TRAVERSEPROC*)args;
        a->result = call_traverseproc_from_trampoline(ctx, f, _py2h(a->self),
                                                      a->visit, a->arg);
        return;
    }
#include "autogen_ctx_call.i"
    default:
        abort();  // XXX
    }
}


HPyAPI_IMPL void
ctx_CallDestroyAndThenDealloc(HPyContext *ctx, void *func, PyObject *self)
{
    /* It would be more consistent to call HPy_AsStruct or HPy_AsStructLegacy on
     * _py2h(self), but HPy_AsStruct calls _h2py(...) which checks whether
     * the reference count of the object passed is non-zero, which it isn't
     * at this point because the object is in the process of being destroyed.
     */
    void *obj = (void *)self;
    if (self->ob_type->tp_flags & HPy_TPFLAGS_INTERNAL_PURE) {
        obj = _HPy_PyObject_Payload(self);
    }
    HPyFunc_destroyfunc f = (HPyFunc_destroyfunc)func;
    f(obj);

    Py_TYPE(self)->tp_free(self);
}
