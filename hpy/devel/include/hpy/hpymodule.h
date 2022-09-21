#ifndef HPY_UNIVERSAL_HPYMODULE_H
#define HPY_UNIVERSAL_HPYMODULE_H
// Copied from Python's exports.h
#ifndef Py_EXPORTED_SYMBOL
    #if defined(_WIN32) || defined(__CYGWIN__)
        #define Py_EXPORTED_SYMBOL __declspec(dllexport)
    #else
        #define Py_EXPORTED_SYMBOL __attribute__ ((visibility ("default")))
    #endif
#endif


// this is defined by HPy_MODINIT
extern HPyContext *_ctx_for_trampolines;

typedef struct {
    const char* name;
    const char* doc;
    HPy_ssize_t size;
    cpy_PyMethodDef *legacy_methods;
    HPyDef **defines;   /* points to an array of 'HPyDef *' */
    /* array with pointers to statically allocated HPyGlobal,
     * with NULL at the end as a sentinel. */
    HPyGlobal **globals;
} HPyModuleDef;


#if defined(__cplusplus)
#  define HPyMODINIT_FUNC extern "C" Py_EXPORTED_SYMBOL HPy
#else /* __cplusplus */
#  define HPyMODINIT_FUNC Py_EXPORTED_SYMBOL HPy
#endif /* __cplusplus */


#define GET_MACRO(exec_or_create_name, exec_name, NAME, ...) NAME
#define HPy_MODINIT(modname, ...) GET_MACRO(__VA_ARGS__, HPy_MODINIT_CREATE_EXEC, HPy_MODINIT_EXEC)(modname, __VA_ARGS__)

#ifdef HPY_UNIVERSAL_ABI

// module initialization in the universal case
#define HPy_MODINIT(modname, exec_or_create_name, ...)         \
    _HPy_HIDDEN HPyContext *_ctx_for_trampolines;              \
    static HPy init_##modname##_impl(HPyContext *ctx);         \
    HPyMODINIT_FUNC                                            \
    HPyInit_##modname(HPyContext *ctx)                         \
    {                                                          \
        _ctx_for_trampolines = ctx;                            \
        return init_##modname##_impl(ctx);                     \
    }

#else // HPY_UNIVERSAL_ABI

// module initialization in the CPython case
#define HPy_MODINIT_EXEC(moddef, exec_name)     \
    PyModuleDef *create_module_def(HPyModuleDef *hpydef, void *create_func, void *exec_func); \
    static _hpy_##modname##_create_func(HPyContext *ctx, HPy *s, HPyModuleDef *d) \
    {                                                                   \
        return _py2h(PyModule_FromDefAndSpec(                           \
                         create_module_def(&moddef),                    \
                         _h2py(s)                                       \
               ));                                                      \
    }                                                                   \
    HPy_MODINIT_CREATE_EXEC(moddef, _hpy_##modname##_create_func, exec_name)

#define HPy_MODINIT_CREATE_EXEC(moddef, create_name, exec_name)         \
    PyModuleDef *create_module_def(HPyModuleDef *hpydef, void *create_func, void *exec_func); \
    static HPy create_name(HPyContext *ctx, HPy spec, HPyModuleDef *def); \
    static int exec_name(HPyContext *ctx, HPy module);                  \
    static PyObject* _hpy_py_##modname##_create_func(PyObject *s, PyModuleDef *d) \
    {                                                                   \
        return _h2py(create_name(_HPyGetContext(), _py2h(s), (HPyModuleDef*)d)); \
    }                                                                   \
    static int _hpy_py_##modname##_exec_func(PyObject *mod)             \
    {                                                                   \
        return exec_name(_HPyGetContext(), _py2h(mod));                 \
    }                                                                   \
    PyMODINIT_FUNC                                                      \
    PyInit_##modname(void)                                              \
    {                                                                   \
        return PyModuleDef_Init(create_module_def(&moddef, _hpy_py_##modname##_create_func, _hpy_py_##modname##_exec_func)); \
    }

#endif // HPY_UNIVERSAL_ABI

#endif // HPY_UNIVERSAL_HPYMODULE_H
