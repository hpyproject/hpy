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

typedef void (*_HPyMethodPairFunc)(HPyCFunction *out_func,
                                   _HPy_CPyCFunction *out_trampoline);

typedef struct {
    const char   *ml_name;   /* The name of the built-in function/method */
    _HPyMethodPairFunc ml_meth;   /* see HPy_FUNCTION() */
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
    HPyContext _ctx_for_trampolines;                           \
    static HPy init_##modname##_impl(HPyContext ctx);          \
    HPy HPyInit_##modname(HPyContext ctx)                      \
    {                                                          \
        _ctx_for_trampolines = ctx;                            \
        return init_##modname##_impl(ctx);                     \
    }

struct _HPyContext_s {
    int version;
    HPy (*module_Create)(HPyContext ctx, HPyModuleDef *def);
    HPy (*none_Get)(HPyContext ctx);
    struct _object *(*callRealFunctionFromTrampoline)(HPyContext ctx,
              struct _object *self, struct _object *args, HPyCFunction func);
    HPy (*fromPyObject)(HPyContext ctx, struct _object *obj);
    struct _object *(*asPyObject)(HPyContext ctx, HPy h);
    HPy (*dup)(HPyContext, HPy h);
    void (*close)(HPyContext, HPy h);
    HPy (*long_FromLong)(HPyContext, long value);
};

extern HPyContext _ctx_for_trampolines;

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

static inline HPy
HPy_FromPyObject(HPyContext ctx, struct _object *obj)
{
    return ctx->fromPyObject(ctx, obj);
}

static inline struct _object *
HPy_AsPyObject(HPyContext ctx, HPy h)
{
    return ctx->asPyObject(ctx, h);
}

static inline HPy
HPy_Dup(HPyContext ctx, HPy h)
{
    return ctx->dup(ctx, h);
}

static inline void
HPy_Close(HPyContext ctx, HPy h)
{
    ctx->close(ctx, h);
}

static inline HPy
HPyLong_FromLong(HPyContext ctx, long value)
{
    return ctx->long_FromLong(ctx, value);
}


#define HPy_FUNCTION(fnname)                                                   \
    static HPy fnname##_impl(HPyContext ctx, HPy self, HPy args);              \
    static struct _object *                                                    \
    fnname##_trampoline(struct _object *self, struct _object *args)            \
    {                                                                          \
        return _ctx_for_trampolines->callRealFunctionFromTrampoline(           \
            _ctx_for_trampolines, self, args, fnname##_impl);                  \
    }                                                                          \
    static void                                                                \
    fnname(HPyCFunction *out_func, _HPy_CPyCFunction *out_trampoline)          \
    {                                                                          \
        *out_func = fnname##_impl;                                             \
        *out_trampoline = fnname##_trampoline;                                 \
    }

#define METH_VARARGS  0x0001
#define METH_KEYWORDS 0x0002
/* METH_NOARGS and METH_O must not be combined with the flags above. */
#define METH_NOARGS   0x0004
#define METH_O        0x0008


#endif /* HPy_UNIVERSAL_H */
