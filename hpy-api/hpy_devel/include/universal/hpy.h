#ifndef HPy_UNIVERSAL_H
#define HPy_UNIVERSAL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

typedef intptr_t HPy_ssize_t;
typedef struct { HPy_ssize_t _i; } HPy;

typedef struct _HPyContext_s *HPyContext;
struct _object;  /* that's PyObject inside CPython */
typedef struct _object *(*_HPy_CPyCFunction)(struct _object *self,
                                             struct _object *args);

#define HPy_NULL ((HPy){0})
#define HPy_IsNull(x) ((x)._i == 0)

#include "meth.h"

#define HPyModuleDef_HEAD_INIT NULL

typedef struct {
    void *dummy; // this is needed because we put a comma after HPyModuleDef_HEAD_INIT :(
    const char* m_name;
    const char* m_doc;
    HPy_ssize_t m_size;
    HPyMethodDef *m_methods;
} HPyModuleDef;

#define _HPy_HIDDEN   __attribute__((visibility("hidden")))
#define HPy_MODINIT(modname)                                   \
    _HPy_HIDDEN HPyContext _ctx_for_trampolines;               \
    static HPy init_##modname##_impl(HPyContext ctx);          \
    HPy HPyInit_##modname(HPyContext ctx)                      \
    {                                                          \
        _ctx_for_trampolines = ctx;                            \
        return init_##modname##_impl(ctx);                     \
    }

#include "autogen_ctx.h"
#include "autogen_func.h"

extern HPyContext _ctx_for_trampolines;



#endif /* HPy_UNIVERSAL_H */
