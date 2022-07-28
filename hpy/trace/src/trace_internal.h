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
} HPyTraceInfo;

static inline HPyTraceInfo *get_info(HPyContext *tctx)
{
    HPyTraceInfo *info = (HPyTraceInfo*)tctx->_private;
    assert(info->magic_number == HPY_TRACE_MAGIC); // sanity check
    return info;
}

static inline int64_t
diff_ns(const struct timespec start, const struct timespec end)
{
    return ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * (int64_t)1000000000
            + ((int64_t)end.tv_nsec - (int64_t)start.tv_nsec);
}

#endif /* HPY_TRACE_INTERNAL_H */
