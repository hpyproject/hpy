#include <Python.h>
#include <stdlib.h>
#include "api.h"
#include "ctx_module.h"

static void*
ctx_Cast(HPyContext ctx, HPy h)
{
    return (void*)_h2py(h);
}

/* HPyMeth */

typedef HPy (*HPyMeth_NoArgs)(HPyContext, HPy self);
typedef HPy (*HPyMeth_O)(HPyContext, HPy self, HPy arg);
typedef HPy (*HPyMeth_VarArgs)(HPyContext, HPy self, HPy *args, HPy_ssize_t);
typedef HPy (*HPyMeth_Keywords)(HPyContext, HPy self, HPy *args, HPy_ssize_t,
                                HPy kw);

static struct _object *
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

static HPy
ctx_FromPyObject(HPyContext ctx, struct _object *obj)
{
    Py_INCREF(obj);
    return _py2h(obj);
}

static struct _object *
ctx_AsPyObject(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_INCREF(obj);
    return obj;
}

static void
ctx_Close(HPyContext ctx, HPy h)
{
    _hclose(h);
}

static HPy
ctx_Dup(HPyContext ctx, HPy h)
{
    PyObject *obj = _h2py(h);
    Py_XINCREF(obj);
    return _py2h(obj);
}

static PyType_Slot *
create_slot_defs(HPyType_Spec *hpyspec)
{
    // count the slots
    Py_ssize_t count;
    if (hpyspec->slots == NULL) {
        count = 0;
    }
    else {
        count = 0;
        while (hpyspec->slots[count].slot != 0)
            count++;
    }

    // allocate&fill the result
    PyType_Slot *result = PyMem_Malloc(sizeof(PyType_Slot) * (count+1));
    if (result == NULL)
        return NULL;
    for(int i=0; i<count; i++) {
        HPyType_Slot *src = &hpyspec->slots[i];
        PyType_Slot *dst = &result[i];
        dst->slot = src->slot;
        if (src->slot == Py_tp_methods) {
            // src->pfunc contains a HPyMethodDef*, which we convert to the
            // CPython's equivalent. The result of create_method_defs can't be
            // freed because CPython stores an internal reference to it, so
            // this creates a small leak :(
            HPyMethodDef *hpymethods = (HPyMethodDef*)src->pfunc;
            dst->pfunc = create_method_defs(hpymethods);
            if (dst->pfunc == NULL) {
                PyMem_Free(result);
                return NULL;
            }
        }
        else {
            // this must be a slot which contains a function pointer.  For now
            // we assume that it uses the HPy calling convention, but we need
            // a way to allow slots with the CPython signature, to make it
            // easier to do incremental porting.
            HPyMeth f = (HPyMeth)hpyspec->slots[i].pfunc;
            void *impl_func;
            _HPy_CPyCFunction trampoline_func;
            f(&impl_func, &trampoline_func);
            dst->pfunc = trampoline_func;
        }
    }
    result[count] = (PyType_Slot){0, NULL};
    return result;
}

static HPy ctx_Type_FromSpec(HPyContext ctx, HPyType_Spec *hpyspec)
{
    PyType_Spec *spec = PyMem_Malloc(sizeof(PyType_Spec));
    if (spec == NULL) {
        PyErr_NoMemory();
        return HPy_NULL;
    }
    spec->name = hpyspec->name;
    spec->basicsize = hpyspec->basicsize;
    spec->itemsize = hpyspec->itemsize;
    spec->flags = hpyspec->flags;
    spec->slots = create_slot_defs(hpyspec);
    if (spec->slots == NULL) {
        PyMem_Free(spec);
        PyErr_NoMemory();
        return HPy_NULL;
    }
    PyObject *result = PyType_FromSpec(spec);
    return _py2h(result);
}

/* expand impl functions as:
 *     static ctx_Long_FromLong(...);
 *
 * Then, they are automatically stored in the global context by
 * autogen_ctx_def
 */
#define _HPy_IMPL_NAME(name) ctx_##name
#define _HPy_IMPL_NAME_NOPREFIX(name) ctx_##name
#include "common/implementation.h"
#undef _HPy_IMPL_NAME_NOPREFIX
#undef _HPy_IMPL_NAME

#include "autogen_ctx_def.h"
