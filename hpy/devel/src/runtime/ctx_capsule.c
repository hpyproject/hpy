#include <Python.h>
#include "hpy.h"

#ifdef HPY_UNIVERSAL_ABI

    // for _h2py and _py2h
#   include "handles.h"
#endif

#define DESTRUCTOR_REGISTRY_KEY "_hpycapsule_destructors"

static PyObject *
hpy_get_destructor_registry()
{
    PyObject *interp_dict;
    PyObject *key;
    PyObject *res;

    /* borrowed ref */
    interp_dict = PyInterpreterState_GetDict(PyThreadState_Get()->interp);
    if (interp_dict == NULL) {
        PyErr_SetString(PyExc_SystemError, "HPyCapsule destructor registry is not available");
        return NULL;
    }
    key = PyUnicode_InternFromString(DESTRUCTOR_REGISTRY_KEY);
    if (key == NULL) {
        return NULL;
    }
    res = PyDict_GetItem(interp_dict, key); /* borrowed ref */
    if (res == NULL) {
        /* create the HPy capsule destructor registry on demand */
        res = PyDict_New();
        PyDict_SetItem(interp_dict, key, res);
        /* this function returns a borrowed ref */
        Py_DECREF(res);
    }
    Py_DECREF(key);
    return res;
}

static HPyCapsule_Destructor
get_hpy_destructor_function(PyObject *capsule)
{
    /* we only register this destructor if the user provided a destructor
       function at creation time */
    PyObject *hpy_destructor_registry = hpy_get_destructor_registry();

    PyObject *key = PyLong_FromVoidPtr(capsule);
    if (key == NULL) {
        return NULL;
    }

    PyObject *hpy_destructor_capsule = PyDict_GetItem(hpy_destructor_registry, key);
    Py_DECREF(key);
    void *ptr = PyCapsule_GetPointer(hpy_destructor_capsule, NULL);
    if (ptr == NULL) {
        return NULL;
    }

    return (HPyCapsule_Destructor) ptr;
}

static void hpy_capsule_destructor_trampoline(PyObject *capsule)
{
    HPyCapsule_Destructor hpy_destr = get_hpy_destructor_function(capsule);
    if (hpy_destr == NULL) {
        PyErr_Clear();
        return;
    }

    /* call the user-supplied destructor function */
    const char *name = PyCapsule_GetName(capsule);
    hpy_destr(name, PyCapsule_GetPointer(capsule, name),
            PyCapsule_GetContext(capsule));
}

_HPy_HIDDEN HPy
ctx_Capsule_New(HPyContext *ctx, void *pointer, const char *name, HPyCapsule_Destructor destructor)
{
    PyObject *registry;
    PyObject *destructor_capsule;
    PyObject *res;
    PyObject *key;

    if (destructor) {
        registry = hpy_get_destructor_registry(); /* borrowed ref */
        if (registry == NULL) {
            return HPy_NULL;
        }

        /* Now create the real capsule since we need it to create the key for
           the registry */
        res = PyCapsule_New(pointer, name,
                (PyCapsule_Destructor) hpy_capsule_destructor_trampoline);
        if (res == NULL) {
            goto error;
        }

        key = PyLong_FromVoidPtr(res);
        if (key == NULL) {
            goto error;
        }

        /* We wrap the pointer to the user-supplied destructor function in a
           a PyCapsule. */
        destructor_capsule = PyCapsule_New((void *) destructor, NULL, NULL);
        if (destructor_capsule == NULL) {
            goto error;
        }

        if (PyDict_SetItem(registry, key, destructor_capsule) < 0) {
            goto error;
        }
        Py_DECREF(key);
        Py_DECREF(destructor_capsule);
    } else {
        res = PyCapsule_New(pointer, name, NULL);
    }

    return _py2h(res);

error:
    /* Ensure that we don't run 'hpy_capsule_destructor_trampoline' in the
       error case because the trampoline will fail since the registration
       is not finised yet */
    if (res) {
        PyCapsule_SetDestructor(res, NULL);
        Py_DECREF(res);
        Py_XDECREF(key);
        Py_XDECREF(destructor_capsule);
    }
    return HPy_NULL;
}

_HPy_HIDDEN HPyCapsule_Destructor
ctx_Capsule_GetDestructor(HPyContext *ctx, HPy h_capsule)
{
    PyObject *capsule = _h2py(h_capsule);
    PyCapsule_Destructor py_destructor = PyCapsule_GetDestructor(capsule);

    if (py_destructor == hpy_capsule_destructor_trampoline)
    {
        return get_hpy_destructor_function(capsule);
    }

    /* We do not expose foreign PyCapsule destructor functions since they have
       a different signature. */
    return NULL;
}

_HPy_HIDDEN int
ctx_Capsule_SetDestructor(HPyContext *ctx, HPy h_capsule,
        HPyCapsule_Destructor destructor)
{
    PyObject *capsule;
    PyObject *registry;
    PyObject *destructor_capsule;
    PyObject *key;
    int i;

    capsule = _h2py(h_capsule);

    registry = hpy_get_destructor_registry(); /* borrowed ref */
    if (registry == NULL) {
        return -1;
    }
    /* We wrap the pointer to the user-supplied destructor function in a
       a PyCapsule. */
    destructor_capsule = PyCapsule_New((void *) destructor, NULL, NULL);
    if (destructor_capsule == NULL) {
        return -1;
    }

    key = PyLong_FromVoidPtr(capsule);
    if (key == NULL) {
        Py_DECREF(destructor_capsule);
        return -1;
    }

    i = PyDict_SetItem(registry, key, destructor_capsule);
    Py_DECREF(key);
    Py_DECREF(destructor_capsule);
    if (i < 0) {
        return -1;
    }

    PyCapsule_SetDestructor(capsule,
            (PyCapsule_Destructor) hpy_capsule_destructor_trampoline);
    return 0;
}

#ifdef HPY_UNIVERSAL_ABI
_HPy_HIDDEN void *
ctx_Capsule_Get(HPyContext *ctx, HPy capsule, _HPyCapsule_key key, const char *name)
{
    switch (key)
    {
    case HPyCapsule_key_Pointer:
        return PyCapsule_GetPointer(_h2py(capsule), name);
    case HPyCapsule_key_Name:
        return (void *) PyCapsule_GetName(_h2py(capsule));
    case HPyCapsule_key_Context:
        return PyCapsule_GetContext(_h2py(capsule));
    case HPyCapsule_key_Destructor:
        return ctx_Capsule_GetDestructor(ctx, capsule);
    }
    /* unreachable */
    assert(0);
    return NULL;
}

_HPy_HIDDEN int
ctx_Capsule_Set(HPyContext *ctx, HPy capsule, _HPyCapsule_key key, void *value)
{
    switch (key)
    {
    case HPyCapsule_key_Pointer:
        return PyCapsule_SetPointer(_h2py(capsule), value);
    case HPyCapsule_key_Name:
        return PyCapsule_SetName(_h2py(capsule), (const char *) value);
    case HPyCapsule_key_Context:
        return PyCapsule_SetContext(_h2py(capsule), value);
    case HPyCapsule_key_Destructor:
        return ctx_Capsule_SetDestructor(ctx, capsule, (HPyCapsule_Destructor) value);
    }
    /* unreachable */
    assert(0);
    return -1;
}
#endif
