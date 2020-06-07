#ifndef HPY_UNIVERSAL_METH_H
#define HPY_UNIVERSAL_METH_H

typedef enum {
    HPyMeth_VARARGS  = 1,  // METH_VARARGS
    HPyMeth_KEYWORDS = 2,  // METH_VARARGS | METH_KEYWORDS
    HPyMeth_NOARGS   = 3,  // METH_NOARGS
    HPyMeth_O        = 4   // METH_O
} HPyMeth_Signature;

typedef HPy (*HPyMeth_noargs)(HPyContext, HPy self);
typedef HPy (*HPyMeth_o)(HPyContext ctx, HPy self, HPy arg);
typedef HPy (*HPyMeth_varargs)(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs);
typedef HPy (*HPyMeth_keywords)(HPyContext ctx, HPy self,
                                HPy *args, HPy_ssize_t nargs, HPy kw);


typedef struct {
    int slot;
    void *impl;                     // function pointer to the implementation
    void *cpy_trampoline;           // used by CPython to call impl
    HPyMeth_Signature signature;    // Indicates impl's expected the signature

    // the following fields are used only if slot == HPy_meth
    const char *name;               // The name of the built-in function/method
    const char *doc;                // The __doc__ attribute, or NULL
} HPyMeth;


/* macros to create CPython trampolines for each signature */

#define HPyMeth_TRAMPOLINE(SYM, IMPL, SIG) _HPyMeth_TRAMPOLINE_##SIG(SYM, IMPL)

#define _HPyMeth_TRAMPOLINE_HPyMeth_NOARGS(NAME, IMPL)                  \
    static cpy_PyObject *                                               \
    NAME(cpy_PyObject *self, cpy_PyObject *noargs)                      \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, NULL, NULL, IMPL,               \
            HPyMeth_NOARGS);                                            \
    }

#define _HPyMeth_TRAMPOLINE_HPyMeth_O(NAME, IMPL)                       \
    static cpy_PyObject *                                               \
    NAME(cpy_PyObject *self, cpy_PyObject *arg)                         \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, arg, NULL, IMPL,                \
            HPyMeth_O);                                                 \
    }

#define _HPyMeth_TRAMPOLINE_HPyMeth_VARARGS(NAME, IMPL)                 \
    static cpy_PyObject *                                               \
    NAME(cpy_PyObject *self, cpy_PyObject *args)                        \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, args, NULL, IMPL,               \
            HPyMeth_VARARGS);                                           \
    }

#define _HPyMeth_TRAMPOLINE_HPyMeth_KEYWORDS(NAME, IMPL)                \
    static cpy_PyObject *                                               \
    NAME(cpy_PyObject *self, cpy_PyObject *args, cpy_PyObject *kw)      \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, args, kw, IMPL,                 \
            HPyMeth_KEYWORDS);                                          \
    }


/* macros to declare the prototype of "impl" depending on the signature. This
 * way, if we use the wrong signature, we get a nice compiler error.
 */

#define _HPyMeth_DECLARE_IMPL(IMPL, SIG)     _HPyMeth_DECLARE_IMPL_##SIG(IMPL)

#define _HPyMeth_DECLARE_IMPL_HPyMeth_NOARGS(IMPL)  \
    static HPy IMPL(HPyContext ctx, HPy self)

#define _HPyMeth_DECLARE_IMPL_HPyMeth_O(IMPL) \
    static HPy IMPL(HPyContext ctx, HPy self, HPy arg)

#define _HPyMeth_DECLARE_IMPL_HPyMeth_VARARGS(IMPL) \
    static HPy IMPL(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs);

#define _HPyMeth_DECLARE_IMPL_HPyMeth_KEYWORDS(IMPL) \
    static HPy IMPL(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs, HPy kw);


/* Macro to define an HPyMeth:
 *     - declare the expected prototype for impl
 *     - create the corresponding CPython trampoline
 *     - define HPyMeth and fill all the fields
 */

#define HPyMeth_DEFINE(SYM, NAME, IMPL, SIG)                            \
    _HPyMeth_DECLARE_IMPL(IMPL, SIG);                                   \
    HPyMeth_TRAMPOLINE(SYM##_trampoline, IMPL, SIG);                    \
    HPyMeth SYM = {                                                     \
        .slot = HPy_meth,                                               \
        .impl = IMPL,                                                   \
        .cpy_trampoline = SYM##_trampoline,                             \
        .signature = SIG,                                               \
        .name = NAME                                                    \
    };

#endif // HPY_UNIVERSAL_METH_H
