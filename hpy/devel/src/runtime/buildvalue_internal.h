#ifndef HPY_DEVEL_SRC_RUNTIME_BUILDVALUE_INTERNAL_H_
#define HPY_DEVEL_SRC_RUNTIME_BUILDVALUE_INTERNAL_H_

/*
 * This header is only for internal usage to share implementation between
 * helpers.
 */

#include "hpy.h"

HPyAPI_HELPER
HPy buildvalue_single(HPyContext *ctx, const char **fmt, va_list *values, int *needs_close);

#endif /* HPY_DEVEL_SRC_RUNTIME_BUILDVALUE_INTERNAL_H_ */
