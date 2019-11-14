#ifndef HPy_UNIVERSAL_H
#define HPy_UNIVERSAL_H

#include <stdlib.h>
#include <stdint.h>

typedef intptr_t HPy_ssize_t;
typedef struct { void *_o; } HPy;

typedef struct _HPyContext_s *HPyContext;
typedef HPy (*HPyCFunction)(HPyContext, HPy self, HPy args);

#define HPy_NULL ((HPy){NULL})
#define HPy_IsNull(x) ((x)._o == NULL)

typedef struct {
    const char   *ml_name;   /* The name of the built-in function/method */
    void         *ml_meth;   /* The C function that implements it */
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
};

static inline HPy
HPyModule_Create(HPyContext ctx, HPyModuleDef *def)
{
    // XXX: think about versioning
    return ctx->module_Create(ctx, def);
}


#endif /* HPy_UNIVERSAL_H */
