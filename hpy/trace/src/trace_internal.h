/* Internal header for all the files in hpy/debug/src. The public API is in
   include/hpy_debug.h
*/
#ifndef HPY_TRACE_INTERNAL_H
#define HPY_TRACE_INTERNAL_H

#include <assert.h>
#include <time.h>
#include "hpy.h"
#include "hpy_trace.h"

#define HPY_TRACE_MAGIC 0xF00BAA5

/* === HPyTraceInfo === */

typedef struct {
    long magic_number; // used just for sanity checks
    HPyContext *uctx;
    /* call count of the corresponding HPy API function */
    uint64_t *call_counts;
    /* durations (ns) spent in the corresponding HPy API function */
    int64_t *durations;
    HPy on_enter_func;
    HPy on_exit_func;
} HPyTraceInfo;

static inline HPyTraceInfo *get_info(HPyContext *tctx)
{
    HPyTraceInfo *info = (HPyTraceInfo*)tctx->_private;
    assert(info->magic_number == HPY_TRACE_MAGIC); // sanity check
    return info;
}

HPyTraceInfo *hpy_trace_on_enter(HPyContext *tctx, int id);
void hpy_trace_on_exit(HPyTraceInfo *info, int id, int cr,
        struct timespec *_ts_start, struct timespec *_ts_end);

#endif /* HPY_TRACE_INTERNAL_H */
