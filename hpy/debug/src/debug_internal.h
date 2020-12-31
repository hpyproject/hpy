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
   call Universal. The signatures of API functions are the same, but the HPy
   types represents very different things:

     * HPys belonging by the Universal ctx are opaque (from our point of view)
     * HPys belonging by the Debug ctx are pointers to a struct called DHPy_s

   Every DHPy is a wrapper around an universal HPy. To get the underlying
   universal HPy, you can use its ->h field.

   To wrap an universal HPy into a DHPy, you need to call DHPy_new: this
   function should be called only ONCE for each handle, and only if it
   "fresh": it will record the DHPy into a list of open handles so that it can
   be checked later.

   To avoid confusion and to prevent passing wrong handles by mistake, all the
   various debug_ctx_* functions take and return DHPys. Inside
   autogen_ctx_def.h there are no-op adapters which converts the DHPys into
   HPys to make the C compiler happy. See also the corresponding comment
   there. _d2h and _h2d "cast" a DHPy into an HPy, and they should be used
   ONLY by the adapters.
*/

struct DHPy_s {
    HPy h;
    struct DHPy_s *next;
};
typedef struct DHPy_s *DHPy; /* "Debug HPy" */

DHPy DHPy_new(HPyContext ctx, HPy h);


/* ======================================================== */
/* These two functions should be used ONLY be the adapters! */
static inline HPy _d2h(DHPy dh) {
    return (HPy){ ._i = (HPy_ssize_t)dh };
}
static inline DHPy _h2d(HPy h) {
    return (DHPy)h._i;
}
/* ======================================================== */


typedef struct {
    long magic_number; // used just for sanity checks
    HPyContext original_ctx;
    DHPy open_handles;   // linked list
    DHPy closed_handles; // linked list
} HPyDebugInfo;

static inline HPyDebugInfo *get_info(HPyContext ctx)
{
    HPyDebugInfo *info = (HPyDebugInfo*)ctx->_private;
    assert(info->magic_number == HPY_DEBUG_MAGIC); // sanity check
    return info;
}


#endif /* HPY_DEBUG_INTERNAL_H */
