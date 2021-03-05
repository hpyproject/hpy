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
#include "hpy_debug.h"

#ifdef PYPY_VERSION
#  error "Cannot build hpy.univeral on top of PyPy. PyPy comes with its own version of it"
#endif

typedef HPy (*InitFuncPtr)(HPyContext *ctx);

static PyObject *set_debug(PyObject *self, PyObject *args)
{
    // TODO
    Py_RETURN_NONE;
}

static const char *prefix = "HPyInit";

static HPyContext * get_context(int debug)
{
    if (debug)
        return hpy_debug_get_ctx(&g_universal_ctx);
    else
        return &g_universal_ctx;
}

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

static PyObject *do_load(PyObject *name_unicode, PyObject *path, int debug)
{
    PyObject *name = NULL;
    PyObject *pathbytes = NULL;

    name = get_encoded_name(name_unicode);
    if (name == NULL) {
        goto error;
    }
    const char *shortname = PyBytes_AS_STRING(name);
    char init_name[258];
    PyOS_snprintf(init_name, sizeof(init_name), "%.20s_%.200s",
            prefix, shortname);

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

    HPyContext *ctx = get_context(debug);
    if (ctx == NULL)
        goto error;
    HPy h_mod = ((InitFuncPtr)initfn)(ctx);
    if (HPy_IsNull(h_mod))
        goto error;
    PyObject *py_mod = HPy_AsPyObject(ctx, h_mod);
    HPy_Close(ctx, h_mod);

    Py_XDECREF(name);
    Py_XDECREF(pathbytes);
    return py_mod;
error:
    Py_XDECREF(name);
    Py_XDECREF(pathbytes);
    return NULL;
}

static PyObject *load(PyObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"name", "path", "debug", NULL};
    PyObject *name_unicode;
    PyObject *path;
    int debug = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|p", kwlist,
                                     &name_unicode, &path, &debug)) {
        return NULL;
    }
    return do_load(name_unicode, path, debug);
}

static PyObject *get_version(PyObject *self, PyObject *ignored)
{
    return Py_BuildValue("ss", HPY_VERSION, HPY_GIT_REVISION);
}

static PyMethodDef HPyMethods[] = {
    {"set_debug", (PyCFunction)set_debug, METH_O, "TODO"},
    {"load", (PyCFunction)load, METH_VARARGS | METH_KEYWORDS, "Load a .hpy.so"},
    {"get_version", (PyCFunction)get_version, METH_NOARGS, "Return a tuple ('version', 'git revision')"},
    {NULL, NULL, 0, NULL}
};

static int exec_module(PyObject *mod);
static PyModuleDef_Slot hpymodule_slots[] = {
    {Py_mod_exec, exec_module},
    {0, NULL},
};

static struct PyModuleDef hpydef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "hpy.universal",
    .m_doc = "HPy universal runtime for CPython",
    .m_size = 0,
    .m_methods = HPyMethods,
    .m_slots = hpymodule_slots,
};


// module initialization function
int exec_module(PyObject* mod) {
    HPyContext *ctx  = &g_universal_ctx;
    HPy h_debug_mod = HPyInit__debug(ctx);
    if (HPy_IsNull(h_debug_mod))
        return -1;
    PyObject *_debug_mod = HPy_AsPyObject(ctx, h_debug_mod);
    HPy_Close(ctx, h_debug_mod);

    if (PyModule_AddObject(mod, "_debug", _debug_mod) < 0)
        return -1;

    return 0;
}

static void init_universal_ctx(HPyContext *ctx)
{
    if (!HPy_IsNull(ctx->h_None))
        // already initialized
        return;

    // XXX this code is basically the same as found in cpython/hpy.h. We
    // should probably share and/or autogenerate both versions
    /* Constants */
    ctx->h_None = _py2h(Py_None);
    ctx->h_True = _py2h(Py_True);
    ctx->h_False = _py2h(Py_False);
    ctx->h_NotImplemented = _py2h(Py_NotImplemented);
    ctx->h_Ellipsis = _py2h(Py_Ellipsis);
    /* Exceptions */
    ctx->h_BaseException = _py2h(PyExc_BaseException);
    ctx->h_Exception = _py2h(PyExc_Exception);
    ctx->h_StopAsyncIteration = _py2h(PyExc_StopAsyncIteration);
    ctx->h_StopIteration = _py2h(PyExc_StopIteration);
    ctx->h_GeneratorExit = _py2h(PyExc_GeneratorExit);
    ctx->h_ArithmeticError = _py2h(PyExc_ArithmeticError);
    ctx->h_LookupError = _py2h(PyExc_LookupError);
    ctx->h_AssertionError = _py2h(PyExc_AssertionError);
    ctx->h_AttributeError = _py2h(PyExc_AttributeError);
    ctx->h_BufferError = _py2h(PyExc_BufferError);
    ctx->h_EOFError = _py2h(PyExc_EOFError);
    ctx->h_FloatingPointError = _py2h(PyExc_FloatingPointError);
    ctx->h_OSError = _py2h(PyExc_OSError);
    ctx->h_ImportError = _py2h(PyExc_ImportError);
    ctx->h_ModuleNotFoundError = _py2h(PyExc_ModuleNotFoundError);
    ctx->h_IndexError = _py2h(PyExc_IndexError);
    ctx->h_KeyError = _py2h(PyExc_KeyError);
    ctx->h_KeyboardInterrupt = _py2h(PyExc_KeyboardInterrupt);
    ctx->h_MemoryError = _py2h(PyExc_MemoryError);
    ctx->h_NameError = _py2h(PyExc_NameError);
    ctx->h_OverflowError = _py2h(PyExc_OverflowError);
    ctx->h_RuntimeError = _py2h(PyExc_RuntimeError);
    ctx->h_RecursionError = _py2h(PyExc_RecursionError);
    ctx->h_NotImplementedError = _py2h(PyExc_NotImplementedError);
    ctx->h_SyntaxError = _py2h(PyExc_SyntaxError);
    ctx->h_IndentationError = _py2h(PyExc_IndentationError);
    ctx->h_TabError = _py2h(PyExc_TabError);
    ctx->h_ReferenceError = _py2h(PyExc_ReferenceError);
    ctx->h_SystemError = _py2h(PyExc_SystemError);
    ctx->h_SystemExit = _py2h(PyExc_SystemExit);
    ctx->h_TypeError = _py2h(PyExc_TypeError);
    ctx->h_UnboundLocalError = _py2h(PyExc_UnboundLocalError);
    ctx->h_UnicodeError = _py2h(PyExc_UnicodeError);
    ctx->h_UnicodeEncodeError = _py2h(PyExc_UnicodeEncodeError);
    ctx->h_UnicodeDecodeError = _py2h(PyExc_UnicodeDecodeError);
    ctx->h_UnicodeTranslateError = _py2h(PyExc_UnicodeTranslateError);
    ctx->h_ValueError = _py2h(PyExc_ValueError);
    ctx->h_ZeroDivisionError = _py2h(PyExc_ZeroDivisionError);
    ctx->h_BlockingIOError = _py2h(PyExc_BlockingIOError);
    ctx->h_BrokenPipeError = _py2h(PyExc_BrokenPipeError);
    ctx->h_ChildProcessError = _py2h(PyExc_ChildProcessError);
    ctx->h_ConnectionError = _py2h(PyExc_ConnectionError);
    ctx->h_ConnectionAbortedError = _py2h(PyExc_ConnectionAbortedError);
    ctx->h_ConnectionRefusedError = _py2h(PyExc_ConnectionRefusedError);
    ctx->h_ConnectionResetError = _py2h(PyExc_ConnectionResetError);
    ctx->h_FileExistsError = _py2h(PyExc_FileExistsError);
    ctx->h_FileNotFoundError = _py2h(PyExc_FileNotFoundError);
    ctx->h_InterruptedError = _py2h(PyExc_InterruptedError);
    ctx->h_IsADirectoryError = _py2h(PyExc_IsADirectoryError);
    ctx->h_NotADirectoryError = _py2h(PyExc_NotADirectoryError);
    ctx->h_PermissionError = _py2h(PyExc_PermissionError);
    ctx->h_ProcessLookupError = _py2h(PyExc_ProcessLookupError);
    ctx->h_TimeoutError = _py2h(PyExc_TimeoutError);
    /* Warnings */
    ctx->h_Warning = _py2h(PyExc_Warning);
    ctx->h_UserWarning = _py2h(PyExc_UserWarning);
    ctx->h_DeprecationWarning = _py2h(PyExc_DeprecationWarning);
    ctx->h_PendingDeprecationWarning = _py2h(PyExc_PendingDeprecationWarning);
    ctx->h_SyntaxWarning = _py2h(PyExc_SyntaxWarning);
    ctx->h_RuntimeWarning = _py2h(PyExc_RuntimeWarning);
    ctx->h_FutureWarning = _py2h(PyExc_FutureWarning);
    ctx->h_ImportWarning = _py2h(PyExc_ImportWarning);
    ctx->h_UnicodeWarning = _py2h(PyExc_UnicodeWarning);
    ctx->h_BytesWarning = _py2h(PyExc_BytesWarning);
    ctx->h_ResourceWarning = _py2h(PyExc_ResourceWarning);
    /* Types */
    ctx->h_BaseObjectType = _py2h((PyObject *)&PyBaseObject_Type);
    ctx->h_TypeType = _py2h((PyObject *)&PyType_Type);
    ctx->h_LongType = _py2h((PyObject *)&PyLong_Type);
    ctx->h_UnicodeType = _py2h((PyObject *)&PyUnicode_Type);
    ctx->h_TupleType = _py2h((PyObject *)&PyTuple_Type);
    ctx->h_ListType = _py2h((PyObject *)&PyList_Type);
}


PyMODINIT_FUNC
PyInit_universal(void)
{
    init_universal_ctx(&g_universal_ctx);
    PyObject *mod = PyModuleDef_Init(&hpydef);
    return mod;
}
