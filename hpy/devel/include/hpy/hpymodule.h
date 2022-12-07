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
#define HPY_MOD_EMBEDDABLE(modname)
// this is defined by HPy_MODINIT
extern HPyContext *_ctx_for_trampolines;
#endif


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
