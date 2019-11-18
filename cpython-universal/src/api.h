#ifndef HPY_API_H
#define HPY_API_H

#include "universal/hpy.h"

#define CONSTANT_H_NULL    0
#define CONSTANT_H_NONE    1
#define CONSTANT_H__TOTAL  2

extern struct _HPyContext_s global_ctx;
HPy _py2h(PyObject *);
PyObject *_h2py(HPy);
void _hclose(HPy);

#endif /* HPY_API_H */
