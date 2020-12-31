#include "debug_internal.h"

DHPy DHPy_new(HPyContext ctx, HPy h)
{
    HPyDebugInfo *info = get_info(ctx);
    DHPy dh = malloc(sizeof(struct DHPy_s));
    dh->h = h;
    dh->next = info->open_handles;
    info->open_handles = dh;
    return dh;
}

