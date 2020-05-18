#ifndef HPY_CTX_MODULE_H
#define HPY_CTX_MODULE_H

#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE HPy ctx_Module_Create(HPyContext ctx, HPyModuleDef *hpydef);

#endif /* HPY_CTX_MODULE_H */
