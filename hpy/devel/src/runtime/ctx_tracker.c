/**
 * A manager for HPy handles, allowing handles to be tracked
 * and closed as a group.
 *
 * Note::
 *    Internally, HPyTracker always keeps space for one extra handle so that
 *    HPyTracker_Add can always store the handle being passed to it,
 *    even if it fails to automatically create space for future
 *    handles. This allows HPyTracker_Free to close all handles passed to
 *    HPyTracker_Add.
 *
 *    Space for this extra handle is created automatically, so
 *    HPyTracker_NewWithSize(ctx, 0) will actually allocated space for one
 *    handle.
 *
 *    Calling HPyTracker_NewWithSize(ctx, n) will ensure that at least n handles
 *    can be tracked without the need for a resize.
 *
 * Example usage (inside an HPyDef_METH function)::
 *
 * long i;
 * HPy key, value;
 * HPyTracker hl;
 *
 * hl = HPyTracker_New(ctx);  // track the key-value pairs
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
 *     if (HPyTracker_Add(ctx, hl, key) < 0)
 *         goto error;
 *     value = HPyLong_FromLong(ctx, i * i);
 *     if (HPy_IsNull(value)) {
 *         goto error;
 *     }
 *     if (HPyTracker_Add(ctx, hl, value) < 0)
 *         goto error;
 *     result = HPy_SetItem(ctx, dict, key, value);
 *     if (result < 0)
 *         goto error;
 * }
 *
 * success:
 *    HPyTracker_Free(ctx, hl);
 *    return dict;
 *
 * error:
 *    HPyTracker_Free(ctx, hl);
 *    HPy_Close(ctx, dict);
 *    HPyErr_SetString(ctx, ctx->h_ValueError, "Failed!");
 *    return HPy_NULL;
 */

#include <Python.h>
#include "hpy.h"
#include "common/runtime/ctx_type.h"

static const HPy_ssize_t HPYTRACKER_INITIAL_SIZE = 5;

struct _HPyTracker_s {
    HPy_ssize_t size;
    HPy_ssize_t next;
    HPy *handles;
};


_HPy_HIDDEN HPyTracker
ctx_HPyTracker_New(HPyContext ctx)
{
    return ctx_HPyTracker_NewWithSize(ctx, HPYTRACKER_INITIAL_SIZE);
}

_HPy_HIDDEN HPyTracker
ctx_HPyTracker_NewWithSize(HPyContext ctx, HPy_ssize_t size)
{
    HPyTracker hl;
    size++;

    hl = PyMem_Malloc(sizeof(struct _HPyTracker_s));
    if (hl == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    hl->handles = PyMem_Calloc(size, sizeof(HPy));
    if (hl->handles == NULL) {
        PyMem_Free(hl);
        PyErr_NoMemory();
        return NULL;
    }
    hl->size = size;
    hl->next = 0;
    return hl;
}

static int
_HPyTracker_Resize(HPyContext ctx, HPyTracker hl, HPy_ssize_t size)
{
    HPy *new_handles;
    size++;

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

_HPy_HIDDEN int
ctx_HPyTracker_Add(HPyContext ctx, HPyTracker hl, HPy h)
{
    hl->handles[hl->next++] = h;
    if (hl->size <= hl->next) {
        if (_HPyTracker_Resize(ctx, hl, hl->size * 2 - 1) < 0)
            return -1;
    }
    return 0;
}

_HPy_HIDDEN int
ctx_HPyTracker_RemoveAll(HPyContext ctx, HPyTracker hl)
{
    hl->next = 0;
    return 0;
}

_HPy_HIDDEN int
ctx_HPyTracker_Free(HPyContext ctx, HPyTracker hl)
{
    HPy_ssize_t i;
    for (i=0; i<hl->next; i++) {
        HPy_Close(ctx, hl->handles[i]);
    }
    PyMem_Free(hl->handles);
    PyMem_Free(hl);
    return 0;
}
