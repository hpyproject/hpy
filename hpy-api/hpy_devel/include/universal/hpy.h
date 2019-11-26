#ifndef HPY_UNIVERSAL_H
#define HPY_UNIVERSAL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

typedef intptr_t HPy_ssize_t;
typedef struct { HPy_ssize_t _i; } HPy;

typedef struct _HPyContext_s *HPyContext;
struct _object;  /* that's PyObject inside CPython */
typedef struct _object *(*_HPy_CPyCFunction)(struct _object *self,
                                             struct _object *args);

#define _HPy_HIDDEN   __attribute__((visibility("hidden")))
#define HPy_NULL ((HPy){0})
#define HPy_IsNull(x) ((x)._i == 0)

#include "meth.h"
#include "module.h"

#include "autogen_ctx.h"
#include "autogen_trampolines.h"

#endif /* HPY_UNIVERSAL_H */
