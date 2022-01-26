#ifndef HPY_COMMON_RUNTIME_CTX_MODULE_H
#define HPY_COMMON_RUNTIME_CTX_MODULE_H

#include <Python.h>
#include "hpy.h"
#include "hpy/hpytype.h"

_HPy_HIDDEN int call_mod_traverseproc_from_trampoline(HPyFunc_traverseproc tp_traverse,
                                                      PyObject *self,
                                                      cpy_visitproc cpy_visit,
                                                      void *cpy_arg);

#endif /* HPY_COMMON_RUNTIME_CTX_MODULE_H */
