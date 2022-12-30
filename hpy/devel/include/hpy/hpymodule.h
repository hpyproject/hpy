#ifndef HPY_UNIVERSAL_HPYMODULE_H
#define HPY_UNIVERSAL_HPYMODULE_H

/* If 'HPY_EMBEDDED_MODULES' is defined, this means that there will be several
   embedded HPy modules (and so, several 'HPy_MODINIT' usages) in the same
   binary. In this case, some restrictions apply: (1) all of the module's
   methods/member/slots/... must be defined in the same file, and (2) the
   embedder *MUST* declare the module to be "embeddable" by using macro
   'HPY_MOD_EMBEDDABLE(modname)'. */
#ifdef HPY_EMBEDDED_MODULES
#define _HPy_CTX_MODIFIER static
#define HPY_MOD_EMBEDDABLE(modname) \
    _HPy_CTX_MODIFIER HPyContext *_ctx_for_trampolines;
#else
#define _HPy_CTX_MODIFIER _HPy_HIDDEN
/**
 * Declares a module to be *embeddable* which means that it and its members can
 * be compiler/linked into a binary together with other embeddable HPy modules.
 *
 * You may declare a module to be *embeddable* if all of its member definitions
 * are in the same file.
 */
#define HPY_MOD_EMBEDDABLE(modname)
// this is defined by HPy_MODINIT
extern HPyContext *_ctx_for_trampolines;
#endif


typedef struct {
    /** The Python name of module (UTF-8 encoded) */
    const char* name;

    /** Docstring of the type (UTF-8 encoded; may be ``NULL``) */
    const char* doc;

    /** The size (in bytes) of the module state structure. */
    HPy_ssize_t size;

    /**
     * ``NULL``-terminated list of legacy module-level methods.
     * In order to enable incremental migration
     * from C API to HPy, it is possible to still add *legacy* method
     * definitions. Those methods have a C API signature which means that they
     * still receive ``PyObject *`` and similar arguments. If legacy methods
     * are defined, you cannot create a *universal binary* (i.e. a binary that
     * will run on all Python engines).
     */
    cpy_PyMethodDef *legacy_methods;

    /**
     * Pointer to a ``NULL``-terminated array of pointers to HPy defines (i.e.
     * ``HPyDef *``). Note, that some kinds of HPy definitions don't make sense
     * for a module. In particular, anything else than methods.
     */
    HPyDef **defines;

    /**
     * Pointer to a ``NULL``-terminated array of pointers to
     * :c:struct:`HPyGlobal` variables. For details, see :doc:`hpy-global`.
     */
    HPyGlobal **globals;
} HPyModuleDef;


#if defined(__cplusplus)
#  define HPyVERSION_FUNC extern "C" HPy_EXPORTED_SYMBOL uint32_t
#  define HPyMODINIT_FUNC extern "C" HPy_EXPORTED_SYMBOL HPy
#else /* __cplusplus */
#  define HPyVERSION_FUNC HPy_EXPORTED_SYMBOL uint32_t
#  define HPyMODINIT_FUNC HPy_EXPORTED_SYMBOL HPy
#endif /* __cplusplus */

#ifdef HPY_ABI_CPYTHON

// module initialization in the CPython case
#define HPy_MODINIT(modname)                                      \
    static HPy init_##modname##_impl(HPyContext *ctx);            \
    PyMODINIT_FUNC                                                \
    PyInit_##modname(void)                                        \
    {                                                             \
        return _h2py(init_##modname##_impl(_HPyGetContext()));    \
    }

#else // HPY_ABI_CPYTHON

// module initialization in the universal and hybrid case

/**
 * Convenience macro for generating the module initialization code. This will
 * generate three functions that are used by to verify an initialize the module
 * when loading:
 *
 * ``get_required_hpy_major_version_<modname>``
 *   The HPy major version this module was built with.
 *
 * ``get_required_hpy_minor_version_<modname>``
 *   The HPy minor version this module was built with.
 *
 * ``HPyInit_<modname>``
 *   The init function that will be called by the interpreter.
 *
 * The macro expects that there is a function ``init_<modname>`` that does
 * the actual initialization of the module.
 *
 * Example:
 *
 * .. code-block:: c
 *
 *   HPy_MODINIT(mymodule)
 *   static HPy init_mymodule(HPyContext *ctx)
 *   {
 *     // ...
 *   }
 */
#define HPy_MODINIT(modname)                                      \
    HPyVERSION_FUNC                                               \
    get_required_hpy_major_version_##modname()                    \
    {                                                             \
        return HPY_ABI_VERSION;                                   \
    }                                                             \
    HPyVERSION_FUNC                                               \
    get_required_hpy_minor_version_##modname()                    \
    {                                                             \
        return HPY_ABI_VERSION_MINOR;                             \
    }                                                             \
    _HPy_CTX_MODIFIER HPyContext *_ctx_for_trampolines;           \
    static HPy init_##modname##_impl(HPyContext *ctx);            \
    HPyMODINIT_FUNC                                               \
    HPyInit_##modname(HPyContext *ctx)                            \
    {                                                             \
        _ctx_for_trampolines = ctx;                               \
        return init_##modname##_impl(ctx);                        \
    }

#endif // HPY_ABI_CPYTHON

#endif // HPY_UNIVERSAL_HPYMODULE_H
