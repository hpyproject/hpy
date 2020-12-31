/* Internal header for all the files in hpy/debug/src. The public API is in
   include/hpy_debug.h
*/
#ifndef HPY_DEBUG_INTERNAL_H
#define HPY_DEBUG_INTERNAL_H

#include <assert.h>
#include "hpy.h"
#include "hpy_debug.h"

// "debooff" is the closest to "debug" I could come up with :)
#define HPY_DEBUG_MAGIC 0xDEB00FF

typedef struct {
    long magic_number; // used just for sanity checks
    HPyContext original_ctx;
} HPyDebugInfo;

static inline HPyDebugInfo *get_info(HPyContext ctx)
{
    HPyDebugInfo *info = (HPyDebugInfo*)ctx->_private;
    assert(info->magic_number == HPY_DEBUG_MAGIC); // sanity check
    return info;
}


#endif /* HPY_DEBUG_INTERNAL_H */
