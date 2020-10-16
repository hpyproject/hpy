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
 * hl = HList_New(ctx);  // track the key-value pairs
 * if (hl == NULL)
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
 *     result = HPy_SetItem(ctx, dict, key, value);
 *     if (result < 0)
 *         goto error;
 * }
 *
 * success:
 *    HList_Free(ctx, hl);
 *    return dict;
 *
 * error:
 *    HList_Free(ctx, hl);
 *    HPy_Close(ctx, dict);
 *    HPyErr_SetString(ctx, ctx->h_ValueError, "Failed!");
 *    return HPy_NULL;
 */

#include <Python.h>
#include "hpy.h"

#define _HLIST_INITIAL_SIZE 6

struct _HList_s {
    HPy_ssize_t size;
    HPy_ssize_t next;
    HPy *handles;
};


HPyAPI_RUNTIME_FUNC(HList)
HList_New(HPyContext ctx)
{
    HList hl;
    hl = PyMem_Malloc(sizeof(struct _HList_s));
    if (hl == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    hl->handles = PyMem_Calloc(_HLIST_INITIAL_SIZE, sizeof(HPy));
    if (hl->handles == NULL) {
        PyMem_Free(hl);
        PyErr_NoMemory();
        return NULL;
    }
    hl->size = _HLIST_INITIAL_SIZE;
    hl->next = 0;
    return hl;
}

HPyAPI_RUNTIME_FUNC(int)
HList_Resize(HPyContext ctx, HList hl, HPy_ssize_t size)
{
    HPy *new_handles;
    if (size < hl->next) {
        // resizing would lose handles
        return -2;
    }
    new_handles = PyMem_Realloc(hl->handles, size * sizeof(HPy));
    if (new_handles == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    hl->size = size;
    hl->handles = new_handles;
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HList_Track(HPyContext ctx, HList hl, HPy h)
{
    if (hl->next >= hl->size) {
        if (HList_Resize(ctx, hl, hl->size == 0 ? 1 : hl->size * 2) < 0)
            return -1;
    }
    hl->handles[hl->next++] = h;
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HList_UntrackAll(HPyContext ctx, HList hl)
{
    hl->next = 0;
    return 0;
}

HPyAPI_RUNTIME_FUNC(int)
HList_Free(HPyContext ctx, HList hl)
{
    HPy_ssize_t i;
    for (i=0; i<hl->next; i++) {
        HPy_Close(ctx, hl->handles[i]);
    }
    PyMem_Free(hl->handles);
    PyMem_Free(hl);
    return 0;
}
