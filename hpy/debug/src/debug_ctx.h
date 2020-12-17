#ifndef HPY_DEBUG_CTX_H
#define HPY_DEBUG_CTX_H

// this header is for internal use. The public API of the debug mode is in
// hpy_debug.h

#include <assert.h>
#include "hpy.h"

// "deboff" is the closest to "debug" I could come up with :)
static const long HPY_DEBUG_MAGIC = 0xDEB0FF;

typedef struct {
    long magic_number; // used just for sanity checks
    HPyContext original_ctx;
} HPyDebugInfo;

static HPyDebugInfo *get_info(HPyContext ctx)
{
    HPyDebugInfo *info = (HPyDebugInfo*)ctx->_private;
    assert(info->magic_number == HPY_DEBUG_MAGIC); // sanity check
    return info;
}


#endif /* HPY_DEBUG_CTX_H */
