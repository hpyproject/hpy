#include "debug_internal.h"

DHPy DHPy_wrap(HPyContext dctx, UHPy uh)
{
    UHPy_sanity_check(uh);
    if (HPy_IsNull(uh))
        return HPy_NULL;
    HPyDebugInfo *info = get_info(dctx);

    // if the closed_handles queue is full, let's reuse one of those. Else,
    // malloc a new one
    DebugHandle *handle = NULL;
    /*
    if (info->closed_handles_queue_size >= info->closed_handles_queue_max_size) {
        handle = closed_handles_popfront(info);
    }
    else */ {
        handle = malloc(sizeof(DebugHandle));
        if (handle == NULL) {
            return HPyErr_NoMemory(info->uctx);
        }
    }

    handle->uh = uh;
    handle->generation = info->current_generation;
    handle->is_closed = 0;
    DHQueue_append(&info->open_handles, handle);
    return as_DHPy(handle);
}

void DHPy_close(HPyContext dctx, DHPy dh)
{
    DHPy_sanity_check(dh);
    if (HPy_IsNull(dh))
        return;
    HPyDebugInfo *info = get_info(dctx);
    DebugHandle *handle = as_DebugHandle(dh);

    // move the handle from open_handles to closed_handles
    DHQueue_remove(&info->open_handles, handle);
    DHQueue_append(&info->closed_handles, handle);
    handle->is_closed = true;

    if (info->closed_handles.size > info->closed_handles_queue_max_size) {
        // we have too many closed handles. Let's free the oldest one
        DebugHandle *oldest = DHQueue_popfront(&info->closed_handles);
        DHPy_free(as_DHPy(oldest));
    }
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
