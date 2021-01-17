/* Internal header for all the files in hpy/debug/src. The public API is in
   include/hpy_debug.h
*/
#ifndef HPY_DEBUG_INTERNAL_H
#define HPY_DEBUG_INTERNAL_H

#include <assert.h>
#include "hpy.h"
#include "hpy_debug.h"

#define HPY_DEBUG_MAGIC 0xDEB00FF

/* The Debug context is a wrapper around an underlying context, which we will
   call Universal. Inside the debug mode we manipulate handles which belongs
   to both contexts, so to make things easier we create two typedefs to make
   it clear what kind of handle we expect: UHPy and DHPy:

     * UHPy are opaque from our point of view.

     * DHPy are actually DebugHandle* in disguise. DebugHandles are wrappers
       around a UHPy, with a bunch of extra info.

   To cast between DHPy and DebugHandle*, use as_DebugHandle and as_DHPy:
   these are just no-op casts.

   To wrap a UHPy, call DHPy_wrap: this contains some actual logic, because it
   malloc()s a new DebugHandle, which will be released at some point in the
   future after we call HPy_Close on it.  Note that if you call DHPy_wrap
   twice on the same UHPy, you get two different DHPy.

   To unwrap a DHPy and get the underyling UHPy, call DHPy_unwrap. If you call
   DHPy_unwrap multiple times on the same DHPy, you always get the same UHPy.

   WARNING: both UHPy and DHPy are alias of HPy, so we need to take care of
   not mixing them, because the compiler cannot help.

   Each DebugHandle has a "generation", which is just a int to be able to get
   only the handles which were created after a certain point.
*/

typedef HPy UHPy;
typedef HPy DHPy;

typedef struct DebugHandle {
    UHPy uh;
    long generation;
    struct DebugHandle *prev;
    struct DebugHandle *next;
} DebugHandle;

static inline DebugHandle * as_DebugHandle(DHPy dh) {
    return (DebugHandle *)dh._i;
}

static inline DHPy as_DHPy(DebugHandle *handle) {
    return (DHPy){(HPy_ssize_t)handle};
}

DHPy DHPy_wrap(HPyContext ctx, UHPy uh);
void DHPy_close(HPyContext ctx, DHPy dh);
void DHPy_free(DHPy dh);

static inline UHPy DHPy_unwrap(DHPy dh) {
    if (HPy_IsNull(dh))
        return HPy_NULL;
    return as_DebugHandle(dh)->uh;
}

/* === HPyDebugInfo === */

typedef struct {
    long magic_number; // used just for sanity checks
    HPyContext uctx;
    long current_generation;
    DebugHandle *open_handles;   // linked list
    //DebugHandle *closed_handles; // linked list
} HPyDebugInfo;

static inline HPyDebugInfo *get_info(HPyContext ctx)
{
    HPyDebugInfo *info = (HPyDebugInfo*)ctx->_private;
    assert(info->magic_number == HPY_DEBUG_MAGIC); // sanity check
    return info;
}


#endif /* HPY_DEBUG_INTERNAL_H */
