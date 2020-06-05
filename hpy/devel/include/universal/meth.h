#ifndef HPY_UNIVERSAL_METH_H
#define HPY_UNIVERSAL_METH_H

/* the values of this enum are compatible with the corresponding CPython's
   METH_*. In particular, the value of HPyMeth.signature is equal to the value
   of the corresponding PyMethodDef.ml_flags
*/
typedef enum {
    HPyMeth_VARARGS  = 0x0001,  // METH_VARARGS
    HPyMeth_KEYWORDS = 0x0003,  // METH_VARARGS | METH_KEYWORDS
    HPyMeth_NOARGS   = 0x0004,  // METH_NOARGS
    HPyMeth_O        = 0x0008   // METH_O
} HPyMeth_Signature;

typedef HPy (*HPyMeth_noargs)(HPyContext, HPy self);
typedef HPy (*HPyMeth_o)(HPyContext ctx, HPy self, HPy arg);
typedef HPy (*HPyMeth_varargs)(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs);

typedef struct {
    const char *name;             // The name of the built-in function/method
    void *impl;                   // function pointer to the implementation
    void *cpython_trampoline;     // CPython-only trampoline which calls impl()
    HPyMeth_Signature signature;  // Specify the signature/calling convention of impl
    const char *doc;              // The __doc__ attribute, or NULL
} HPyMeth;


/* macros to create CPython trampolines for each signature */

#define HPyMeth_TRAMPOLINE(SYM, IMPL, SIG) _HPyMeth_TRAMPOLINE_##SIG(SYM, IMPL)

#define _HPyMeth_TRAMPOLINE_HPyMeth_NOARGS(NAME, IMPL)                  \
    static struct _object *                                             \
    NAME(struct _object *self, struct _object *noargs)                  \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, NULL, NULL, IMPL,               \
            HPyMeth_NOARGS);                                            \
    }

#define _HPyMeth_TRAMPOLINE_HPyMeth_O(NAME, IMPL)                       \
    static struct _object *                                             \
    NAME(struct _object *self, struct _object *arg)                     \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, arg, NULL, IMPL,                \
            HPyMeth_O);                                                 \
    }

#define _HPyMeth_TRAMPOLINE_HPyMeth_VARARGS(NAME, IMPL)                 \
    static struct _object *                                             \
    NAME(struct _object *self, struct _object *args)                    \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, args, NULL, IMPL,               \
            HPyMeth_VARARGS);                                           \
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


/* Macro to define an HPyMeth:
 *     - declare the expected prototype for impl
 *     - create the corresponding CPython trampoline
 *     - define HPyMeth and fill all the fields
 */

#define HPyMeth_DEFINE(SYM, NAME, IMPL, SIG)                            \
    _HPyMeth_DECLARE_IMPL(IMPL, SIG);                                   \
    HPyMeth_TRAMPOLINE(SYM##_trampoline, IMPL, SIG);                    \
    HPyMeth SYM = {                                                     \
        .name = NAME,                                                   \
        .impl = IMPL,                                                   \
        .cpython_trampoline = SYM##_trampoline,                         \
        .signature = SIG                                                \
    };



/*

#define HPy_DEF_METH_KEYWORDS(fnname)                                          \
    static HPy fnname##_impl(HPyContext ctx, HPy self,                         \
                             HPy *args, HPy_ssize_t nargs, HPy kw);            \
    static struct _object *                                                    \
    fnname##_trampoline(struct _object *self, struct _object *args,            \
                        struct _object *kw)                                    \
    {                                                                          \
        return _HPy_CallRealFunctionFromTrampoline(                            \
            _ctx_for_trampolines, self, args, kw, fnname##_impl,               \
            HPy_METH_KEYWORDS);                                                \
    }                                                                          \
    void                                                                       \
    fnname(void **out_func, _HPy_CPyCFunction *out_trampoline)                 \
    {                                                                          \
        *out_func = fnname##_impl;                                             \
        *out_trampoline = (_HPy_CPyCFunction) fnname##_trampoline;             \
    }

*/

#endif // HPY_UNIVERSAL_METH_H
