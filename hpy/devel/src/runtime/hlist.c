/**
 * A manager for a list of HPy handles, allowing handles to be tracked
 * and closed as a group.
 *
 * Example usage (inside an HPyDef_METH function)::
 *
 * long i;
 * HPy key, value;
 * HList hl;
 *
 * hl = HList_New(ctx, 5 * 2);  // track 5 key-value pairs
 * if (HList_IsNull(hl))
 *     return HPy_NULL;
 *
 * HPy dict = HPyDict_New(ctx);
 * if (HPy_IsNull(dict))
 *     goto error;
 *
 * for (i=0; i<5; i++) {
 *     key = HPyLong_FromLong(ctx, i);
 *     if (HPy_IsNull(key))
 *         goto error;
 *     HList_Track(ctx, hl, key);
 *     value = HPyLong_FromLong(ctx, i * i);
 *     if (HPy_IsNull(value)) {
 *         goto error;
 *     }
 *     HList_Track(ctx, hl, value);
 * }
 *
 * success:
 *    HList_Free(ctx, hl);
 *    return dict;
 *
 * error:
 *    HList_CloseAll(ctx, hl);
 *    HList_Free(ctx, hl);
 *    return HPy_NULL;
 */

#include <Python.h>
#include "hpy.h"

HPyAPI_RUNTIME_FUNC(HList)
HList_New(HPyContext ctx, HPy_ssize_t size)
{
    HList hl = (HList) {0, 0, NULL};
    hl.handles = PyMem_Calloc(size, sizeof(HPy *));
    if (hl.handles == NULL) {
        PyErr_NoMemory();
        return hl;
    }
    hl.size = size;
    hl.next = 0;
    return hl;
}

HPyAPI_RUNTIME_FUNC(int)
HList_Track(HPyContext ctx, HList hl, HPy h)
{
    if (hl.next == hl.size) {
        return -1;
    }
    hl.handles[hl.next++] = h;
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HList_CloseAll(HPyContext ctx, HList hl)
{
    HPy_ssize_t i;
    for (i=0; i<hl.next; i++) {
        HPy_Close(ctx, hl.handles[i]);
    }
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HList_Free(HPyContext ctx, HList hl)
{
    PyMem_Free(hl.handles);
    return 0;
}
