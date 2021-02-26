#include "debug_internal.h"

#include <stdio.h>

static void closed_handles_append(HPyDebugInfo *info, DebugHandle *handle);
static DebugHandle *closed_handles_popfront(HPyDebugInfo *info);

static void linked_item_sanity_check(DebugHandle *h) {
    if (h == NULL)
        return;
    if (h->next != NULL)
        assert(h->next->prev == h);
    if (h->prev != NULL)
        assert(h->prev->next == h);
}

static void debug_info_sanity_check(HPyDebugInfo *info) {
#ifndef NDEBUG
    // check that the open_handles list is sane
    if (info->open_handles != NULL)
        assert(info->open_handles->prev == NULL);
    DebugHandle *h = info->open_handles;
    while(h != NULL) {
        linked_item_sanity_check(h);
        h = h->next;
    }
    // check that the closed_handles queue is sane
    if (info->closed_handles_head == NULL || info->closed_handles_tail == NULL) {
        assert(info->closed_handles_head == NULL);
        assert(info->closed_handles_tail == NULL);
        assert(info->closed_handles_queue_size == 0);
    }
    else {
        assert(info->closed_handles_head->prev == NULL);
        assert(info->closed_handles_tail->next == NULL);
        assert(info->closed_handles_queue_size > 0);
        h = info->closed_handles_head;
        HPy_ssize_t size = 0;
        while(h != NULL) {
            linked_item_sanity_check(h);
            if (h->next == NULL)
                assert(h == info->closed_handles_tail);
            h = h->next;
            size++;
        }
        assert(info->closed_handles_queue_size == size);
    }
#endif
}


DHPy DHPy_wrap(HPyContext dctx, UHPy uh)
{
    UHPy_sanity_check(uh);
    if (HPy_IsNull(uh))
        return HPy_NULL;
    HPyDebugInfo *info = get_info(dctx);
    
    // if the closed_handles queue is full, let's reuse one of those. Else,
    // malloc a new one
    DebugHandle *handle = NULL;
    if (info->closed_handles_queue_size >= info->closed_handles_queue_max_size) {
        handle = closed_handles_popfront(info);
    }
    else {
        handle = malloc(sizeof(DebugHandle));
        if (handle == NULL) {
            return HPyErr_NoMemory(info->uctx);
        }
    }

    handle->uh = uh;
    handle->generation = info->current_generation;
    handle->is_closed = 0;
    handle->prev = NULL;
    handle->next = info->open_handles;
    if (info->open_handles)
        info->open_handles->prev = handle;
    info->open_handles = handle;
    debug_info_sanity_check(info);
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
    handle->is_closed = true;
    closed_handles_append(info, handle);
    if (info->closed_handles_queue_size > info->closed_handles_queue_max_size) {
        // we have too many closed handles. Let's free the oldest one
        DebugHandle *oldest = info->closed_handles_head;
        info->closed_handles_head = oldest->next;
        info->closed_handles_queue_size--;
        DHPy_free(as_DHPy(oldest));
    }
    debug_info_sanity_check(info);
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
        debug_info_sanity_check(info);
        return;
    }
    assert(info->closed_handles_tail->next == NULL);
    DebugHandle *tail = info->closed_handles_tail;
    tail->next = handle;
    handle->prev = tail;
    handle->next = NULL;
    info->closed_handles_tail = handle;
    info->closed_handles_queue_size++;
    debug_info_sanity_check(info);
}

static DebugHandle *closed_handles_popfront(HPyDebugInfo *info)
{
    assert(info->closed_handles_queue_size > 0);
    assert(info->closed_handles_head != NULL);
    DebugHandle *head = info->closed_handles_head;
    info->closed_handles_head = head->next;
    info->closed_handles_head->prev = NULL;
    info->closed_handles_queue_size--;
    debug_info_sanity_check(info);
    return head;
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
