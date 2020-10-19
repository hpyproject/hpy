#ifndef HPY_COMMON_RUNTIME_CTX_TRACKER_H
#define HPY_COMMON_RUNTIME_CTX_TRACKER_H

#include "hpy.h"

typedef struct _HPyTracker_s * HPyTracker;

_HPy_HIDDEN HPyTracker
ctx_HPyTracker_New(HPyContext ctx);

_HPy_HIDDEN HPyTracker
ctx_HPyTracker_NewWithSize(HPyContext ctx, HPy_ssize_t size);

_HPy_HIDDEN int
ctx_HPyTracker_Add(HPyContext ctx, HPyTracker hl, HPy h);

_HPy_HIDDEN int
ctx_HPyTracker_RemoveAll(HPyContext ctx, HPyTracker hl);

_HPy_HIDDEN int
ctx_HPyTracker_Free(HPyContext ctx, HPyTracker hl);

#endif /* HPY_COMMON_RUNTIME_CTX_TRACKER_H */
