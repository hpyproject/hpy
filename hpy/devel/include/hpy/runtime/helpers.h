#ifndef HPY_COMMON_RUNTIME_HELPERS_H
#define HPY_COMMON_RUNTIME_HELPERS_H

#include "hpy.h"
#include "hpy/hpytype.h"

HPyAPI_HELPER int
HPyHelpers_AddType(HPyContext *ctx, HPy obj, const char *name,
                  HPyType_Spec *hpyspec, HPyType_SpecParam *params);

HPyAPI_HELPER HPy HPyErr_SetFromErrno(HPyContext *ctx, HPy h_type);

HPyAPI_HELPER HPy HPyErr_SetFromErrnoWithFilenameObject(HPyContext *ctx, HPy h_type, HPy filename);

#endif /* HPY_COMMON_RUNTIME_HELPERS_H */
