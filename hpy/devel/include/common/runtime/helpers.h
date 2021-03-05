#ifndef HPY_COMMON_RUNTIME_HELPERS_H
#define HPY_COMMON_RUNTIME_HELPERS_H

#include "hpy.h"
#include "common/hpytype.h"

HPyAPI_RUNTIME_FUNC(int)
HPyHelpers_AddType(HPyContext ctx, HPy module, const char *name,
                  HPyType_Spec *hpyspec, HPyType_SpecParam *params);

#endif /* HPY_COMMON_RUNTIME_HELPERS_H */
