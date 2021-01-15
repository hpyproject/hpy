#include "debug_internal.h"

DHPy DHPy_wrap(HPyContext ctx, UHPy uh)
{
    HPyDebugInfo *info = get_info(ctx);
    DebugHandle *handle = malloc(sizeof(DebugHandle));
    handle->uh = uh;
    handle->next = info->open_handles;
    info->open_handles = handle;
    return as_DHPy(handle);
}
