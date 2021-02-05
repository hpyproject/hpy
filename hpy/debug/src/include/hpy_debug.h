#ifndef HPY_DEBUG_H
#define HPY_DEBUG_H

#include "hpy.h"

HPyContext hpy_debug_get_ctx(HPyContext original_ctx);

// this is the HPy init function created by HPy_MODINIT. In CPython's version
// of hpy.universal the code is embedded inside the extension, so we can call
// this function directly instead of dlopen it. This is similar to what
// CPython does for its own built-in modules
HPy HPyInit__debug(HPyContext uctx);

#endif /* HPY_DEBUG_H */
