#ifndef HPy_UNIVERSAL_H
#define HPy_UNIVERSAL_H

#include <stdlib.h>
#include <stdint.h>

typedef intptr_t HPy_ssize_t;
typedef struct { void *_o; } HPy;

typedef struct _HPyContext_s *HPyContext;
typedef HPy (*HPyCFunction)(HPyContext, HPy self, HPy args);
struct _object;  /* that's PyObject inside CPython */
typedef struct _object *(*_HPy_CPyCFunction)(struct _object *self,
                                             struct _object *args);

#define HPy_NULL ((HPy){NULL})
#define HPy_IsNull(x) ((x)._o == NULL)

struct _HPyMethodPair_s
{
    _HPy_CPyCFunction trampoline;
    HPyCFunction func;
};

typedef struct {
    const char   *ml_name;   /* The name of the built-in function/method */
    const struct _HPyMethodPair_s *ml_meth;   /* see HPy_FUNCTION() */
    int          ml_flags;   /* Combination of METH_xxx flags, which mostly
                                describe the args expected by the C func */
    const char   *ml_doc;    /* The __doc__ attribute, or NULL */
} HPyMethodDef;


#define HPyModuleDef_HEAD_INIT NULL

typedef struct {
    void *dummy; // this is needed because we put a comma after HPyModuleDef_HEAD_INIT :(
    const char* m_name;
    const char* m_doc;
    HPy_ssize_t m_size;
    HPyMethodDef *m_methods;
} HPyModuleDef;

#define HPy_MODINIT(modname)                                   \
    static HPy init_##modname##_impl(HPyContext ctx);          \
    HPy HPyInit_##modname(HPyContext ctx)                      \
    {                                                          \
        return init_##modname##_impl(ctx);                     \
    }

struct _HPyContext_s {
    int version;
    HPy (*module_Create)(HPyContext ctx, HPyModuleDef *def);
    HPy (*none_Get)(HPyContext ctx);
};

static inline HPy
HPyModule_Create(HPyContext ctx, HPyModuleDef *def)
{
    // XXX: think about versioning
    return ctx->module_Create(ctx, def);
}

static inline HPy
HPyNone_Get(HPyContext ctx)
{
    return ctx->none_Get(ctx);
}

struct _object *
_HPy_CallRealFunctionFromTrampoline(struct _object *self, struct _object *args,
                                    HPyCFunction func);

#define HPy_FUNCTION(fnname)                                                   \
    static HPy fnname##_impl(HPyContext ctx, HPy self, HPy args);              \
    static struct _object *                                                    \
    fnname##_trampoline(struct _object *self, struct _object *args)            \
    {                                                                          \
        return _HPy_CallRealFunctionFromTrampoline(self, args, fnname##_impl); \
    }                                                                          \
    static const struct _HPyMethodPair_s fnname##_struct = {                   \
        .trampoline = fnname##_trampoline,                                     \
        .func = fnname##_impl                                                  \
    };                                                                         \
    static const struct _HPyMethodPair_s *const fnname = &fnname##_struct;

#define METH_VARARGS  0x0001
#define METH_KEYWORDS 0x0002
/* METH_NOARGS and METH_O must not be combined with the flags above. */
#define METH_NOARGS   0x0004
#define METH_O        0x0008


#endif /* HPy_UNIVERSAL_H */
