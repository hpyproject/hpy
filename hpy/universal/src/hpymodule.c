#define PY_SSIZE_T_CLEAN
#include <Python.h>
#ifdef MS_WIN32
# include <windows.h>
# include "misc_win32.h"
#else
# include <dlfcn.h>
#endif
#include <stdio.h>

#include "api.h"
#include "handles.h"
#include "common/version.h"

typedef HPy (*InitFuncPtr)(HPyContext ctx);

static PyObject *set_debug(PyObject *self, PyObject *args)
{
    // TODO
    Py_RETURN_NONE;
}

static const char *prefix = "HPyInit";

static PyObject *
get_encoded_name(PyObject *name) {
    PyObject *tmp;
    PyObject *encoded = NULL;
    PyObject *modname = NULL;
    Py_ssize_t name_len, lastdot;

    /* Get the short name (substring after last dot) */
    name_len = PyUnicode_GetLength(name);
    lastdot = PyUnicode_FindChar(name, '.', 0, name_len, -1);
    if (lastdot < -1) {
        return NULL;
    } else if (lastdot >= 0) {
        tmp = PyUnicode_Substring(name, lastdot + 1, name_len);
        if (tmp == NULL)
            return NULL;
        name = tmp;
        /* "name" now holds a new reference to the substring */
    } else {
        Py_INCREF(name);
    }

    /* Encode to ASCII */
    encoded = PyUnicode_AsEncodedString(name, "ascii", NULL);
    if (encoded != NULL) {
    } else {
        goto error;
    }

    /* Replace '-' by '_' */
    modname = PyObject_CallMethod(encoded, "replace", "cc", '-', '_');
    if (modname == NULL)
        goto error;

    Py_DECREF(name);
    Py_DECREF(encoded);
    return modname;
error:
    Py_DECREF(name);
    Py_XDECREF(encoded);
    return NULL;
}


static PyObject *load_from_spec(PyObject *self, PyObject *spec)
{
    PyObject *name_unicode, *name;
    PyObject *path = NULL;
    PyObject *pathbytes = NULL;

    name_unicode = PyObject_GetAttrString(spec, "name");
    if (name_unicode == NULL) {
        return NULL;
    }
    name = get_encoded_name(name_unicode);
    if (name == NULL) {
        goto error;
    }
    const char *shortname = PyBytes_AS_STRING(name);
    char init_name[258];
    PyOS_snprintf(init_name, sizeof(init_name), "%.20s_%.200s",
            prefix, shortname);

    path = PyObject_GetAttrString(spec, "origin");
    if (path == NULL)
        goto error;
    pathbytes = PyUnicode_EncodeFSDefault(path);
    if (pathbytes == NULL)
        goto error;
    const char *soname = PyBytes_AS_STRING(pathbytes);

    void *mylib = dlopen(soname, RTLD_NOW); // who closes this?
    if (mylib == NULL) {
        const char *error = dlerror();
        if (error == NULL)
            error = "unknown dlopen() error";
        PyErr_Format(PyExc_RuntimeError, "dlopen: %s", error);
        goto error;
    }

    void *initfn = dlsym(mylib, init_name);
    if (initfn == NULL) {
        const char *error = dlerror();
        if (error == NULL)
            error = "unknown dlsym() error";
        PyErr_Format(PyExc_RuntimeError, "dlsym: %s", error);
        goto error;
    }

    HPy mod = ((InitFuncPtr)initfn)(&global_ctx);
    if (HPy_IsNull(mod))
        goto error;
    PyObject *py_mod = _h2py(mod);
    // XXX close the handle

    Py_DECREF(name_unicode);
    Py_XDECREF(name);
    Py_XDECREF(path);
    Py_XDECREF(pathbytes);
    return py_mod;
error:
    Py_DECREF(name_unicode);
    Py_XDECREF(name);
    Py_XDECREF(path);
    Py_XDECREF(pathbytes);
    return NULL;
}

static PyObject *get_version(PyObject *self, PyObject *ignored)
{
    return Py_BuildValue("ss", HPY_VERSION, HPY_GIT_REVISION);
}

static PyMethodDef HPyMethods[] = {
    {"set_debug", (PyCFunction)set_debug, METH_O, "TODO"},
    {"load_from_spec", (PyCFunction)load_from_spec, METH_O, "Load a .hpy.so"},
    {"get_version", (PyCFunction)get_version, METH_NOARGS, "Return a tuple ('version', 'git revision')"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef hpydef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "hpy.universal",
    .m_doc = "HPy universal runtime for CPython",
    .m_size = 0,
    .m_methods = HPyMethods,
};


PyMODINIT_FUNC
PyInit_universal(void)
{
    return PyModuleDef_Init(&hpydef);
}
