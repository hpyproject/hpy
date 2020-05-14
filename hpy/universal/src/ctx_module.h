#ifndef HPY_CTX_MODULE_H
#define HPY_CTX_MODULE_H

#include <Python.h>
#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE HPy ctx_Module_Create(HPyContext ctx, HPyModuleDef *hpydef);
HPyAPI_STORAGE PyMethodDef *create_method_defs(HPyMethodDef *hpymethods);

#endif /* HPY_CTX_MODULE_H */
