/**
 * A manager for a list of HPy handles, allowing handles to be tracked
 * and closed as a group.
 *
 * Note::
 *    HList always keeps space for one extra handle free so that
 *    HList_Track can always store the handle being passed to it,
 *    even if it fails to automatically create space for future
 *    handles. This allows HList_Free to close all handles passed to
 *    HList_Track.
 *
 *    As a consequence, the minimum size for an HList is one handle.
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
 *     if (HList_Track(ctx, hl, key) < 0)
 *         goto error;
 *     value = HPyLong_FromLong(ctx, i * i);
 *     if (HPy_IsNull(value)) {
 *         goto error;
 *     }
 *     if (HList_Track(ctx, hl, value) < 0)
 *         goto error;
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

#define _HLIST_INITIAL_SIZE (5 + 1)

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
    if (size <= hl->next) {
        // refuse a resize that would either 1) lose handles or  2) not leave
        // space for one new handle
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
    hl->handles[hl->next++] = h;
    if (hl->size <= hl->next) {
        if (HList_Resize(ctx, hl, hl->size * 2) < 0)
            return -1;
    }
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
