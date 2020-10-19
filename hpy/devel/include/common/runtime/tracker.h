#ifndef HPY_COMMON_RUNTIME_HPYTRACKER_H
#define HPY_COMMON_RUNTIME_HPYTRACKER_H

#include "hpy.h"

typedef struct _HPyTracker_s * HPyTracker;

HPyAPI_RUNTIME_FUNC(HPyTracker)
HPyTracker_New(HPyContext ctx);

HPyAPI_RUNTIME_FUNC(HPyTracker)
HPyTracker_NewWithSize(HPyContext ctx, HPy_ssize_t size);

HPyAPI_RUNTIME_FUNC(int)
HPyTracker_Add(HPyContext ctx, HPyTracker hl, HPy h);

HPyAPI_RUNTIME_FUNC(int)
HPyTracker_RemoveAll(HPyContext ctx, HPyTracker hl);

HPyAPI_RUNTIME_FUNC(int)
HPyTracker_Free(HPyContext ctx, HPyTracker hl);

#endif /* HPY_COMMON_RUNTIME_HPYTRACKER_H */
