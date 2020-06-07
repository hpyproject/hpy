#ifndef HPY_UNIVERSAL_SLOT_H
#define HPY_UNIVERSAL_SLOT_H

// XXX: HPySlot_unary is essentially the same as HPyMeth_noargs, and
// HPySlot_new the same as HPyMeth_keywords: maybe we should try to armonize
// the differences and merge the two concepts?
typedef HPy (*HPySlot_unary)(HPyContext, HPy a);
typedef HPy (*HPySlot_new)(HPyContext ctx, HPy self,
                           HPy *args, HPy_ssize_t nargs, HPy kw);

// XXX: isn't the name a bit obscure? There might be confusion between HPySlot
// and HPyType_Slot. Maybe we could reuse HPyMeth (ignoring the extra fields),
// but then it's obscure in another way since HPySlot_DEFINE would create an
// object of type HPyMeth.
typedef struct {
    void *impl;            // function pointer to the implementation
    void * cpy_trampoline; // used by CPython to call impl
} HPySlot;


#define HPySlot_DEFINE(SYM, IMPL, T)                                    \
    _HPySlot_DECLARE_IMPL(IMPL, T);                                     \
    HPySlot_TRAMPOLINE(SYM##_trampoline, IMPL, T);                      \
    HPySlot SYM = {                                                     \
        .impl = IMPL,                                                   \
        .cpy_trampoline = SYM##_trampoline                              \
    };

/* trampolines */

#define HPySlot_TRAMPOLINE(SYM, IMPL, T) _HPySlot_TRAMPOLINE_##T(SYM, IMPL)

#define _HPySlot_TRAMPOLINE_HPySlot_unary(NAME, IMPL)                   \
    static cpy_PyObject *                                               \
    NAME(cpy_PyObject *self, cpy_PyObject *noargs)                      \
    {                                                                   \
        return _HPy_CallRealFunctionFromTrampoline(                     \
            _ctx_for_trampolines, self, NULL, NULL, IMPL,               \
            HPyMeth_NOARGS);                                            \
    }

#define _HPySlot_TRAMPOLINE_HPySlot_new(NAME, IMPL)                     \
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

#define _HPySlot_DECLARE_IMPL(IMPL, T)     _HPySlot_DECLARE_IMPL_##T(IMPL)

#define _HPySlot_DECLARE_IMPL_HPySlot_unary(IMPL)  \
    static HPy IMPL(HPyContext ctx, HPy a)

#define _HPySlot_DECLARE_IMPL_HPySlot_new(IMPL) \
    static HPy IMPL(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs, HPy kw);


#endif /* HPY_UNIVERSAL_SLOT_H */
