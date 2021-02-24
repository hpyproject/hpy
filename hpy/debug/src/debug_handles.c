#include "debug_internal.h"

static void closed_handles_append(HPyDebugInfo *info, DebugHandle *handle);

DHPy DHPy_wrap(HPyContext dctx, UHPy uh)
{
    UHPy_sanity_check(uh);
    if (HPy_IsNull(uh))
        return HPy_NULL;
    HPyDebugInfo *info = get_info(dctx);
    DebugHandle *handle = malloc(sizeof(DebugHandle));
    if (handle == NULL) {
        return HPyErr_NoMemory(info->uctx);
    }
    handle->uh = uh;
    handle->generation = info->current_generation;
    handle->is_closed = 0;
    handle->prev = NULL;
    handle->next = info->open_handles;
    if (info->open_handles)
        info->open_handles->prev = handle;
    info->open_handles = handle;
    return as_DHPy(handle);
}

void DHPy_close(HPyContext dctx, DHPy dh)
{
    DHPy_sanity_check(dh);
    if (HPy_IsNull(dh))
        return;
    HPyDebugInfo *info = get_info(dctx);
    DebugHandle *handle = as_DebugHandle(dh);

    // remove the handle from the open_handles list
    if (handle->prev)
        handle->prev->next = handle->next;
    if (handle->next)
        handle->next->prev = handle->prev;
    if (info->open_handles == handle)
        info->open_handles = handle->next;

    // put the handle in the closed_handles queue
    // XXX: we should check the size of the queue and free the tail if we have
    // reached the max size
    handle->is_closed = true;
    closed_handles_append(info, handle);
    //DHPy_free(dh);
}

static void closed_handles_append(HPyDebugInfo *info, DebugHandle *handle)
{
    if (info->closed_handles_tail == NULL) {
        assert(info->closed_handles_head == NULL);
        assert(info->closed_handles_queue_size == 0);
        info->closed_handles_head = handle;
        info->closed_handles_tail = handle;
        info->closed_handles_queue_size = 1;
        handle->prev = NULL;
        handle->next = NULL;
        return;
    }
    assert(info->closed_handles_tail->next == NULL);
    DebugHandle *tail = info->closed_handles_tail;
    tail->next = handle;
    handle->prev = tail;
    handle->next = NULL;
    info->closed_handles_tail = handle;
    info->closed_handles_queue_size += 1;
}

void DHPy_free(DHPy dh)
{
    DHPy_sanity_check(dh);
    DebugHandle *handle = as_DebugHandle(dh);
    // this is not strictly necessary, but it increases the chances that you
    // get a clear segfault if you use a freed handle
    handle->uh = HPy_NULL;
    free(handle);
}
