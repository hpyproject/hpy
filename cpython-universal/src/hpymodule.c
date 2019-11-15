#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <dlfcn.h>
#include <stdio.h>

#include "api.h"

typedef HPy (*InitFuncPtr)(HPyContext ctx);

static PyObject *set_debug(PyObject *self, PyObject *args)
{
    // TODO
    Py_RETURN_NONE;
}

static PyObject *load(PyObject *self, PyObject *soname_obj)
{
    PyObject *string_obj = NULL;
    const char *soname;
    if (!PyUnicode_FSConverter(soname_obj, &string_obj)) {
        return NULL;
    }
    soname = PyBytes_AS_STRING(string_obj);

    printf("loading %s\n", soname);
    void *mylib = dlopen(soname, RTLD_NOW); // how closes this?
    if (mylib == NULL) {
        const char *error = dlerror();
        if (error == NULL)
            error = "unknown dlopen() error";
        PyErr_Format(PyExc_RuntimeError, "dlopen: %s", error);
        return NULL;
    }

    void *initfn = dlsym(mylib, "HPyInit_mytest");
    if (initfn == NULL) {
        const char *error = dlerror();
        if (error == NULL)
            error = "unknown dlsym() error";
        PyErr_Format(PyExc_RuntimeError, "dlsym: %s", error);
        return NULL;
    }

    HPy mod = ((InitFuncPtr)initfn)(&global_ctx);
    PyObject *py_mod = _h2py(mod);
    // XXX close the handle

    Py_CLEAR(string_obj);
    return py_mod;
}


static PyMethodDef HPyMethods[] = {
    {"set_debug", (PyCFunction)set_debug, METH_O, "TODO"},
    {"load", (PyCFunction)load, METH_O, "Load a .hpy.so"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef hpydef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "hpy_universal",
    .m_doc = "HPy universal runtime for CPython",
    .m_size = 0,
    .m_methods = HPyMethods,
};


PyMODINIT_FUNC
PyInit_hpy_universal(void)
{
    return PyModuleDef_Init(&hpydef);
}
