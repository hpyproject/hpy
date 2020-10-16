#ifndef HPY_COMMON_RUNTIME_HLIST_H
#define HPY_COMMON_RUNTIME_HLIST_H

#include "hpy.h"

typedef struct _HList_s * HList;

HPyAPI_RUNTIME_FUNC(HList)
HList_New(HPyContext ctx, HPy_ssize_t size);

HPyAPI_RUNTIME_FUNC(int)
HList_Track(HPyContext ctx, HList hl, HPy h);

HPyAPI_RUNTIME_FUNC(int)
HList_CloseAll(HPyContext ctx, HList hl);

HPyAPI_RUNTIME_FUNC(int)
HList_Free(HPyContext ctx, HList hl);

#endif /* HPY_COMMON_RUNTIME_HLIST_H */
